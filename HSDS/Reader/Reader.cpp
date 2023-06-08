// Copyright 2023 CommunityUtils Authors

#include "HsdsResource.h"
#include "Common.h"

#include <dds/DCPS/DCPS_Utils.h>
#include <dds/DCPS/JsonValueWriter.h>
#include <dds/DCPS/Observer.h>
#include <dds/DCPS/DataReaderImpl.h>

#include <curlcpp/curl_easy.h>
#include <curlcpp/curl_exception.h>
#include <curlcpp/curl_header.h>

#include <curl/curl.h>

using namespace OpenDDS;

using curl::curl_easy;
using curl::curl_easy_exception;
using curl::curlcpp_traceback;

template <typename T>
void sync(Application& application, curl_easy& easy)
{
  const std::string url = application.server_url() + application.unit<T>().endpoint;
  ACE_DEBUG((LM_INFO, "Synchronizing %C\n", url.c_str()));

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  OpenDDS::DCPS::JsonValueWriter<rapidjson::Writer<rapidjson::StringBuffer> > jvw(writer);
  jvw.begin_array();
  size_t idx = 0;
  for (const auto& element : application.unit<T>().container) {
    jvw.begin_element(idx++);
    vwrite(jvw, element);
    jvw.end_element();
  }
  jvw.end_array();
  writer.Flush();
  std::stringstream ss(buffer.GetString());
  curl::curl_ios<std::istream> input(ss);

  curl::curl_header header;
  header.add("Content-Type: application/json");

  easy.add<CURLOPT_HTTPHEADER>(header.get());
  easy.add<CURLOPT_UPLOAD>(1L);
  easy.add<CURLOPT_URL>(url.c_str());
  easy.add<CURLOPT_READFUNCTION>(input.get_function());
  easy.add<CURLOPT_READDATA>(input.get_stream());

  application.increment_transaction();
  easy.perform();
  // TODO: Error handling.
  // TODO: Handle output.
}

void synchronize(Application& application, curl_easy& easy)
{
  struct curl_header *header;
  if (CURLHE_OK != curl_easy_header(easy.get_curl(), "Transaction", 0, CURLH_HEADER, -1, &header)) {
    // TODO: Error handling.
    return;
  }

  // TODO: Check parse.
  std::stringstream ss(header->value);
  uint64_t transaction;
  ss >> transaction;

  if (transaction == application.transaction()) {
    return;
  }
  application.transaction(transaction);

  sync<HSDS3::Service>(application, easy);
  sync<HSDS3::Phone>(application, easy);
  sync<HSDS3::Schedule>(application, easy);
  sync<HSDS3::ServiceArea>(application, easy);
  sync<HSDS3::ServiceAtLocation>(application, easy);
  sync<HSDS3::Location>(application, easy);
  sync<HSDS3::Language>(application, easy);
  sync<HSDS3::Organization>(application, easy);
  sync<HSDS3::Funding>(application, easy);
  sync<HSDS3::Accessibility>(application, easy);
  sync<HSDS3::CostOption>(application, easy);
  sync<HSDS3::Program>(application, easy);
  sync<HSDS3::RequiredDocument>(application, easy);
  sync<HSDS3::Contact>(application, easy);
  sync<HSDS3::OrganizationIdentifier>(application, easy);
  sync<HSDS3::Attribute>(application, easy);
  sync<HSDS3::Metadata>(application, easy);
  sync<HSDS3::MetaTableDescription>(application, easy);
  sync<HSDS3::Taxonomy>(application, easy);
  sync<HSDS3::TaxonomyTerm>(application, easy);
  sync<HSDS3::Address>(application, easy);
}

template<typename T>
class Listener : public DDS::DataReaderListener
{
public:
  typedef Container<T> ContainerType;

  Listener(Application& application)
    : application_(application)
    , reader_(application.unit<T>().reader)
  {
    reader_->set_listener(this, DDS::DATA_AVAILABLE_STATUS);
    on_data_available(reader_);
  }

  void on_requested_deadline_missed(DDS::DataReader_ptr,
                                    const DDS::RequestedDeadlineMissedStatus&)
  {}

  void on_requested_incompatible_qos(DDS::DataReader_ptr,
                                     const DDS::RequestedIncompatibleQosStatus&)
  {}

  void on_sample_rejected(DDS::DataReader_ptr,
                          const DDS::SampleRejectedStatus&)
  {}

  void on_liveliness_changed(DDS::DataReader_ptr,
                             const DDS::LivelinessChangedStatus&)
  {}

  void on_data_available(DDS::DataReader_ptr)
  {
    ACE_GUARD(ACE_Thread_Mutex, g, application_.get_mutex());

    typename OpenDDS::DCPS::DDSTraits<T>::MessageSequenceType datas;
    DDS::SampleInfoSeq infos;

    const DDS::ReturnCode_t error =
      reader_->take(datas, infos, DDS::LENGTH_UNLIMITED,
                    DDS::NOT_READ_SAMPLE_STATE, DDS::ANY_VIEW_STATE, DDS::ANY_INSTANCE_STATE);
    if (error != DDS::RETCODE_OK && error != DDS::RETCODE_NO_DATA) {
      ACE_ERROR((LM_ERROR, "ERROR: take failed %C\n", OpenDDS::DCPS::retcode_to_string(error)));
    }

    for (CORBA::ULong idx = 0; idx != infos.length(); ++idx) {
      const T& data = datas[idx];
      const DDS::SampleInfo& info = infos[idx];

      if (info.instance_state != DDS::ALIVE_INSTANCE_STATE) {
        application_.unit<T>().container.erase(data);
      } else if (info.valid_data) {
        application_.unit<T>().container.insert(data);
      }

      if (!application_.server_url().empty()) {
        const std::string url = application_.server_url() + application_.unit<T>().endpoint + "/" + data.dpmgid() + "/" + data.id();

        if (info.instance_state != DDS::ALIVE_INSTANCE_STATE) {
          // DELETE
          try {
            curl_easy easy;
            easy.add<CURLOPT_URL>(url.c_str());
            easy.add<CURLOPT_CUSTOMREQUEST>("DELETE");

            application_.increment_transaction();
            easy.perform();
            synchronize(application_, easy);
            // TODO: Handle output.
          } catch (curl_easy_exception& error) {
            // TODO: Error handling.
            // If you want to get the entire error stack we can do:
            curlcpp_traceback errors = error.get_traceback();
            // Otherwise we could print the stack like this:
            error.print_traceback();
          }
        } else if (info.valid_data) {
          // PUT
          try {
            curl::curl_header header;
            header.add("Content-Type: application/json");

            std::stringstream ss(OpenDDS::DCPS::to_json(data));
            curl::curl_ios<std::istream> input(ss);

            curl_easy easy;
            easy.add<CURLOPT_URL>(url.c_str());
            easy.add<CURLOPT_HTTPHEADER>(header.get());
            easy.add<CURLOPT_UPLOAD>(1L);
            easy.add<CURLOPT_READFUNCTION>(input.get_function());
            easy.add<CURLOPT_READDATA>(input.get_stream());

            application_.increment_transaction();
            easy.perform();
            synchronize(application_, easy);
            // TODO: Handle output.
          } catch (curl_easy_exception& error) {
            // TODO: Error handling.
            // If you want to get the entire error stack we can do:
            curlcpp_traceback errors = error.get_traceback();
            // Otherwise we could print the stack like this:
            error.print_traceback();
          }
        }
      }
    }
  }

  void on_subscription_matched(DDS::DataReader_ptr,
                               const DDS::SubscriptionMatchedStatus&)
  {}

  void on_sample_lost(DDS::DataReader_ptr,
                      const DDS::SampleLostStatus&)
  {}

private:
  Application& application_;
  typename OpenDDS::DCPS::DDSTraits<T>::DataReaderType::_var_type reader_;
};

struct DataPoint {
  DCPS::SystemTimePoint capture_time;
  size_t organization_count;
  size_t location_count;
  size_t service_count;
  long organization_writer_count;
  long location_writer_count;
  long service_writer_count;
};

class Stats {
public:
  Stats(DCPS::TimeDuration period) : period_(period) {}

  void insert(DataPoint dp)
  {
    ACE_GUARD(ACE_Thread_Mutex, g, datapoints_mutex_);
    datapoints_.push_back(dp);
  }

  vector<DataPoint> query(DCPS::TimeDuration duration)
  {
    const DCPS::SystemTimePoint now = DCPS::SystemTimePoint::now();

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, datapoints_mutex_, vector<DataPoint>());
    if (now.value() <= duration.value()) {
      return datapoints_;
    }

    vector<DataPoint> result;
    const DCPS::SystemTimePoint start_timepoint = now - duration;
    for (const_iterator it = datapoints_.rbegin(); it != datapoints_.rend(); ++it) {
      if (it->capture_time < start_timepoint) {
        break;
      }
      result.insert(result.begin(), *it);
    }
    return result;
  }

  vector<DataPoint> query()
  {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, datapoints_mutex_, vector<DataPoint>());
    return datapoints_;
  }

private:
  DCPS::TimeDuration period_;
  ACE_Thread_Mutex datapoints_mutex_;
  vector<DataPoint> datapoints_;
};

Stats stats;

void collect_datapoint(const Application& application)
{
  DataPoint dp;
  dp.capture_time = DCPS::SystemTimePoint::now();

  ACE_GUARD(ACE_Thread_Mutex, g, application.get_mutex());
  const Unit<HSDS3::Organization>& organization_unit = application.unit<HSDS3::Organization>();
  const Unit<HSDS3::Location>& location_unit = application.unit<HSDS3::Location>();
  const Unit<HSDS3::Service>& service_unit = application.unit<HSDS3::Service>();
  dp.organization_count = organization_unit.container.size();
  dp.location_count = location_unit.container.size();
  dp.service_count = service_unit.container.size();

  DDS::InstanceHandleSeq handles;
  if (organization_unit.reader->get_matched_publications(handles) == DDS::RETCODE_OK) {
    dp.organization_writer_count = handles.length();
  } else {
    // Writer count information is not available.
    dp.organization_writer_count = -1;
  }

  if (location_unit.reader->get_matched_publications(handles) == DDS::RETCODE_OK) {
    dp.location_writer_count = handles.length();
  } else {
    dp.location_writer_count = -1;
  }

  if (service_unit.reader->get_matched_publications(handles) == DDS::RETCODE_OK) {
    dp.service_writer_count = handles.length();
  } else {
    dp.service_writer_count = -1;
  }

  stats.insert(dp);
}

// TODO(sonndinh): Spawn a separate thread to run this function.
void run_stats(const Application& application)
{
  GuardWrapper wrapper;
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;

  bool keep_going = true;
  while (keep_going) {
    DDS::ReturnCode_t ret = ws->wait(active, application.reader_stats_interval());
    if (ret == DDS::RETCODE_TIMEOUT) {
      collect_datapoint(application);
    } else if (ret != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: run_stats: wait failed!\n"));
    }

    for (unsigned int i = 0; keep_going && i < active.length(); ++i) {
      if (active[i] == wrapper.guard()) {
        keep_going = false;
      }
    }
  }

  ws->detach_condition(wrapper.guard());
}

class StatsResource : public httpserver::http_resource {
public:
  StatsResource(httpserver::webserver& ws)
  {
    disallow_all();
    set_allowing("GET", true);
    ws.register_resource("/hsds3/organization/statistics", this);
  }

  const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& request)
  {
    const vector<DataPoint> data = stats.query();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    DCPS::JsonValueWriter<rapidjson::Writer<rapidjson::StringBuffer> > jvw(writer);

    jvw.begin_sequence();
    for (size_t i = 0; i < data.size(); ++i) {
      jvw.begin_element(i);
      jvw.begin_struct();
      jvw.begin_struct_member(XTypes::MemberDescriptorImpl("timestamp", false));
      const ACE_CDR::Float timestamp = data[i].capture_time.value().get_msec() * (1.0/1000);
      jvw.write_float32(timestamp);
      jvw.end_struct_member();
      jvw.begin_struct_member(XTypes::MemberDescriptorImpl("count", false));
      jvw.write_uint64(data[i].organization_count);
      jvw.end_struct_member();
      jvw.begin_struct_member(XTypes::MemberDescriptorImpl("writers", false));
      jvw.write_int32(data[i].organization_writer_count);
      jvw.end_struct_member();
      jvw.end_struct();
      jvw.end_element();
    }
    jvw.end_sequence();
    writer.Flush();

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(buffer.GetString(),
                                                                                      httpserver::http::http_utils::http_ok,
                                                                                      "application/json"));
  }
}

int
ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  Application application;
  application.load_environment_variables();
  application.dump_configuration();
  if (application.download_security_documents() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }
  if (application.download_relay_config() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }
  if (application.setup_discovery() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }
  if (application.setup_transport() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }
  application.setup_security_plugin();
  if (application.create_participant(argc, argv) != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }
  application.install_observer();
  if (application.create_topics() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }
  if (application.create_datareaders() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }

  // Provide endpoints to get data out of the reader.
  httpserver::webserver ws = httpserver::create_webserver(application.http_port());

  HsdsResource<HSDS3::Service> service_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Phone> phone_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Schedule> schedule_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::ServiceArea> service_area_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::ServiceAtLocation> service_at_location_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Location> location_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Language> language_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Organization> organization_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Funding> funding_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Accessibility> accessibility_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::CostOption> cost_option_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Program> program_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::RequiredDocument> required_document_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Contact> contact_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::OrganizationIdentifier> organization_identifier_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Attribute> attribute_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Metadata> metadata_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::MetaTableDescription> meta_table_description_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Taxonomy> taxonomy_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::TaxonomyTerm> taxonomy_term_hsds_resource(application, ws, false);
  HsdsResource<HSDS3::Address> address_hsds_resource(application, ws, false);

  StatsResource stats_resource(ws);

  ws.start(false);

  // Set up listeners for DDS samples.
  DDS::DataReaderListener_var service_listener(new Listener<HSDS3::Service> (application));
  DDS::DataReaderListener_var phone_listener(new Listener<HSDS3::Phone> (application));
  DDS::DataReaderListener_var schedule_listener(new Listener<HSDS3::Schedule> (application));
  DDS::DataReaderListener_var service_area_listener(new Listener<HSDS3::ServiceArea> (application));
  DDS::DataReaderListener_var service_at_location_listener(new Listener<HSDS3::ServiceAtLocation> (application));
  DDS::DataReaderListener_var location_listener(new Listener<HSDS3::Location> (application));
  DDS::DataReaderListener_var language_listener(new Listener<HSDS3::Language> (application));
  DDS::DataReaderListener_var organization_listener(new Listener<HSDS3::Organization> (application));
  DDS::DataReaderListener_var funding_listener(new Listener<HSDS3::Funding> (application));
  DDS::DataReaderListener_var accessibility_listener(new Listener<HSDS3::Accessibility> (application));
  DDS::DataReaderListener_var cost_option_listener(new Listener<HSDS3::CostOption> (application));
  DDS::DataReaderListener_var program_listener(new Listener<HSDS3::Program> (application));
  DDS::DataReaderListener_var required_document_listener(new Listener<HSDS3::RequiredDocument> (application));
  DDS::DataReaderListener_var contact_listener(new Listener<HSDS3::Contact> (application));
  DDS::DataReaderListener_var organization_identifier_listener(new Listener<HSDS3::OrganizationIdentifier> (application));
  DDS::DataReaderListener_var attribute_listener(new Listener<HSDS3::Attribute> (application));
  DDS::DataReaderListener_var metadata_listener(new Listener<HSDS3::Metadata> (application));
  DDS::DataReaderListener_var meta_table_description_listener(new Listener<HSDS3::MetaTableDescription> (application));
  DDS::DataReaderListener_var taxonomy_listener(new Listener<HSDS3::Taxonomy> (application));
  DDS::DataReaderListener_var taxonomy_term_listener(new Listener<HSDS3::TaxonomyTerm> (application));
  DDS::DataReaderListener_var address_listener(new Listener<HSDS3::Address> (application));

  GuardWrapper wrapper;
  DDS::WaitSet_var waitset = new DDS::WaitSet;
  waitset->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;
  const DDS::Duration_t period = application.server_poll_period();

  bool keep_going = true;
  while (keep_going) {
    DDS::ReturnCode_t error = waitset->wait(active, period);
    if (error == DDS::RETCODE_TIMEOUT) {
      ACE_GUARD_RETURN(ACE_Thread_Mutex, g, application.get_mutex(), EXIT_FAILURE);

      if (!application.server_url().empty()) {
        try {
          curl_easy easy;
          easy.add<CURLOPT_URL>((application.server_url() + application.unit<HSDS3::Organization>().endpoint + "?offset=0&count=0").c_str());

          easy.perform();
          synchronize(application, easy);
          // TODO: Handle output.
        } catch (curl_easy_exception& error) {
          // TODO: Error handling.
          // If you want to get the entire error stack we can do:
          curlcpp_traceback errors = error.get_traceback();
          // Otherwise we could print the stack like this:
          error.print_traceback();
        }
      }

    } else if (error != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: wait failed!\n"));
    }

    for (unsigned int i = 0; keep_going && i != active.length(); ++i) {
      if (active[i] == wrapper.guard()) {
        keep_going = false;
      }
    }
  }

  waitset->detach_condition(wrapper.guard());

  application.shutdown();

  return EXIT_SUCCESS;
}

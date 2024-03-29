// Copyright 2023 CommunityUtils Authors

#include "Application.h"

#include "CommunitySecurityPlugin.h"

#include <dds/DCPS/JsonValueWriter.h>
#include <dds/DCPS/RTPS/RtpsDiscovery.h>
#include <dds/DCPS/security/framework/Properties.h>
#include <dds/DCPS/transport/rtps_udp/RtpsUdpInst.h>

#include <curlcpp/curl_cookie.h>
#include <curlcpp/curl_easy.h>
#include <curlcpp/curl_exception.h>
#include <curlcpp/curl_header.h>

using curl::curl_cookie;
using curl::curlcpp_cookies;
using curl::curl_easy;
using curl::curl_easy_exception;
using curl::curlcpp_traceback;

class Observer : public OpenDDS::DCPS::Observer {
public:
  void on_sample_sent(DDS::DataWriter_ptr dw, const Sample& s)
  {
    DDS::Topic_var topic = dw->get_topic();
    CORBA::String_var topic_name = topic->get_name();
    CORBA::String_var type_name = topic->get_type_name();
    ACE_DEBUG((LM_INFO, "INFO: Observer::on_sample_sent: topic=%C type=%C sample=%C\n",
               topic_name.in(), type_name.in(), OpenDDS::DCPS::to_json(s).c_str()));
  }

  void on_sample_received(DDS::DataReader_ptr dr, const Sample& s)
  {
    DDS::TopicDescription_var topic = dr->get_topicdescription();
    CORBA::String_var topic_name = topic->get_name();
    CORBA::String_var type_name = topic->get_type_name();
    ACE_DEBUG((LM_INFO, "INFO: Observer::on_sample_received: topic=%C type=%C sample=%C\n",
               topic_name.in(), type_name.in(), OpenDDS::DCPS::to_json(s).c_str()));
  }

  void on_disposed(DDS::DataWriter_ptr dw, const Sample& s)
  {
    DDS::Topic_var topic = dw->get_topic();
    CORBA::String_var topic_name = topic->get_name();
    CORBA::String_var type_name = topic->get_type_name();
    ACE_DEBUG((LM_INFO, "INFO: Observer::on_disposed: (on writer) topic=%C type=%C sample=%C\n",
               topic_name.in(), type_name.in(), OpenDDS::DCPS::to_json(s).c_str()));
  }

  void on_disposed(DDS::DataReader_ptr dr, const Sample& s)
  {
    DDS::TopicDescription_var topic = dr->get_topicdescription();
    CORBA::String_var topic_name = topic->get_name();
    CORBA::String_var type_name = topic->get_type_name();
    ACE_DEBUG((LM_INFO, "INFO: Observer::on_disposed: (on reader) topic=%C type=%C sample=%C\n",
               topic_name.in(), type_name.in(), OpenDDS::DCPS::to_json(s).c_str()));
  }

  void on_unregistered(DDS::DataWriter_ptr dw, const Sample& s)
  {
    DDS::Topic_var topic = dw->get_topic();
    CORBA::String_var topic_name = topic->get_name();
    CORBA::String_var type_name = topic->get_type_name();
    ACE_DEBUG((LM_INFO, "INFO: Observer::on_unregistered: (on writer) topic=%C type=%C sample=%C\n",
               topic_name.in(), type_name.in(), OpenDDS::DCPS::to_json(s).c_str()));
  }

  void on_unregistered(DDS::DataReader_ptr dr, const Sample& s)
  {
    DDS::TopicDescription_var topic = dr->get_topicdescription();
    CORBA::String_var topic_name = topic->get_name();
    CORBA::String_var type_name = topic->get_type_name();
    ACE_DEBUG((LM_INFO, "INFO: Observer::on_unregistered: (on reader) topic=%C type=%C sample=%C\n",
               topic_name.in(), type_name.in(), OpenDDS::DCPS::to_json(s).c_str()));
  }
};

namespace {

  DDS::ReturnCode_t
  download_security_document(curl_easy& easy,
                             const std::string& url,
                             std::string& target)
  {
    std::ostringstream stream;
    curl::curl_ios<std::ostream> stream_ios(stream);

    easy.add<CURLOPT_URL>(url.c_str());
    easy.add<CURLOPT_WRITEFUNCTION>(stream_ios.get_function());
    easy.add<CURLOPT_WRITEDATA>(stream_ios.get_stream());

    try {
      easy.perform();
    } catch (curl_easy_exception& error) {
      ACE_ERROR((LM_ERROR, "ERROR: download_security_document failed: %C\n", error.what()));
      error.print_traceback();
      return DDS::RETCODE_ERROR;
    }

    target = stream.str();
    return DDS::RETCODE_OK;
  }

  // TODO: This should be part of OpenDDS.
  void append(DDS::PropertySeq& props, const char* name, const char* value, bool propagate = false)
  {
    const DDS::Property_t prop = {name, value, propagate};
    const unsigned int len = props.length();
    props.length(len + 1);
    props[len] = prop;
  }
}

struct SetTopicName {
  template <typename T>
  DDS::ReturnCode_t operator() (Unit<T>& unit)
  {
    const char* e = getenv(unit.topic_name_key.c_str());
    if (e) {
      unit.topic_name = e;
    }
    return DDS::RETCODE_OK;
  }
};

void
Application::load_environment_variables()
{
  const char* e;

  e = getenv("DPM_URL");
  if (e) {
    dpm_url_ = e;
  }

  e = getenv("DPM_GID");
  if (e) {
    dpm_gid_ = e;
  }

  e = getenv("DPM_APPLICATION_ID");
  if (e) {
    dpm_application_id_ = e;
  }

  e = getenv("DPM_PASSWORD");
  if (e) {
    dpm_password_ = e;
  }

  e = getenv("DPM_NONCE");
  if (e) {
    dpm_nonce_ = e;
  }

  e = getenv("RELAY_CONFIG_URL");
  if (e) {
    relay_config_url_ = e;
  }

  e = getenv("DATA_PATH");
  if (e) {
    data_path_ = e;
  }

  e = getenv("DOMAIN_ID");
  if (e) {
    domain_id_ = ACE_OS::atoi(e);
  }

  for_each(SetTopicName());

  e = getenv("HTTP_PORT");
  if (e) {
    http_port_ = ACE_OS::atoi(e);
  }

  e = getenv("CREATE_WRITERS");
  if (e) {
    create_writers_ = ACE_OS::atoi(e);
  }

  e = getenv("SERVER_URL");
  if (e) {
    server_url_ = e;
  }

  e = getenv("SERVER_POLL_PERIOD");
  if (e) {
    server_poll_period_.sec = ACE_OS::atoi(e);
  }

  e = getenv("ACCESS_CONTROL_ALLOW_ORIGIN");
  if (e) {
    access_control_allow_origin_ = e;
  }

  e = getenv("RTPS_RELAY_ONLY");
  if (e) {
    rtps_relay_only_ = ACE_OS::atoi(e);
  }

  e = getenv("ENABLE_HTTP_LOG_ACCESS");
  if (e) {
    enable_http_log_access_ = ACE_OS::atoi(e);
  }

  e = getenv("ENABLE_OBSERVER");
  if (e) {
    enable_observer_ = ACE_OS::atoi(e);
  }
}

struct LogTopicName {
  template <typename T>
  DDS::ReturnCode_t operator() (const Unit<T>& unit)
  {
    ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: %C=%C\n", unit.topic_name_key.c_str(), unit.topic_name.c_str()));
    return DDS::RETCODE_OK;
  }
};

void
Application::dump_configuration() const
{
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: DPM_URL=%C\n", dpm_url_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: DPM_GID=%C\n", dpm_gid_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: DPM_APPLICATION_ID=%C\n", dpm_application_id_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: DPM_PASSWORD=%C\n", dpm_password_.empty() ? "not set" : "set"));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: DPM_NONCE=%C\n", dpm_nonce_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: RELAY_CONFIG_URL=%C\n", relay_config_url_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: DATA_PATH=%C\n", data_path_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: DOMAIN_ID=%d\n", domain_id_));
  for_each(LogTopicName());
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: HTTP_PORT=%d\n", http_port_));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: CREATE_WRITERS=%d\n", create_writers_));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: SERVER_URL=%C\n", server_url_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: SERVER_POLL_PERIOD=%d\n", server_poll_period_.sec));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: ACCESS_CONTROL_ALLOW_ORIGIN=%C\n", access_control_allow_origin_.c_str()));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: RTPS_RELAY_ONLY=%d\n", rtps_relay_only_));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: ENABLE_HTTP_LOG_ACCESS=%d\n", enable_http_log_access_));
  ACE_DEBUG((LM_INFO, "INFO: Application::dump_configuration: ENABLE_OBSERVER=%d\n", enable_observer_));
}

DDS::ReturnCode_t
Application::download_security_documents()
{
  if (dpm_url_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: DPM_URL cannot be empty\n"));
    return DDS::RETCODE_ERROR;
  }

  if (dpm_application_id_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: DPM_APPLICATION_ID cannot be empty\n"));
    return DDS::RETCODE_ERROR;
  }

  if (dpm_password_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: DPM_PASSWORD cannot be empty\n"));
    return DDS::RETCODE_ERROR;
  }

  if (dpm_nonce_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: DPM_NONCE cannot be empty\n"));
    return DDS::RETCODE_ERROR;
  }

  try {
    curl_easy easy;

    {
      curl::curl_header header;
      header.add("Content-Type: application/json");

      rapidjson::Document credentials;
      credentials.SetObject();
      rapidjson::Value username;
      username.SetString(dpm_application_id_.c_str(), dpm_application_id_.size());
      rapidjson::Value password;
      password.SetString(dpm_password_.c_str(), dpm_password_.size());
      credentials.AddMember("username", username, credentials.GetAllocator());
      credentials.AddMember("password", password, credentials.GetAllocator());

      rapidjson::StringBuffer buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      credentials.Accept(writer);
      writer.Flush();

      std::istringstream ss(buffer.GetString());
      curl::curl_ios<std::istream> input(ss);

      easy.add<CURLOPT_URL>((dpm_url_ + "/api/login").c_str());
      easy.add<CURLOPT_COOKIEFILE>("");
      easy.add<CURLOPT_HTTPHEADER>(header.get());
      easy.add<CURLOPT_UPLOAD>(1L);
      easy.add<CURLOPT_POST>(1L);
      easy.add<CURLOPT_READFUNCTION>(input.get_function());
      easy.add<CURLOPT_READDATA>(input.get_stream());

      easy.perform();

      // Reset.
      curl::curl_header empty_header;
      easy.add<CURLOPT_HTTPHEADER>(empty_header.get());
      easy.add<CURLOPT_UPLOAD>(0L);
      easy.add<CURLOPT_HTTPGET>(1L);
      easy.add<CURLOPT_READFUNCTION>(0);
      easy.add<CURLOPT_READDATA>(0);
    }

    std::string nonce = dpm_nonce_;
    easy.escape(nonce);

    DDS::ReturnCode_t ret;
    ret = download_security_document(easy, dpm_url_ + "/api/applications/identity_ca.pem", identity_ca_);
    if (ret != DDS::RETCODE_OK) {
      return ret;
    }
    ret = download_security_document(easy, dpm_url_ + "/api/applications/permissions_ca.pem", permissions_ca_);
    if (ret != DDS::RETCODE_OK) {
      return ret;
    }
    ret = download_security_document(easy, dpm_url_ + "/api/applications/governance.xml.p7s", governance_);
    if (ret != DDS::RETCODE_OK) {
      return ret;
    }
    ret = download_security_document(easy, dpm_url_ + "/api/applications/permissions.xml.p7s?nonce=" + nonce, permissions_);
    if (ret != DDS::RETCODE_OK) {
      return ret;
    }

    {
      std::string key_pair;
      ret = download_security_document(easy, dpm_url_ + "/api/applications/key_pair?nonce=" + nonce, key_pair);
      if (ret != DDS::RETCODE_OK) {
        return ret;
      }

      rapidjson::Document document;
      document.Parse(key_pair.c_str());

      if (document.IsObject() &&
          document.HasMember("public") &&
          document["public"].IsString() &&
          document.HasMember("private") &&
          document["private"].IsString()) {
        public_key_ = document["public"].GetString();
        private_key_ = document["private"].GetString();
      } else {
        ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: Could not extract public and private key\n"));
        return DDS::RETCODE_ERROR;
      }
    }

    {
      verifier_.with_subject(dpm_application_id_);

      std::string public_keys;
      ret = download_security_document(easy, dpm_url_ + "/auth/public_keys", public_keys);
      if (ret != DDS::RETCODE_OK) {
        return ret;
      }

      bool have_key = false;
      rapidjson::Document document;
      document.Parse(public_keys.c_str());
      if (document.IsArray()) {
        for (rapidjson::SizeType idx = 0; idx != document.Size(); ++idx) {
          if (document[idx].IsObject() &&
              document[idx].HasMember("public") &&
              document[idx]["public"].IsString()) {
            verifier_.allow_algorithm(jwt::algorithm::rs256(document[idx]["public"].GetString()));
            have_key = true;
          } else {
            ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: DPM public key is not a string\n"));
            return DDS::RETCODE_ERROR;
          }
        }
      } else {
        ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: Could not extract DPM public keys\n"));
        return DDS::RETCODE_ERROR;
      }

      if (!have_key) {
        ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: No DPM public keys\n"));
        return DDS::RETCODE_ERROR;
      }
    }

    return DDS::RETCODE_OK;
  } catch (curl_easy_exception& error) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::download_security_documents: %C\n", error.what()));
    error.print_traceback();
    return DDS::RETCODE_ERROR;
  }
}

DDS::ReturnCode_t
Application::download_relay_config()
{
  if (relay_config_url_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::download_relay_config: RELAY_CONFIG_URL cannot be empty\n"));
    return DDS::RETCODE_ERROR;
  }

  try {
    curl_easy easy;

    std::ostringstream stream;
    curl::curl_ios<std::ostream> stream_ios(stream);

    easy.add<CURLOPT_URL>(relay_config_url_.c_str());
    easy.add<CURLOPT_WRITEFUNCTION>(stream_ios.get_function());
    easy.add<CURLOPT_WRITEDATA>(stream_ios.get_stream());

    easy.perform();

    rapidjson::Document document;
    document.Parse(stream.str().c_str());

    // TODO: Add support for STUN here.
    if (document.IsObject() &&
        document.HasMember("SpdpRtpsRelayAddress") &&
        document["SpdpRtpsRelayAddress"].IsString() &&
        document.HasMember("SedpRtpsRelayAddress") &&
        document["SedpRtpsRelayAddress"].IsString() &&
        document.HasMember("DataRtpsRelayAddress") &&
        document["DataRtpsRelayAddress"].IsString()) {
      spdp_rtps_relay_address_ = document["SpdpRtpsRelayAddress"].GetString();
      sedp_rtps_relay_address_ = document["SedpRtpsRelayAddress"].GetString();
      data_rtps_relay_address_ = document["DataRtpsRelayAddress"].GetString();
    } else {
      ACE_ERROR((LM_ERROR, "Could not extract Spdp/Sedp/DataRtpsRelayAddress\n"));
      return DDS::RETCODE_ERROR;
    }

    return DDS::RETCODE_OK;
  } catch (curl_easy_exception& error) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::download_relay_config: %C\n", error.what()));
    error.print_traceback();
    return DDS::RETCODE_ERROR;
  }
}

DDS::ReturnCode_t
Application::setup_discovery()
{
  if (spdp_rtps_relay_address_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::setup_discovery: SpdpRtpsRelayAddress is empty\n"));
    return DDS::RETCODE_ERROR;
  }

  if (sedp_rtps_relay_address_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::setup_discovery: SedpRtpsRelayAddress is empty\n"));
    return DDS::RETCODE_ERROR;
  }

  // Use RtpsDiscovery.
  OpenDDS::DCPS::RcHandle<OpenDDS::RTPS::RtpsDiscovery> discovery = OpenDDS::DCPS::make_rch<OpenDDS::RTPS::RtpsDiscovery>("RtpsDiscovery");
  discovery->config()->use_rtps_relay(true);
  discovery->config()->rtps_relay_only(rtps_relay_only_);
  discovery->config()->spdp_rtps_relay_address(OpenDDS::DCPS::NetworkAddress(spdp_rtps_relay_address_.c_str()));
  discovery->config()->sedp_rtps_relay_address(OpenDDS::DCPS::NetworkAddress(sedp_rtps_relay_address_.c_str()));
  discovery->config()->sedp_max_message_size(1400);
  discovery->config()->secure_participant_user_data(true);
  discovery->config()->sedp_responsive_mode(true);
  TheServiceParticipant->add_discovery(discovery);
  TheServiceParticipant->set_default_discovery(discovery->key());

  return DDS::RETCODE_OK;
}

DDS::ReturnCode_t
Application::setup_transport()
{
  if (data_rtps_relay_address_.empty()) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::setup_transport: DataRtpsRelayAddress is empty\n"));
    return DDS::RETCODE_ERROR;
  }

  transport_config_ = TheTransportRegistry->create_config("RtpsTransport");
  if (!transport_config_) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::setup_transport: create_config failed\n"));
    return DDS::RETCODE_ERROR;
  }
  OpenDDS::DCPS::TransportInst_rch instance = TheTransportRegistry->create_inst("RtpsTransport", "rtps_udp");
  if (!instance) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::setup_transport: create_inst failed\n"));
    return DDS::RETCODE_ERROR;
  }

  OpenDDS::DCPS::RcHandle<OpenDDS::DCPS::RtpsUdpInst> rtps =
    OpenDDS::DCPS::dynamic_rchandle_cast<OpenDDS::DCPS::RtpsUdpInst>(instance);
  rtps->max_message_size_ = 1400;
  rtps->responsive_mode_ = true;
  rtps->use_rtps_relay(true);
  rtps->rtps_relay_only(rtps_relay_only_);
  rtps->rtps_relay_address(OpenDDS::DCPS::NetworkAddress(data_rtps_relay_address_.c_str()));

  transport_config_->instances_.push_back(instance);

  return DDS::RETCODE_OK;
}

void
Application::setup_security_plugin()
{
  TheServiceParticipant->set_security(true);
  Community::install_community_security_plugin();
}

DDS::ReturnCode_t
Application::create_participant(int argc, ACE_TCHAR* argv[])
{
  //OpenDDS::DCPS::DCPS_debug_level = 4;
  domain_participant_factory_ = TheParticipantFactoryWithArgs(argc, argv);

  DDS::DomainParticipantQos part_qos;
  domain_participant_factory_->get_default_participant_qos(part_qos);

  DDS::PropertySeq& props = part_qos.property.value;

  using namespace DDS::Security::Properties;
  append(props, AuthIdentityCA, ("data:," + identity_ca_).c_str());
  append(props, AuthIdentityCertificate, ("data:," + public_key_).c_str());
  append(props, AuthPrivateKey, ("data:," + private_key_).c_str());
  append(props, AccessPermissionsCA, ("data:," + permissions_ca_).c_str());
  append(props, AccessGovernance, ("data:," + governance_).c_str());
  append(props, AccessPermissions, ("data:," + permissions_).c_str());

  const DDS::Duration_t one_minute = { 60, 0 };
  TheServiceParticipant->bit_autopurge_nowriter_samples_delay(one_minute);
  TheServiceParticipant->bit_autopurge_disposed_samples_delay(one_minute);

  participant_ = domain_participant_factory_->create_participant(domain_id_,
                                                                 part_qos,
                                                                 0,
                                                                 OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (!participant_) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::create_participant: create_participant failed\n"));
    return DDS::RETCODE_ERROR;
  }

  TheTransportRegistry->bind_config(transport_config_, participant_);

  return DDS::RETCODE_OK;
}

void
Application::install_observer()
{
  OpenDDS::DCPS::Observer_rch observer = OpenDDS::DCPS::make_rch<Observer>();
  dynamic_cast<OpenDDS::DCPS::EntityImpl*>(participant_.in())->set_observer(observer, Observer::e_SAMPLE_SENT | Observer::e_SAMPLE_RECEIVED | Observer::e_DISPOSED | Observer::e_UNREGISTERED);
}

class CreateTopic {
public:
  CreateTopic(DDS::DomainParticipant_var participant)
    : participant_(participant)
  {}

  template <typename T>
  DDS::ReturnCode_t operator() (Unit<T>& unit)
  {
    if (unit.topic_name.empty()) {
      ACE_ERROR((LM_ERROR, "ERROR: CreateTopic::operator(): %C cannot be empty\n", unit.topic_name_key.c_str()));
      return DDS::RETCODE_ERROR;
    }

    typedef typename OpenDDS::DCPS::DDSTraits<T>::TypeSupportImplType ImplType;
    unit.type_support = new ImplType;
    if (unit.type_support->register_type(participant_, "") != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: CreateTopic::operator(): register_type failed\n"));
      return DDS::RETCODE_ERROR;
    }
    CORBA::String_var type_name = unit.type_support->get_type_name();
    unit.topic = participant_->create_topic(unit.topic_name.c_str(),
                                            type_name,
                                            TOPIC_QOS_DEFAULT,
                                            0,
                                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (!unit.topic) {
      ACE_ERROR((LM_ERROR, "ERROR: CreateTopic::operator(): create_topic (%C) failed!\n", unit.topic_name.c_str()));
      return DDS::RETCODE_ERROR;
    }

    return DDS::RETCODE_OK;
  }

private:
  DDS::DomainParticipant_var participant_;
};

DDS::ReturnCode_t
Application::create_topics()
{
  return for_each(CreateTopic(participant_));
}

class CreateDataWriter {
public:
  CreateDataWriter(DDS::Publisher_var publisher)
    : publisher_(publisher)
  {}

  template <typename T>
  DDS::ReturnCode_t operator() (Unit<T>& unit)
  {
    DDS::DataWriter_var writer;
    writer = publisher_->create_datawriter(unit.topic,
                                           DATAWRITER_QOS_DEFAULT,
                                           0,
                                           OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (!writer) {
      ACE_ERROR((LM_ERROR, "ERROR: CreateDataWriter::operator(): create_datawriter failed!\n"));
      return DDS::RETCODE_ERROR;
    }
    unit.writer = OpenDDS::DCPS::DDSTraits<T>::DataWriterType::_narrow(writer);
    if (!unit.writer) {
      ACE_ERROR((LM_ERROR, "ERROR: CreateDataWriter::operator(): narrow failed!\n"));
      return DDS::RETCODE_ERROR;
    }

    return DDS::RETCODE_OK;
  }

private:
  DDS::Publisher_var publisher_;
};

DDS::ReturnCode_t
Application::create_datawriters()
{
  // Create a publisher.
  DDS::Publisher_var publisher = participant_->create_publisher(PUBLISHER_QOS_DEFAULT,
                                                                0,
                                                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (!publisher) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::create_datawriters: create_publisher failed!\n"));
    return DDS::RETCODE_ERROR;
  }

  DDS::DataWriterQos qos;
  if (publisher->get_default_datawriter_qos(qos) != DDS::RETCODE_OK) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::create_datawriters: get_default_datawriter_qos failed!\n"));
    return DDS::RETCODE_ERROR;
  }
  qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
  qos.durability.kind = DDS::TRANSIENT_LOCAL_DURABILITY_QOS;
  if (publisher->set_default_datawriter_qos(qos) != DDS::RETCODE_OK) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::create_datawriters: set_default_datawriter_qos failed!\n"));
    return DDS::RETCODE_ERROR;
  }

  return for_each(CreateDataWriter(publisher));
}

class CreateDataReader {
public:
  CreateDataReader(DDS::Subscriber_var subscriber)
    : subscriber_(subscriber)
  {}

  template <typename T>
  DDS::ReturnCode_t operator() (Unit<T>& unit)
  {
    DDS::DataReader_var reader;
    reader = subscriber_->create_datareader(unit.topic,
                                            DATAREADER_QOS_DEFAULT,
                                            0,
                                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (!reader) {
      ACE_ERROR((LM_ERROR, "ERROR: CreateDataReader::operator(): create_datareader failed!\n"));
      return DDS::RETCODE_ERROR;
    }
    unit.reader = OpenDDS::DCPS::DDSTraits<T>::DataReaderType::_narrow(reader);
    if (!unit.reader) {
      ACE_ERROR((LM_ERROR, "ERROR: CreateDataReader::operator(): narrow failed!\n"));
      return DDS::RETCODE_ERROR;
    }

    return DDS::RETCODE_OK;
  }

private:
  DDS::Subscriber_var subscriber_;
};

DDS::ReturnCode_t
Application::create_datareaders()
{
  // Create a subscriber.
  DDS::Subscriber_var subscriber = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                                                   0,
                                                                   OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (!subscriber) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::create_datareaders: create_subscriber failed!\n"));
    return DDS::RETCODE_ERROR;
  }

  DDS::DataReaderQos qos;
  if (subscriber->get_default_datareader_qos(qos) != DDS::RETCODE_OK) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::create_datareaders: get_default_datareader_qos failed!\n"));
    return DDS::RETCODE_ERROR;
  }
  qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
  qos.durability.kind = DDS::TRANSIENT_LOCAL_DURABILITY_QOS;
  if (subscriber->set_default_datareader_qos(qos) != DDS::RETCODE_OK) {
    ACE_ERROR((LM_ERROR, "ERROR: Application::create_datareaders: set_default_datareader_qos failed!\n"));
    return DDS::RETCODE_ERROR;
  }

  return for_each(CreateDataReader(subscriber));
}

void
Application::shutdown()
{
  participant_->delete_contained_entities();
  domain_participant_factory_->delete_participant(participant_);
  TheServiceParticipant->shutdown();
}

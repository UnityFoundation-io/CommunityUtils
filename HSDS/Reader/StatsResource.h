#ifndef HSDS_READER_STATS_RESOURCE_H
#define HSDS_READER_STATS_RESOURCE_H

#include "StatsApplication.h"

#include <dds/DCPS/JsonValueWriter.h>

#include <httpserver.hpp>

#include <string>

template <typename T>
class StatsResource : public httpserver::http_resource {
public:
  StatsResource(StatsApplication& stats_app,
                httpserver::webserver& ws,
                const std::string& endpoint)
    : stats_app_(stats_app)
  {
    disallow_all();
    set_allowing("GET", true);
    ws.register_resource(endpoint, this);
  }

  const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& /*request*/)
  {
    const std::vector<DataPoint> data = stats_app_.get_stats<T>().query();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    OpenDDS::DCPS::JsonValueWriter<rapidjson::Writer<rapidjson::StringBuffer> > jvw(writer);

    jvw.begin_sequence();
    for (size_t i = 0; i < data.size(); ++i) {
      jvw.begin_element(i);
      jvw.begin_struct();
      jvw.begin_struct_member(OpenDDS::XTypes::MemberDescriptorImpl("timestamp", false));
      const ACE_CDR::Double timestamp = data[i].timestamp.value().sec() +
        (data[i].timestamp.value().usec()*1.0)/1000000;
      jvw.write_float64(timestamp);
      jvw.end_struct_member();
      jvw.begin_struct_member(OpenDDS::XTypes::MemberDescriptorImpl("count", false));
      jvw.write_uint64(data[i].record_count);
      jvw.end_struct_member();
      jvw.begin_struct_member(OpenDDS::XTypes::MemberDescriptorImpl("writers", false));
      jvw.write_int32(data[i].writer_count);
      jvw.end_struct_member();
      jvw.end_struct();
      jvw.end_element();
    }
    jvw.end_sequence();
    writer.Flush();

    std::shared_ptr<httpserver::http_response> response(new httpserver::string_response(buffer.GetString(),
                                                                                        httpserver::http::http_utils::http_ok,
                                                                                        "application/json"));
    if (!stats_app_.application().access_control_allow_origin().empty()) {
      response->with_header("Access-Control-Allow-Origin", stats_app_.application().access_control_allow_origin());
    }

    return response;
  }

private:
  StatsApplication& stats_app_;
};

#endif // HSDS_READER_STATS_RESOURCE_H

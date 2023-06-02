#ifndef HSDS_WRITER_RESOURCE_BASE_H
#define HSDS_WRITER_RESOURCE_BASE_H

// Copyright 2023 CommunityUtils Authors

#include "Application.h"

#include <dds/DCPS/JsonValueWriter.h>

#include <httpserver.hpp>


class ErrorResponse {
public:
  ErrorResponse()
    : status_(httpserver::http::http_utils::http_bad_request)
    , message_("Bad Request")
  {}

  static ErrorResponse make_bad_request(const std::string& message)
  {
    return ErrorResponse(httpserver::http::http_utils::http_bad_request, message);
  }

  static ErrorResponse make_not_found()
  {
    return ErrorResponse(httpserver::http::http_utils::http_not_found, "Not Found");
  }

  static ErrorResponse make_internal_server_error()
  {
    return ErrorResponse(httpserver::http::http_utils::http_internal_server_error, "Internal Server Error");
  }

  int status() const { return status_; }
  const std::string& message() const { return message_; }

private:
  ErrorResponse(int status,
                const std::string& message)
    : status_(status)
    , message_(message)
  {}

  int status_;
  std::string message_;
};

class ResourceBase : public http_resource {
public:
  ResourceBase(Application& application)
    : application_(application)
  {}

protected:

  virtual void add_headers(std::shared_ptr<httpserver::http_response>) const {}

  // TODO: Could we have done this in IDL?
  bool check_input(ErrorResponse& err,
                   const HSDS3::Accessibility& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::OrganizationIdentifier& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.identifier_type().empty()) {
      err = ErrorResponse::make_bad_request("organization_identifier.identifier_type is required");
      return false;
    }

    if (value.identifier().empty()) {
      err = ErrorResponse::make_bad_request("organization_identifier.identifier is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::CostOption& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Taxonomy& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.name().empty()) {
      err = ErrorResponse::make_bad_request("taxonomy.name is required");
      return false;
    }

    if (value.description().empty()) {
      err = ErrorResponse::make_bad_request("taxonomy.description is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Organization& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.name().empty()) {
      err = ErrorResponse::make_bad_request("organization.name is required");
      return false;
    }

    if (value.description().empty()) {
      err = ErrorResponse::make_bad_request("organization.description is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Program& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.organization_id().empty()) {
      err = ErrorResponse::make_bad_request("program.organization_id is required");
      return false;
    }

    if (value.name().empty()) {
      err = ErrorResponse::make_bad_request("program.name is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Service& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.organization_id().empty()) {
      err = ErrorResponse::make_bad_request("service.organization_id is required");
      return false;
    }

    if (value.name().empty()) {
      err = ErrorResponse::make_bad_request("service.name is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Attribute& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::ServiceAtLocation& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.service_id().empty()) {
      err = ErrorResponse::make_bad_request("service_at_location.service_id is required");
      return false;
    }

    if (value.location_id().empty()) {
      err = ErrorResponse::make_bad_request("service_at_location.location_id is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Location& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Phone& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.number().empty()) {
      err = ErrorResponse::make_bad_request("phone.number is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Contact& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Address& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.address_1().empty()) {
      err = ErrorResponse::make_bad_request("address.address_1 is required");
      return false;
    }

    if (value.city().empty()) {
      err = ErrorResponse::make_bad_request("address.city is required");
      return false;
    }

    if (value.state_province().empty()) {
      err = ErrorResponse::make_bad_request("address.state_province is required");
      return false;
    }

    if (value.postal_code().empty()) {
      err = ErrorResponse::make_bad_request("address.postal_code is required");
      return false;
    }

    if (value.country().empty()) {
      err = ErrorResponse::make_bad_request("address.country is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Schedule& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Funding& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::ServiceArea& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::RequiredDocument& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Language& value) const
  {
    return check_common(err, value);
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::TaxonomyTerm& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.name().empty()) {
      err = ErrorResponse::make_bad_request("taxonomy_term.name is required");
      return false;
    }

    if (value.description().empty()) {
      err = ErrorResponse::make_bad_request("taxonomy_term.description is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::Metadata& value) const
  {
    if (!check_common(err, value)) {
      return false;
    }

    if (value.resource_id().empty()) {
      err = ErrorResponse::make_bad_request("metadata.resource_id is required");
      return false;
    }

    if (value.last_action_date().empty()) {
      err = ErrorResponse::make_bad_request("metadata.last_action_date is required");
      return false;
    }

    if (value.last_action_type().empty()) {
      err = ErrorResponse::make_bad_request("metadata.last_action_type is required");
      return false;
    }

    if (value.field_name().empty()) {
      err = ErrorResponse::make_bad_request("metadata.field_name is required");
      return false;
    }

    if (value.previous_value().empty()) {
      err = ErrorResponse::make_bad_request("metadata.previous_value is required");
      return false;
    }

    if (value.replacement_value().empty()) {
      err = ErrorResponse::make_bad_request("metadata.replacement_value is required");
      return false;
    }

    if (value.updated_by().empty()) {
      err = ErrorResponse::make_bad_request("metadata.updated_by is required");
      return false;
    }

    return true;
  }

  bool check_input(ErrorResponse& err,
                   const HSDS3::MetaTableDescription& value) const
  {
    return check_common(err, value);
  }

  std::shared_ptr<httpserver::http_response> respond(const ErrorResponse& response) const
  {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    OpenDDS::DCPS::JsonValueWriter<rapidjson::Writer<rapidjson::StringBuffer> > jvw(writer);

    size_t idx = 0;
    jvw.begin_sequence();
    jvw.begin_element(idx++);
    jvw.begin_struct();
    jvw.begin_struct_member(OpenDDS::XTypes::MemberDescriptorImpl("code", false));
    jvw.write_int32(response.status());
    jvw.end_struct_member();
    jvw.begin_struct_member(OpenDDS::XTypes::MemberDescriptorImpl("message", false));
    jvw.ValueWriter::write_string(response.message());
    jvw.end_struct_member();
    jvw.end_struct();
    jvw.end_element();
    jvw.end_sequence();
    writer.Flush();

    std::shared_ptr<httpserver::http_response> res(new string_response(buffer.GetString(),
                                                                       response.status(),
                                                                       "application/json"));
    add_transaction(res);
    add_headers(res);
    return res;
  }

  std::shared_ptr<httpserver::http_response> respond_with_json(const std::string& json) const
  {
    std::shared_ptr<httpserver::http_response> res(new string_response(json,
                                                                       httpserver::http::http_utils::http_ok,
                                                                       "application/json"));
    add_transaction(res);
    add_headers(res);
    return res;
  }

  std::shared_ptr<httpserver::http_response> respond_with_no_content() const
  {
    std::shared_ptr<httpserver::http_response> res(new string_response("",
                                                                       httpserver::http::http_utils::http_no_content));
    add_transaction(res);
    add_headers(res);
    return res;
  }

  Application& application_;

private:
  void add_transaction(std::shared_ptr<httpserver::http_response> response) const
  {
    std::stringstream ss;
    ss << application_.transaction();
    response->with_header("Transaction", ss.str());
  }

  template <typename T>
  bool check_common(ErrorResponse& err,
                    const T& value) const
  {
    if (value.dpmgid().empty()) {
      err = ErrorResponse::make_bad_request("dpmgid is required");
      return false;
    }
    if (value.id().empty()) {
      err = ErrorResponse::make_bad_request("id is required");
      return false;
    }
    if (application_.create_writers() && value.dpmgid() != application_.dpm_gid()) {
      err = ErrorResponse::make_bad_request("dpmgid is not the required value");
      return false;
    }

    return true;
  }
};

template <class T>
class UnitResourceBase : public ResourceBase {
public:
  typedef Container<T> ContainerType;
  typedef typename OpenDDS::DCPS::DDSTraits<T>::DataWriterType::_var_type WriterType;

  UnitResourceBase(Application& application)
    : ResourceBase(application)
    , unit_(application.unit<T>())
  {}

protected:
  Unit<T>& unit_;
};

#endif // HSDS_WRITER_RESOURCE_BASE_H

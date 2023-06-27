// Copyright 2023 CommunityUtils Authors

#include "HsdsResource.h"
#include "Common.h"

// template <typename T>
// class Predicate {
// public:
//   Predicate(const Unit<T>& unit,
//             DDS::MemberId member_id,
//             DDS::TypeKind kind,
//             bool ascending)
//     : unit_(unit)
//     , member_id_(member_id)
//     , kind_(kind)
//     , ascending_(ascending)
//   {}

//   template <typename I>
//   bool operator()(I x, I y) const
//   {
//     DDS::DynamicType_var type = unit_.type_support->get_type();
//     DDS::DynamicData_var xx = OpenDDS::XTypes::get_dynamic_data_adapter<T, T>(type, *x);
//     DDS::DynamicData_var yy = OpenDDS::XTypes::get_dynamic_data_adapter<T, T>(type, *y);

//     switch (kind_) {
//     case OpenDDS::XTypes::TK_BOOLEAN:
//       {
//         DDS::Boolean xxx;
//         xx->get_boolean_value(xxx, member_id_);
//         DDS::Boolean yyy;
//         yy->get_boolean_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_BYTE:
//       {
//         DDS::Byte xxx;
//         xx->get_byte_value(xxx, member_id_);
//         DDS::Byte yyy;
//         yy->get_byte_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_INT16:
//       {
//         DDS::Int16 xxx;
//         xx->get_int16_value(xxx, member_id_);
//         DDS::Int16 yyy;
//         yy->get_int16_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_INT32:
//       {
//         DDS::Int32 xxx;
//         xx->get_int32_value(xxx, member_id_);
//         DDS::Int32 yyy;
//         yy->get_int32_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_INT64:
//       {
//         DDS::Int64 xxx;
//         xx->get_int64_value(xxx, member_id_);
//         DDS::Int64 yyy;
//         yy->get_int64_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_UINT16:
//       {
//         DDS::UInt16 xxx;
//         xx->get_uint16_value(xxx, member_id_);
//         DDS::UInt16 yyy;
//         yy->get_uint16_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_UINT32:
//       {
//         DDS::UInt32 xxx;
//         xx->get_uint32_value(xxx, member_id_);
//         DDS::UInt32 yyy;
//         yy->get_uint32_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_UINT64:
//       {
//         DDS::UInt64 xxx;
//         xx->get_uint64_value(xxx, member_id_);
//         DDS::UInt64 yyy;
//         yy->get_uint64_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_FLOAT32:
//       {
//         DDS::Float32 xxx;
//         xx->get_float32_value(xxx, member_id_);
//         DDS::Float32 yyy;
//         yy->get_float32_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_FLOAT64:
//       {
//         DDS::Float64 xxx;
//         xx->get_float64_value(xxx, member_id_);
//         DDS::Float64 yyy;
//         yy->get_float64_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_FLOAT128:
//       {
//         DDS::Float128 xxx;
//         xx->get_float128_value(xxx, member_id_);
//         DDS::Float128 yyy;
//         yy->get_float128_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_INT8:
//       {
//         DDS::Int8 xxx;
//         xx->get_int8_value(xxx, member_id_);
//         DDS::Int8 yyy;
//         yy->get_int8_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_UINT8:
//       {
//         DDS::UInt8 xxx;
//         xx->get_uint8_value(xxx, member_id_);
//         DDS::UInt8 yyy;
//         yy->get_uint8_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_CHAR8:
//       {
//         DDS::Char8 xxx;
//         xx->get_char8_value(xxx, member_id_);
//         DDS::Char8 yyy;
//         yy->get_char8_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_CHAR16:
//       {
//         DDS::Char16 xxx;
//         xx->get_char16_value(xxx, member_id_);
//         DDS::Char16 yyy;
//         yy->get_char16_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_STRING8:
//       {
//         DDS::String8 xxx;
//         xx->get_string_value(xxx, member_id_);
//         DDS::String8 yyy;
//         yy->get_string_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_STRING16:
//       {
//         DDS::String16 xxx;
//         xx->get_wstring_value(xxx, member_id_);
//         DDS::String16 yyy;
//         yy->get_wstring_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     case OpenDDS::XTypes::TK_ENUM:
//       {
//         // Not sure what to do here.
//         DDS::Int64 xxx;
//         xx->get_int64_value(xxx, member_id_);
//         DDS::Int64 yyy;
//         yy->get_int64_value(yyy, member_id_);
//         return compare(xxx, yyy);
//       }
//     default:
//       return false;
//     }
//   }

// private:
//   template <typename U>
//   bool compare(const U& x, const U& y) const
//   {
//     if (ascending_) {
//       return x < y;
//     }
//     return y < x;
//   }

//   const Unit<T>& unit_;
//   const DDS::MemberId member_id_;
//   const DDS::TypeKind kind_;
//   const bool ascending_;
// };

// class CollectionRequest {
// public:
//   CollectionRequest()
//   {}

//   bool load(ErrorResponse& err,
//             const http_request& request,
//             DDS::TypeSupport_ptr type_support)
//   {
//     // TODO: Does page start at 0 or 1?
//     page_ = 0;
//     per_page_ = 100;
//     sort_by_ = request.get_arg("sort_by");
//     order_ = request.get_arg("order");
//     if (order_.empty()) {
//       order_ = "asc";
//     }

//     const std::string page_str = request.get_arg("page");
//     if (!page_str.empty()) {
//       page_ = ACE_OS::atoi(page_str.c_str());
//     }
//     const std::string per_page_str = request.get_arg("per_page");
//     if (!per_page_str.empty()) {
//       per_page_ = std::max(0, std::min(100, ACE_OS::atoi(per_page_str.c_str())));
//     }

//     if (!(order_ == "asc" || order_ == "desc")) {
//       err = ErrorResponse::make_bad_request("order must asc or desc");
//       return false;
//     }

//     if (!sort_by_.empty()) {
//       DDS::DynamicType_var type = type_support->get_type();
//       DDS::DynamicTypeMember_var member;
//       if (type->get_member_by_name(member, sort_by_.c_str()) != DDS::RETCODE_OK) {
//         err = ErrorResponse::make_bad_request("unknown sort_by field");
//         return false;
//       }

//       DDS::MemberDescriptor_var member_descriptor;
//       if (member->get_descriptor(member_descriptor) != DDS::RETCODE_OK) {
//         err = ErrorResponse::make_internal_server_error();
//         return false;
//       }

//       DDS::DynamicType_var member_type = member_descriptor->type();
//       member_type = OpenDDS::XTypes::get_base_type(member_type);

//       DDS::TypeDescriptor_var type_descriptor;
//       if (member_descriptor->type()->get_descriptor(type_descriptor) != DDS::RETCODE_OK) {
//         err = ErrorResponse::make_internal_server_error();
//         return false;
//       }

//       switch (type_descriptor->kind()) {
//       case OpenDDS::XTypes::TK_BOOLEAN:
//       case OpenDDS::XTypes::TK_BYTE:
//       case OpenDDS::XTypes::TK_INT16:
//       case OpenDDS::XTypes::TK_INT32:
//       case OpenDDS::XTypes::TK_INT64:
//       case OpenDDS::XTypes::TK_UINT16:
//       case OpenDDS::XTypes::TK_UINT32:
//       case OpenDDS::XTypes::TK_UINT64:
//       case OpenDDS::XTypes::TK_FLOAT32:
//       case OpenDDS::XTypes::TK_FLOAT64:
//       case OpenDDS::XTypes::TK_FLOAT128:
//       case OpenDDS::XTypes::TK_INT8:
//       case OpenDDS::XTypes::TK_UINT8:
//       case OpenDDS::XTypes::TK_CHAR8:
//       case OpenDDS::XTypes::TK_CHAR16:
//       case OpenDDS::XTypes::TK_STRING8:
//       case OpenDDS::XTypes::TK_STRING16:
//       case OpenDDS::XTypes::TK_ENUM:
//         break;
//       default:
//         err = ErrorResponse::make_bad_request("sort_by field is not comparable");
//         return false;
//       }

//       member_id_ = member_descriptor->id();
//       kind_ = type_descriptor->kind();
//     }

//     return true;
//   }

//   bool sort() const { return !sort_by_.empty(); }
//   DDS::MemberId member_id() const { return member_id_; }
//   DDS::TypeKind kind() const { return kind_; }
//   bool ascending() const { return order_ == "asc"; }
//   size_t offset() const { return page_ * per_page_; }
//   size_t per_page() const { return per_page_; }

// private:
//   std::string sort_by_;
//   DDS::MemberId member_id_;
//   DDS::TypeKind kind_;
//   std::string order_;
//   size_t page_;
//   size_t per_page_;
// };

// template <typename T>
// class HsdaCollectionResource : public UnitResourceBase<T> {
// public:
//   HsdaCollectionResource(Application& application,
//                          webserver& ws,
//                          const std::string& endpoint)
//     : UnitResourceBase<T>(application)
//   {
//     this->disallow_all();
//     this->set_allowing("GET", true);
//     this->set_allowing("POST", true);

//     ws.register_resource(endpoint, this);
//   }

// private:
//   typedef typename UnitResourceBase<T>::ContainerType ContainerType;

//   const std::shared_ptr<http_response> render_GET(const http_request& request) {
//     CollectionRequest cr;
//     ErrorResponse err;
//     if (!cr.load(err, request, this->unit_.type_support)) {
//       return this->respond(err);
//     }

//     typedef std::vector<typename ContainerType::const_iterator> IndexType;
//     IndexType index;
//     index.reserve(this->unit_.container.size());
//     for (typename ContainerType::const_iterator pos = this->unit_.container.begin(), limit = this->unit_.container.end(); pos != limit; ++pos) {
//       // TODO: Apply filter here.
//       index.push_back(pos);
//     }

//     // Sort.
//     if (cr.sort()) {
//       std::sort(index.begin(), index.end(), Predicate<T>(this->unit_, cr.member_id(), cr.kind(), cr.ascending()));
//     }

//     // Page.
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     OpenDDS::DCPS::JsonValueWriter<rapidjson::Writer<rapidjson::StringBuffer> > jvw(writer);

//     size_t idx = 0;
//     jvw.begin_sequence();
//     const size_t offset = cr.offset();
//     if (offset < index.size()) {
//       typename IndexType::const_iterator pos = index.begin();
//       std::advance(pos, offset);
//       for (typename IndexType::const_iterator limit = index.end();
//            pos != limit && idx != cr.per_page(); ++pos) {
//         jvw.begin_element(idx++);
//         vwrite(jvw, **pos);
//         jvw.end_element();
//       }
//     }
//     jvw.end_sequence();
//     writer.Flush();

//     return this->respond_with_json(buffer.GetString());
//   }

//   const std::shared_ptr<http_response> render_POST(const http_request& request) {

//     // Parse the input.
//     rapidjson::StringStream ss(request.get_content().c_str());
//     T element;
//     if (!OpenDDS::DCPS::from_json(element, ss)) {
//       return this->respond(ErrorResponse::make_bad_request("input is malformed"));
//     }

//     ErrorResponse err;
//     if (!this->check_input(err, element)) {
//       return this->respond(err);
//     }

//     if (this->application_.insert_and_write(element) != DDS::RETCODE_OK) {
//       return this->respond(ErrorResponse::make_internal_server_error());
//     }

//     this->application_.increment_transaction();

//     return this->respond_with_json(OpenDDS::DCPS::to_json(element));
//   }
// };

// template <typename T, typename U>
// class HsdaCompleteCollectionResource : public ResourceBase {
// public:
//   HsdaCompleteCollectionResource(Application& application,
//                                  webserver& ws,
//                                  const std::string& endpoint)
//     : ResourceBase(application)
//   {
//     this->disallow_all();
//     this->set_allowing("GET", true);
//     this->set_allowing("POST", true);

//     ws.register_resource(endpoint, this);
//   }

// private:

//   const std::shared_ptr<http_response> render_GET(const http_request& request) {
//     const Unit<U>& unit = this->application_.template unit<U>();
//     typedef typename Unit<U>::ContainerType ContainerType;

//     CollectionRequest cr;
//     ErrorResponse err;
//     if (!cr.load(err, request, unit.type_support)) {
//       return this->respond(err);
//     }

//     typedef std::vector<typename ContainerType::const_iterator> IndexType;
//     IndexType index;
//     index.reserve(unit.container.size());
//     for (typename ContainerType::const_iterator pos = unit.container.begin(), limit = unit.container.end(); pos != limit; ++pos) {
//       // TODO: Apply filter here.
//       index.push_back(pos);
//     }

//     // Sort.
//     if (cr.sort()) {
//       std::sort(index.begin(), index.end(), Predicate<U>(unit, cr.member_id(), cr.kind(), cr.ascending()));
//     }

//     // Page.
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     OpenDDS::DCPS::JsonValueWriter<rapidjson::Writer<rapidjson::StringBuffer> > jvw(writer);

//     T complete;
//     size_t idx = 0;
//     jvw.begin_sequence();
//     const size_t offset = cr.offset();
//     if (offset < index.size()) {
//       typename IndexType::const_iterator pos = index.begin();
//       std::advance(pos, offset);
//       for (typename IndexType::const_iterator limit = index.end();
//            pos != limit && idx != cr.per_page(); ++pos) {
//         jvw.begin_element(idx++);
//         this->application_.complete(complete, **pos);
//         vwrite(jvw, complete);
//         jvw.end_element();
//       }
//     }
//     jvw.end_sequence();
//     writer.Flush();

//     return this->respond_with_json(buffer.GetString());
//   }

//   const std::shared_ptr<http_response> render_POST(const http_request& request) {

//     // Parse the input.
//     rapidjson::StringStream ss(request.get_content().c_str());
//     T element;
//     if (!OpenDDS::DCPS::from_json(element, ss)) {
//       return respond(ErrorResponse::make_bad_request("input is malformed"));
//     }

//     ErrorResponse err;
//     if (!check_input(err, element)) {
//       return respond(err);
//     }

//     if (application_.insert_and_write(element) != DDS::RETCODE_OK) {
//       return respond(ErrorResponse::make_internal_server_error());
//     }

//     this->application_.increment_transaction();

//     return respond_with_json(OpenDDS::DCPS::to_json(element));
//   }
// };

void log_access_fn(const std::string& url)
{
  ACE_DEBUG((LM_INFO, "ACCESSING: %C\n", url.c_str()));
}

int
ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  Application application;
  application.load_environment_variables();
  application.dump_configuration();

  if (application.create_writers()) {
    if (application.dpm_gid().empty()) {
      ACE_ERROR((LM_ERROR, "DPM_GID cannot be empty\n"));
      return EXIT_FAILURE;
    }

    if (application.download_relay_config() != DDS::RETCODE_OK) {
      return EXIT_FAILURE;
    }
    if (application.download_security_documents() != DDS::RETCODE_OK) {
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
    if (application.enable_observer()) {
      application.install_observer();
    }
    if (application.create_topics() != DDS::RETCODE_OK) {
      return EXIT_FAILURE;
    }
    if (application.create_datawriters() != DDS::RETCODE_OK) {
      return EXIT_FAILURE;
    }
  }

  httpserver::create_webserver create_ws(application.http_port());
  if (application.enable_http_log_access()) {
    create_ws.log_access(log_access_fn);
  }
  httpserver::webserver webserver = create_ws;

  // Create /hsds endpoints.
  HsdsResource<HSDS3::Service> service_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Phone> phone_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Schedule> schedule_hsds_resource(application, webserver);
  HsdsResource<HSDS3::ServiceArea> service_area_hsds_resource(application, webserver);
  HsdsResource<HSDS3::ServiceAtLocation> service_at_location_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Location> location_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Language> language_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Organization> organization_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Funding> funding_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Accessibility> accessibility_hsds_resource(application, webserver);
  HsdsResource<HSDS3::CostOption> cost_option_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Program> program_hsds_resource(application, webserver);
  HsdsResource<HSDS3::RequiredDocument> required_document_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Contact> contact_hsds_resource(application, webserver);
  HsdsResource<HSDS3::OrganizationIdentifier> organization_identifier_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Attribute> attribute_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Metadata> metadata_hsds_resource(application, webserver);
  HsdsResource<HSDS3::MetaTableDescription> meta_table_description_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Taxonomy> taxonomy_hsds_resource(application, webserver);
  HsdsResource<HSDS3::TaxonomyTerm> taxonomy_term_hsds_resource(application, webserver);
  HsdsResource<HSDS3::Address> address_hsds_resource(application, webserver);

  webserver.start(false);

  GuardWrapper wrapper;
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;

  bool keep_going = true;
  const DDS::Duration_t infinite = { DDS::DURATION_INFINITE_SEC, DDS::DURATION_INFINITE_NSEC };
  while (keep_going) {
    const DDS::ReturnCode_t error = ws->wait(active, infinite);
    if (error != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: wait failed %C\n", OpenDDS::DCPS::retcode_to_string(error)));
    }

    for (unsigned int i = 0; keep_going && i != active.length(); ++i) {
      if (active[i] == wrapper.guard()) {
        keep_going = false;
      }
    }
  }

  ws->detach_condition(wrapper.guard());

  webserver.stop();
  application.shutdown();

  return EXIT_SUCCESS;
}

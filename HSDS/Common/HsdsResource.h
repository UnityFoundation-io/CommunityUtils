#ifndef HSDS_WRITER_HSDS_RESOURCE_H
#define HSDS_WRITER_HSDS_RESOURCE_H

// Copyright 2023 CommunityUtils Authors

#include "ResourceBase.h"

#include <dds/DCPS/JsonValueReader.h>
#include <dds/DCPS/JsonValueWriter.h>

template <class T>
class HsdsElementResource : public UnitResourceBase<T> {
public:
  HsdsElementResource(Application& application,
                      httpserver::webserver& ws,
                      bool allow_modifying)
    : UnitResourceBase<T>(application)
  {
    this->disallow_all();
    this->set_allowing("PUT", allow_modifying);
    this->set_allowing("GET", true);
    this->set_allowing("DELETE", allow_modifying);

    ws.register_resource(application.unit<T>().endpoint + "/{dpmgid}/{id}", this);
  }

  const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& request) {
    // Extract the dpmgid and id from the URL.
    const std::string dpmgid = request.get_arg("dpmgid");
    const std::string id = request.get_arg("id");

    if (dpmgid.empty()) {
      return this->respond(ErrorResponse::make_bad_request("dpmgid is required"));
    }

    if (id.empty()) {
      return this->respond(ErrorResponse::make_bad_request("id is required"));
    }

    // Parse the input.
    rapidjson::StringStream ss(request.get_content().c_str());
    T element;
    if (!OpenDDS::DCPS::from_json(element, ss)) {
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }

    ErrorResponse err;
    if (!this->check_input(err, element)) {
      return this->respond(err);
    }

    // Compare ids.
    if (dpmgid != element.dpmgid()) {
      return this->respond(ErrorResponse::make_bad_request("dpmgid from URL and body disagree"));
    }
    if (id != element.id()) {
      return this->respond(ErrorResponse::make_bad_request("id from URL and body disagree"));
    }

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    if (this->application_.insert_and_write(element) != DDS::RETCODE_OK) {
      return this->respond(ErrorResponse::make_internal_server_error());
    }
    this->application_.increment_transaction();

    return this->respond_with_json(OpenDDS::DCPS::to_json(element));
  }

  const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& request) {
    // Extract the dpmgid and id from the URL.
    const std::string dpmgid = request.get_arg("dpmgid");
    const std::string id = request.get_arg("id");

    if (dpmgid.empty()) {
      return this->respond(ErrorResponse::make_bad_request("dpmgid is required"));
    }

    if (id.empty()) {
      return this->respond(ErrorResponse::make_bad_request("id is required"));
    }

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    typename UnitResourceBase<T>::ContainerType::const_iterator pos = this->unit_.container.find(std::make_pair(dpmgid, id));
    if (pos == this->unit_.container.end()) {
      return this->respond(ErrorResponse::make_not_found());
    }

    return this->respond_with_json(OpenDDS::DCPS::to_json(*pos));
  }

  const std::shared_ptr<httpserver::http_response> render_DELETE(const httpserver::http_request& request) {
    // Extract the dpmgid and id from the URL.
    const std::string dpmgid = request.get_arg("dpmgid");
    const std::string id = request.get_arg("id");

    if (dpmgid.empty()) {
      return this->respond(ErrorResponse::make_bad_request("dpmgid is required"));
    }

    if (id.empty()) {
      return this->respond(ErrorResponse::make_bad_request("id is required"));
    }

    const typename UnitResourceBase<T>::ContainerType::KeyType key = std::make_pair(dpmgid, id);

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    typename UnitResourceBase<T>::ContainerType::const_iterator pos = this->unit_.container.find(key);
    if (pos == this->unit_.container.end()) {
      return this->respond(ErrorResponse::make_not_found());
    }

    if (this->application_.unregister_and_erase(*pos) != DDS::RETCODE_OK) {
      return this->respond(ErrorResponse::make_internal_server_error());
    }
    this->application_.increment_transaction();

    return this->respond_with_no_content();
  }
};

template <class T>
class HsdsCollectionResource : public UnitResourceBase<T> {
public:
  HsdsCollectionResource(Application& application, httpserver::webserver& ws, bool allow_modifying)
    : UnitResourceBase<T>(application)
  {
    this->disallow_all();
    this->set_allowing("PUT", allow_modifying);
    this->set_allowing("POST", allow_modifying);
    this->set_allowing("GET", true);
    this->set_allowing("DELETE", allow_modifying);

    ws.register_resource(application.unit<T>().endpoint, this);
  }

  const std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& request) {
    typename UnitResourceBase<T>::ContainerType new_map;

    // Parse the input.
    rapidjson::StringStream ss(request.get_content().c_str());
    OpenDDS::DCPS::JsonValueReader<rapidjson::StringStream> jvr(ss);
    if (!jvr.begin_sequence()) {
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }
    while (jvr.elements_remaining()) {
      if (!jvr.begin_element()) {
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      T element;
      OpenDDS::DCPS::set_default(element);
      if (!vread(jvr, element)) {
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      ErrorResponse err;
      if (!this->check_input(err, element)) {
        return this->respond(err);
      }
      // Save.
      new_map.insert(element);
      if (!jvr.end_element()) {
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
    }
    if (!jvr.end_sequence()) {
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    // Unregister.
    if (this->application_.create_writers()) {
      for (const auto& e : this->unit_.container) {
        const DDS::ReturnCode_t retcode = this->unit_.writer->unregister_instance(e, DDS::HANDLE_NIL);
        if (retcode != DDS::RETCODE_OK) {
          ACE_ERROR((LM_ERROR, "ERROR: unregister_instance failed %C\n", OpenDDS::DCPS::retcode_to_string(retcode)));
          return this->respond(ErrorResponse::make_internal_server_error());
        }
      }
    }

    // Persist.
    this->unit_.container.swap(new_map);

    // Write.
    if (this->application_.create_writers()) {
      for (const auto& e : this->unit_.container) {
        const DDS::ReturnCode_t retcode = this->unit_.writer->write(e, DDS::HANDLE_NIL);
        if (retcode != DDS::RETCODE_OK) {
          ACE_ERROR((LM_ERROR, "ERROR: write failed %C\n", OpenDDS::DCPS::retcode_to_string(retcode)));
          return this->respond(ErrorResponse::make_internal_server_error());
        }
      }
    }

    this->application_.increment_transaction();

    return this->respond_with_no_content();
  }

  const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& request) {
    std::vector<T> list;

    // Parse the input.
    rapidjson::StringStream ss(request.get_content().c_str());
    OpenDDS::DCPS::JsonValueReader<rapidjson::StringStream> jvr(ss);
    if (!jvr.begin_sequence()) {
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }
    while (jvr.elements_remaining()) {
      if (!jvr.begin_element()) {
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      T element;
      OpenDDS::DCPS::set_default(element);
      if (!vread(jvr, element)) {
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      ErrorResponse err;
      if (!this->check_input(err, element)) {
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      list.push_back(element);
      if (!jvr.end_element()) {
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
    }
    if (!jvr.end_sequence()) {
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    for (const auto& e : list) {
      if (this->application_.insert_and_write(e) != DDS::RETCODE_OK) {
        return this->respond(ErrorResponse::make_internal_server_error());
      }
    }

    this->application_.increment_transaction();

    return this->respond_with_no_content();
  }

  const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& request) {
    size_t offset = 0;
    size_t count = this->unit_.container.size();

    const std::string offset_str = request.get_arg("offset");
    if (!offset_str.empty()) {
      offset = ACE_OS::atoi(offset_str.c_str());
    }
    const std::string count_str = request.get_arg("count");
    if (!count_str.empty()) {
      count = ACE_OS::atoi(count_str.c_str());
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    OpenDDS::DCPS::JsonValueWriter<rapidjson::Writer<rapidjson::StringBuffer> > jvw(writer);

    size_t idx = 0;
    jvw.begin_sequence();

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    if (offset < this->unit_.container.size()) {
      typename UnitResourceBase<T>::ContainerType::const_iterator pos = this->unit_.container.begin();
      std::advance(pos, offset);
      for (typename UnitResourceBase<T>::ContainerType::const_iterator limit = this->unit_.container.end();
           pos != limit && idx != count; ++pos) {
        jvw.begin_element(idx++);
        vwrite(jvw, *pos);
        jvw.end_element();
      }
    }
    jvw.end_sequence();
    writer.Flush();

    std::shared_ptr<httpserver::http_response> response = this->respond_with_json(buffer.GetString());
    response->with_header("Offset", OpenDDS::DCPS::to_dds_string(offset));
    response->with_header("Count", OpenDDS::DCPS::to_dds_string(idx));
    return response;
  }

  const std::shared_ptr<httpserver::http_response> render_DELETE(const httpserver::http_request&) {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    // Unregister.
    if (this->application_.create_writers()) {
      for (const auto& e : this->unit_.container) {
        const DDS::ReturnCode_t retcode = this->unit_.writer->unregister_instance(e, DDS::HANDLE_NIL);
        if (retcode != DDS::RETCODE_OK) {
          ACE_ERROR((LM_ERROR, "ERROR: unregister_instance failed %C\n", OpenDDS::DCPS::retcode_to_string(retcode)));
          return this->respond(ErrorResponse::make_internal_server_error());
        }
      }
    }

    // Delete.
    this->unit_.container.clear();

    this->application_.increment_transaction();

    return this->respond_with_no_content();
  }

private:
  void add_headers(std::shared_ptr<httpserver::http_response> response) const
  {
    response->with_header("Total", OpenDDS::DCPS::to_dds_string(this->unit_.container.size()));
    response->with_header("Access-Control-Expose-Headers", "Offset, Count, Total");
  }
};

template <typename T>
class HsdsResource {
public:
  HsdsResource(Application& application,
               httpserver::webserver& ws,
               bool allow_modifying = true)
    : element_resource_(application, ws, allow_modifying)
    , collection_resource_(application, ws, allow_modifying)
  {}

private:
  HsdsElementResource<T> element_resource_;
  HsdsCollectionResource<T> collection_resource_;
};

#endif // HSDS_WRITER_HSDS_RESOURCE_H

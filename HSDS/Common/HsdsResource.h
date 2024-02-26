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

  std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& request) {
    if (!ResourceBase::is_authorized(request)) {
      return this->respond(ErrorResponse::make_unauthorized());
    }

    // Extract the dpmgid and id from the URL.
    const std::string dpmgid = request.get_arg("dpmgid");
    const std::string id = request.get_arg("id");

    if (dpmgid.empty()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_PUT: dpmgid is missing from URL!\n"));
      return this->respond(ErrorResponse::make_bad_request("dpmgid is required"));
    }

    if (id.empty()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_PUT: id is missing from URL!\n"));
      return this->respond(ErrorResponse::make_bad_request("id is required"));
    }

    // Parse the input.
    const std::string s(request.get_content());
    rapidjson::StringStream ss(s.c_str());
    T element;
    if (!OpenDDS::DCPS::from_json(element, ss)) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_PUT: failed to parse input json!!\n"));
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }

    ErrorResponse err;
    if (!this->check_input(err, element)) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_PUT: check_input failed!\n"));
      return this->respond(err);
    }

    // Compare ids.
    if (dpmgid != element.dpmgid()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_PUT: dpmgid from URL and body disagree!\n"));
      return this->respond(ErrorResponse::make_bad_request("dpmgid from URL and body disagree"));
    }
    if (id != element.id()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_PUT: id from URL and body disagree!\n"));
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

  std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& request) {
    if (!ResourceBase::is_authorized(request)) {
      return this->respond(ErrorResponse::make_unauthorized());
    }

    // Extract the dpmgid and id from the URL.
    const std::string dpmgid = request.get_arg("dpmgid");
    const std::string id = request.get_arg("id");

    if (dpmgid.empty()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_GET: dpmgid is missing from URL!\n"));
      return this->respond(ErrorResponse::make_bad_request("dpmgid is required"));
    }

    if (id.empty()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_GET: id is missing from URL!\n"));
      return this->respond(ErrorResponse::make_bad_request("id is required"));
    }

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    typename UnitResourceBase<T>::ContainerType::const_iterator pos = this->unit_.container.find(std::make_pair(dpmgid, id));
    if (pos == this->unit_.container.end()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_GET: Resource for (dpmgid:%C, id:%C) not found!\n",
                 dpmgid.c_str(), id.c_str()));
      return this->respond(ErrorResponse::make_not_found());
    }

    return this->respond_with_json(OpenDDS::DCPS::to_json(*pos));
  }

  std::shared_ptr<httpserver::http_response> render_DELETE(const httpserver::http_request& request) {
    if (!ResourceBase::is_authorized(request)) {
      return this->respond(ErrorResponse::make_unauthorized());
    }

    // Extract the dpmgid and id from the URL.
    const std::string dpmgid = request.get_arg("dpmgid");
    const std::string id = request.get_arg("id");

    if (dpmgid.empty()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_DELETE: dpmgid is missing from URL!\n"));
      return this->respond(ErrorResponse::make_bad_request("dpmgid is required"));
    }

    if (id.empty()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_DELETE: id is missing from URL!\n"));
      return this->respond(ErrorResponse::make_bad_request("id is required"));
    }

    const typename UnitResourceBase<T>::ContainerType::KeyType key = std::make_pair(dpmgid, id);

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    typename UnitResourceBase<T>::ContainerType::const_iterator pos = this->unit_.container.find(key);
    if (pos == this->unit_.container.end()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsElementResource::render_DELETE: Resource for (dpmgid:%C, id:%C) not found!\n",
                 dpmgid.c_str(), id.c_str()));
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

  std::shared_ptr<httpserver::http_response> render_PUT(const httpserver::http_request& request) {
    if (!ResourceBase::is_authorized(request)) {
      return this->respond(ErrorResponse::make_unauthorized());
    }

    typedef typename UnitResourceBase<T>::ContainerType ContainerType;
    ContainerType new_container;

    // Parse the input.
    const std::string s(request.get_content());
    rapidjson::StringStream ss(s.c_str());
    OpenDDS::DCPS::JsonValueReader<rapidjson::StringStream> jvr(ss);
    if (!jvr.begin_sequence()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_PUT: begin_sequence failed!\n"));
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }
    while (jvr.elements_remaining()) {
      if (!jvr.begin_element()) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_PUT: begin_element failed!\n"));
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      T element;
      OpenDDS::DCPS::set_default(element);
      if (!vread(jvr, element)) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_PUT: vread failed!\n"));
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      ErrorResponse err;
      if (!this->check_input(err, element)) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_PUT: check_input failed!\n"));
        return this->respond(err);
      }
      // Save.
      new_container.insert(element);
      if (!jvr.end_element()) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_PUT: end_element failed!\n"));
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
    }
    if (!jvr.end_sequence()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_PUT: end_sequence failed!\n"));
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }

    if (this->application_.create_writers()) {
      ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                       this->respond(ErrorResponse::make_internal_server_error()));

      typename ContainerType::const_ordered_iterator pos_new = new_container.ordered_begin();
      const typename ContainerType::const_ordered_iterator limit_new = new_container.ordered_end();
      typename ContainerType::const_ordered_iterator pos_old = this->unit_.container.ordered_begin();
      const typename ContainerType::const_ordered_iterator limit_old= this->unit_.container.ordered_end();

      while (pos_new != limit_new || pos_old != limit_old) {
        if (pos_new == limit_new) {
          // Deleted item.
          const DDS::ReturnCode_t retcode = this->unit_.writer->unregister_instance(this->unit_.container.at(pos_old->second), DDS::HANDLE_NIL);
          if (retcode != DDS::RETCODE_OK) {
            ACE_ERROR((LM_ERROR, "ERROR: HsdsCollectionResource::render_PUT: unregister_instance failed %C\n",
                       OpenDDS::DCPS::retcode_to_string(retcode)));
            return this->respond(ErrorResponse::make_internal_server_error());
          }
          ++pos_old;
          continue;
        }
        if (pos_old == limit_old) {
          // New item.
          const DDS::ReturnCode_t retcode = this->unit_.writer->write(new_container.at(pos_new->second), DDS::HANDLE_NIL);
          if (retcode != DDS::RETCODE_OK) {
            ACE_ERROR((LM_ERROR, "ERROR: HsdsCollectionResource::render_PUT: write failed %C\n",
                       OpenDDS::DCPS::retcode_to_string(retcode)));
            return this->respond(ErrorResponse::make_internal_server_error());
          }
          ++pos_new;
          continue;
        }

        if (pos_new->first == pos_old->first) {
          const T& n = new_container.at(pos_new->second);
          const T& o = this->unit_.container.at(pos_old->second);

          if (n != o) {
            // Changed item.
            const DDS::ReturnCode_t retcode = this->unit_.writer->write(n, DDS::HANDLE_NIL);
            if (retcode != DDS::RETCODE_OK) {
              ACE_ERROR((LM_ERROR, "ERROR: HsdsCollectionResource::render_PUT: write failed %C\n",
                         OpenDDS::DCPS::retcode_to_string(retcode)));
              return this->respond(ErrorResponse::make_internal_server_error());
            }
          }
          ++pos_new;
          ++pos_old;
          continue;
        }

        if (pos_new->first < pos_old->first) {
          // New item.
          const DDS::ReturnCode_t retcode = this->unit_.writer->write(new_container.at(pos_new->second), DDS::HANDLE_NIL);
          if (retcode != DDS::RETCODE_OK) {
            ACE_ERROR((LM_ERROR, "ERROR: HsdsCollectionResource::render_PUT: write failed %C\n",
                       OpenDDS::DCPS::retcode_to_string(retcode)));
            return this->respond(ErrorResponse::make_internal_server_error());
          }
          ++pos_new;
          continue;
        }

        // Deleted item.
        const DDS::ReturnCode_t retcode = this->unit_.writer->unregister_instance(this->unit_.container.at(pos_old->second), DDS::HANDLE_NIL);
        if (retcode != DDS::RETCODE_OK) {
          ACE_ERROR((LM_ERROR, "ERROR: HsdsCollectionResource::render_PUT: unregister_instance failed %C\n",
                     OpenDDS::DCPS::retcode_to_string(retcode)));
          return this->respond(ErrorResponse::make_internal_server_error());
        }
        ++pos_old;
      }
    }

    // Persist.
    this->unit_.container.swap(new_container);

    this->application_.increment_transaction();

    return this->respond_with_no_content();
  }

  std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& request) {
    if (!ResourceBase::is_authorized(request)) {
      return this->respond(ErrorResponse::make_unauthorized());
    }

    std::vector<T> list;

    // Parse the input.
    const std::string s(request.get_content());
    rapidjson::StringStream ss(s.c_str());
    OpenDDS::DCPS::JsonValueReader<rapidjson::StringStream> jvr(ss);
    if (!jvr.begin_sequence()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_POST: begin_sequence failed!\n"));
      return this->respond(ErrorResponse::make_bad_request("input is malformed"));
    }
    while (jvr.elements_remaining()) {
      if (!jvr.begin_element()) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_POST: begin_element failed!\n"));
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      T element;
      OpenDDS::DCPS::set_default(element);
      if (!vread(jvr, element)) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_POST: vread failed!\n"));
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      ErrorResponse err;
      if (!this->check_input(err, element)) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_POST: check_input failed!\n"));
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
      list.push_back(element);
      if (!jvr.end_element()) {
        ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_POST: end_element failed!\n"));
        return this->respond(ErrorResponse::make_bad_request("input is malformed"));
      }
    }
    if (!jvr.end_sequence()) {
      ACE_ERROR((LM_NOTICE, "NOTICE: HsdsCollectionResource::render_POST: end_sequence failed!\n"));
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

  std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& request) {
    if (!ResourceBase::is_authorized(request)) {
      return this->respond(ErrorResponse::make_unauthorized());
    }

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

  std::shared_ptr<httpserver::http_response> render_DELETE(const httpserver::http_request& request) {
    if (!ResourceBase::is_authorized(request)) {
      return this->respond(ErrorResponse::make_unauthorized());
    }

    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, this->application_.get_mutex(),
                     this->respond(ErrorResponse::make_internal_server_error()));
    // Unregister.
    if (this->application_.create_writers()) {
      for (const auto& e : this->unit_.container) {
        const DDS::ReturnCode_t retcode = this->unit_.writer->unregister_instance(e, DDS::HANDLE_NIL);
        if (retcode != DDS::RETCODE_OK) {
          ACE_ERROR((LM_ERROR, "ERROR: HsdsCollectionResource::render_DELETE: unregister_instance failed %C\n",
                     OpenDDS::DCPS::retcode_to_string(retcode)));
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

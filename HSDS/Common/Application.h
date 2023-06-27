#ifndef HSDS_COMMON_APPLICATION_H
#define HSDS_COMMON_APPLICATION_H

// Copyright 2023 CommunityUtils Authors

#include "Container.h"

#include "HSDS_Common_export.h"
#include "HSDS3TypeSupportImpl.h"

#include <dds/DCPS/DCPS_Utils.h>
#include <dds/DCPS/transport/framework/TransportConfig_rch.h>

template <typename T>
struct Unit {
  Unit(const std::string& key,
       const std::string& end,
       const std::string& j_f)
    : topic_name_key(key)
    , endpoint(end)
    , json_file(j_f)
  {}

  typedef Container<T> ContainerType;
  ContainerType container;

  const std::string topic_name_key;
  std::string topic_name;
  DDS::Topic_var topic;
  const std::string endpoint;
  const std::string json_file;

  typename OpenDDS::DCPS::DDSTraits<T>::TypeSupportType::_var_type type_support;
  typename OpenDDS::DCPS::DDSTraits<T>::DataWriterType::_var_type writer;
  typename OpenDDS::DCPS::DDSTraits<T>::DataReaderType::_var_type reader;
};

class HSDS_Common_Export Application {
public:
  Application()
    : domain_id_(1)
    , http_port_(8080)
    , create_writers_(true)
    , rtps_relay_only_(true)
    , enable_http_log_access_(true)
    , enable_observer_(true)
    , transaction_(0)
    , accessibility_("ACCESSIBILITY_TOPIC", HSDS3::ACCESSIBILITY_ENDPOINT, HSDS3::ACCESSIBILITY_JSON_FILE)
    , address_("ADDRESS_TOPIC", HSDS3::ADDRESS_ENDPOINT, HSDS3::ADDRESS_JSON_FILE)
    , attribute_("ATTRIBUTE_TOPIC", HSDS3::ATTRIBUTE_ENDPOINT, HSDS3::ATTRIBUTE_JSON_FILE)
    , contact_("CONTACT_TOPIC", HSDS3::CONTACT_ENDPOINT, HSDS3::CONTACT_JSON_FILE)
    , cost_option_("COST_OPTION_TOPIC", HSDS3::COST_OPTION_ENDPOINT, HSDS3::COST_OPTION_JSON_FILE)
    , funding_("FUNDING_TOPIC", HSDS3::FUNDING_ENDPOINT, HSDS3::FUNDING_JSON_FILE)
    , language_("LANGUAGE_TOPIC", HSDS3::LANGUAGE_ENDPOINT, HSDS3::LANGUAGE_JSON_FILE)
    , location_("LOCATION_TOPIC", HSDS3::LOCATION_ENDPOINT, HSDS3::LOCATION_JSON_FILE)
    , meta_table_description_("META_TABLE_DESCRIPTION_TOPIC", HSDS3::META_TABLE_DESCRIPTION_ENDPOINT, HSDS3::META_TABLE_DESCRIPTION_JSON_FILE)
    , metadata_("METADATA_TOPIC", HSDS3::METADATA_ENDPOINT, HSDS3::METADATA_JSON_FILE)
    , organization_("ORGANIZATION_TOPIC", HSDS3::ORGANIZATION_ENDPOINT, HSDS3::ORGANIZATION_JSON_FILE)
    , organization_identifier_("ORGANIZATION_IDENTIFIER_TOPIC", HSDS3::ORGANIZATION_IDENTIFIER_ENDPOINT, HSDS3::ORGANIZATION_IDENTIFIER_JSON_FILE)
    , phone_("PHONE_TOPIC", HSDS3::PHONE_ENDPOINT, HSDS3::PHONE_JSON_FILE)
    , program_("PROGRAM_TOPIC", HSDS3::PROGRAM_ENDPOINT, HSDS3::PROGRAM_JSON_FILE)
    , required_document_("REQUIRED_DOCUMENT_TOPIC", HSDS3::REQUIRED_DOCUMENT_ENDPOINT, HSDS3::REQUIRED_DOCUMENT_JSON_FILE)
    , schedule_("SCHEDULE_TOPIC", HSDS3::SCHEDULE_ENDPOINT, HSDS3::SCHEDULE_JSON_FILE)
    , service_("SERVICE_TOPIC", HSDS3::SERVICE_ENDPOINT, HSDS3::SERVICE_JSON_FILE)
    , service_area_("SERVICE_AREA_TOPIC", HSDS3::SERVICE_AREA_ENDPOINT, HSDS3::SERVICE_AREA_JSON_FILE)
    , service_at_location_("SERVICE_AT_LOCATION_TOPIC", HSDS3::SERVICE_AT_LOCATION_ENDPOINT, HSDS3::SERVICE_AT_LOCATION_JSON_FILE)
    , taxonomy_("TAXONOMY_TOPIC", HSDS3::TAXONOMY_ENDPOINT, HSDS3::TAXONOMY_JSON_FILE)
    , taxonomy_term_("TAXONOMY_TERM_TOPIC", HSDS3::TAXONOMY_TERM_ENDPOINT, HSDS3::TAXONOMY_TERM_JSON_FILE)
  {
    server_poll_period_.sec = 300;
    server_poll_period_.nanosec = 0;
  }

  void load_environment_variables();
  void dump_configuration() const;
  DDS::ReturnCode_t download_security_documents();
  DDS::ReturnCode_t download_relay_config();
  DDS::ReturnCode_t setup_discovery();
  DDS::ReturnCode_t setup_transport();
  void setup_security_plugin();
  DDS::ReturnCode_t create_participant(int argc, ACE_TCHAR* argv[]);
  void install_observer();
  DDS::ReturnCode_t create_topics();
  DDS::ReturnCode_t create_datawriters();
  DDS::ReturnCode_t create_datareaders();
  void shutdown();

  const std::string& dpm_gid() const { return dpm_gid_; }

  uint64_t transaction() const { return transaction_; }
  void transaction(uint64_t t) { transaction_ = t; }
  void increment_transaction() { ++transaction_; }

  const std::string& access_control_allow_origin() const { return access_control_allow_origin_; }

  unsigned short http_port() const { return http_port_; }
  bool create_writers() const { return create_writers_; }
  bool enable_http_log_access() const { return enable_http_log_access_; }
  bool enable_observer() const { return enable_observer_; }
  const std::string& server_url() const { return server_url_; }
  DDS::Duration_t server_poll_period() const
  {
    if (server_url_.empty()) {
      return { DDS::DURATION_INFINITE_SEC, DDS::DURATION_INFINITE_NSEC };
    } else {
      return server_poll_period_;
    }
  }

  template <typename T>
  Unit<T>& unit();

  template <typename T>
  DDS::ReturnCode_t for_each(T func)
  {
    DDS::ReturnCode_t retcode;
    if ((retcode = func(accessibility_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(address_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(attribute_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(contact_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(cost_option_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(funding_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(language_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(location_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(meta_table_description_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(metadata_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(organization_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(organization_identifier_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(phone_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(program_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(required_document_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(schedule_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(service_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(service_area_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(service_at_location_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(taxonomy_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(taxonomy_term_)) != DDS::RETCODE_OK) { return retcode; }
    return DDS::RETCODE_OK;
  }

  template <typename T>
  DDS::ReturnCode_t for_each(T func) const
  {
    DDS::ReturnCode_t retcode;
    if ((retcode = func(accessibility_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(address_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(attribute_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(contact_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(cost_option_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(funding_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(language_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(location_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(meta_table_description_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(metadata_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(organization_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(organization_identifier_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(phone_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(program_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(required_document_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(schedule_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(service_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(service_area_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(service_at_location_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(taxonomy_)) != DDS::RETCODE_OK) { return retcode; }
    if ((retcode = func(taxonomy_term_)) != DDS::RETCODE_OK) { return retcode; }
    return DDS::RETCODE_OK;
  }

  template <typename T>
  DDS::ReturnCode_t insert_and_write(const T& element)
  {
    DDS::ReturnCode_t retcode = DDS::RETCODE_OK;
    Unit<T>& unit = this->unit<T>();

    const auto pos = unit.container.find(element);
    if (pos != unit.container.end() && *pos == element) {
      // No change.
      return retcode;
    }

    // Save.
    unit.container.insert(element);

    // Write.
    if (create_writers()) {
      retcode = unit.writer->write(element, DDS::HANDLE_NIL);
      if (retcode != DDS::RETCODE_OK) {
        ACE_ERROR((LM_ERROR, "ERROR: Application::insert_and_write: write returned %C\n",
                   OpenDDS::DCPS::retcode_to_string(retcode)));
      }
    }

    return retcode;
  }

  template <typename T>
  DDS::ReturnCode_t unregister_and_erase(const T& element)
  {
    DDS::ReturnCode_t retcode = DDS::RETCODE_OK;
    Unit<T>& unit = this->unit<T>();

    // Unregister.
    if (create_writers()) {
      retcode = unit.writer->unregister_instance(element, DDS::HANDLE_NIL);
      if (retcode != DDS::RETCODE_OK) {
        ACE_ERROR((LM_ERROR, "ERROR: Application::unregister_and_erase: unregister_instance returned %C\n",
                   OpenDDS::DCPS::retcode_to_string(retcode)));
      }
    }

    // Delete.
    unit.container.erase(element);

    return retcode;
  }

  // void complete(HSDS2::CompleteService& complete_service,
  //               const HSDS2::Service& service) const
  // {
  //   // This is going to be horribly inefficient.

  //   complete_service.dpmgid(service.dpmgid());
  //   complete_service.id(service.id());
  //   complete_service.organization_id(service.organization_id());
  //   complete_service.program_id(service.program_id());
  //   complete_service.name(service.name());
  //   complete_service.alternate_name(service.alternate_name());
  //   complete_service.description(service.description());
  //   complete_service.url(service.url());
  //   complete_service.email(service.email());
  //   complete_service.status(service.status());
  //   complete_service.interpretation_services(service.interpretation_services());
  //   complete_service.application_process(service.application_process());
  //   complete_service.wait_time(service.wait_time());
  //   complete_service.fees(service.fees());
  //   complete_service.accreditations(service.accreditations());
  //   complete_service.licenses(service.licenses());

  //   for (const auto& e : contact_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.contacts().push_back(e);
  //     }
  //   }

  //   for (const auto& e : eligibility_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.eligibility().push_back(e);
  //     }
  //   }

  //   for (const auto& e : funding_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.funding().push_back(e);
  //     }
  //   }

  //   // TODO: Don't know how to distinguish between regular and holiday schedules.
  //   for (const auto& e : schedule_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.regular_schedule().push_back(e);
  //       //complete_service.holiday_schedule().push_back(e);
  //     }
  //   }

  //   for (const auto& e : language_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.language().push_back(e);
  //     }
  //   }

  //   for (const auto& e : payment_accepted_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.payment_accepted().push_back(e);
  //     }
  //   }

  //   for (const auto& e : phone_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.phones().push_back(e);
  //     }
  //   }

  //   for (const auto& e : required_document_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.required_documents().push_back(e);
  //     }
  //   }

  //   for (const auto& e : service_area_.container) {
  //     if (e.dpmgid() == complete_service.dpmgid() &&
  //         e.service_id() == complete_service.id()) {
  //       complete_service.service_area().push_back(e);
  //     }
  //   }
  // }

  // void incomplete(HSDS2::Service& complete_service,
  //                 const HSDS2::CompleteService& service) const
  // {
  //   complete_service.dpmgid(service.dpmgid());
  //   complete_service.id(service.id());
  //   complete_service.organization_id(service.organization_id());
  //   complete_service.program_id(service.program_id());
  //   complete_service.name(service.name());
  //   complete_service.alternate_name(service.alternate_name());
  //   complete_service.description(service.description());
  //   complete_service.url(service.url());
  //   complete_service.email(service.email());
  //   complete_service.status(service.status());
  //   complete_service.interpretation_services(service.interpretation_services());
  //   complete_service.application_process(service.application_process());
  //   complete_service.wait_time(service.wait_time());
  //   complete_service.fees(service.fees());
  //   complete_service.accreditations(service.accreditations());
  //   complete_service.licenses(service.licenses());
  // }


  // void complete(HSDS2::CompleteLocation& complete_location,
  //               const HSDS2::Location& location) const
  // {
  //   // This is going to be horribly inefficient.

  //   complete_location.dpmgid(location.dpmgid());
  //   complete_location.id(location.id());
  //   complete_location.organization_id(location.organization_id());
  //   complete_location.name(location.name());
  //   complete_location.alternate_name(location.alternate_name());
  //   complete_location.description(location.description());
  //   complete_location.transportation(location.transportation());
  //   complete_location.latitude(location.latitude());
  //   complete_location.longitude(location.longitude());

  //   // TODO: Don't know how to distinguish between regular and holiday schedules.
  //   for (const auto& e : schedule_.container) {
  //     if (e.dpmgid() == complete_location.dpmgid() &&
  //         e.location_id() == complete_location.id()) {
  //       complete_location.regular_schedule().push_back(e);
  //       //complete_location.holiday_schedule().push_back(e);
  //     }
  //   }

  //   for (const auto& e : language_.container) {
  //     if (e.dpmgid() == complete_location.dpmgid() &&
  //         e.location_id() == complete_location.id()) {
  //       complete_location.languages().push_back(e);
  //     }
  //   }

  //   for (const auto& e : postal_address_.container) {
  //     if (e.dpmgid() == complete_location.dpmgid() &&
  //         e.location_id() == complete_location.id()) {
  //       complete_location.postal_address().push_back(e);
  //     }
  //   }

  //   for (const auto& e : physical_address_.container) {
  //     if (e.dpmgid() == complete_location.dpmgid() &&
  //         e.location_id() == complete_location.id()) {
  //       complete_location.physical_address().push_back(e);
  //     }
  //   }

  //   for (const auto& e : phone_.container) {
  //     if (e.dpmgid() == complete_location.dpmgid() &&
  //         e.location_id() == complete_location.id()) {
  //       complete_location.phones().push_back(e);
  //     }
  //   }

  //   for (const auto& e : accessibility_for_disabilities_.container) {
  //     if (e.dpmgid() == complete_location.dpmgid() &&
  //         e.location_id() == complete_location.id()) {
  //       complete_location.accessibility_for_disabilities().push_back(e);
  //     }
  //   }
  // }

  // void incomplete(HSDS2::Location& location,
  //                 const HSDS2::CompleteLocation& complete_location) const
  // {
  //   location.dpmgid(complete_location.dpmgid());
  //   location.id(complete_location.id());
  //   location.organization_id(complete_location.organization_id());
  //   location.name(complete_location.name());
  //   location.alternate_name(complete_location.alternate_name());
  //   location.description(complete_location.description());
  //   location.transportation(complete_location.transportation());
  //   location.latitude(complete_location.latitude());
  //   location.longitude(complete_location.longitude());
  // }


  // void complete(HSDS2::CompleteContact& complete_contact,
  //               const HSDS2::Contact& contact) const
  // {
  //   // This is going to be horribly inefficient.

  //   complete_contact.dpmgid(contact.dpmgid());
  //   complete_contact.id(contact.id());
  //   complete_contact.organization_id(contact.organization_id());
  //   complete_contact.service_id(contact.service_id());
  //   complete_contact.service_at_location_id(contact.service_at_location_id());
  //   complete_contact.name(contact.name());
  //   complete_contact.title(contact.title());
  //   complete_contact.department(contact.department());
  //   complete_contact.email(contact.email());

  //   for (const auto& e : phone_.container) {
  //     if (e.dpmgid() == complete_contact.dpmgid() &&
  //         e.contact_id() == complete_contact.id()) {
  //       complete_contact.phones().push_back(e);
  //     }
  //   }
  // }

  // void incomplete(HSDS2::Contact& contact,
  //                 const HSDS2::CompleteContact& complete_contact) const
  // {
  //   contact.dpmgid(complete_contact.dpmgid());
  //   contact.id(complete_contact.id());
  //   contact.organization_id(complete_contact.organization_id());
  //   contact.service_id(complete_contact.service_id());
  //   contact.service_at_location_id(complete_contact.service_at_location_id());
  //   contact.name(complete_contact.name());
  //   contact.title(complete_contact.title());
  //   contact.department(complete_contact.department());
  //   contact.email(complete_contact.email());
  // }

  const std::string& data_path() const { return data_path_; }

  ACE_Thread_Mutex& get_mutex() { return mutex_; }

 private:
  std::string dpm_url_;
  std::string dpm_gid_;
  std::string dpm_application_id_;
  std::string dpm_password_;
  std::string dpm_nonce_;
  std::string relay_config_url_;
  std::string data_path_;
  DDS::DomainId_t domain_id_;

  std::string access_control_allow_origin_;

  unsigned short http_port_;
  bool create_writers_;
  std::string server_url_;
  DDS::Duration_t server_poll_period_;

  std::string identity_ca_;
  std::string permissions_ca_;
  std::string governance_;
  std::string public_key_;
  std::string private_key_;
  std::string permissions_;

  bool rtps_relay_only_;
  std::string spdp_rtps_relay_address_;
  std::string sedp_rtps_relay_address_;
  std::string data_rtps_relay_address_;

  bool enable_http_log_access_;
  bool enable_observer_;

  OpenDDS::DCPS::TransportConfig_rch transport_config_;
  DDS::DomainParticipantFactory_var domain_participant_factory_;
  DDS::DomainParticipant_var participant_;

  // Protect the transaction_ and these Unit objects.
  // A more fine-grained locking approach such as one for the container
  // inside each Unit object may be considered for an improvement.
  ACE_Thread_Mutex mutex_;

  uint64_t transaction_;

  Unit<HSDS3::Accessibility> accessibility_;
  Unit<HSDS3::Address> address_;
  Unit<HSDS3::Attribute> attribute_;
  Unit<HSDS3::Contact> contact_;
  Unit<HSDS3::CostOption> cost_option_;
  Unit<HSDS3::Funding> funding_;
  Unit<HSDS3::Language> language_;
  Unit<HSDS3::Location> location_;
  Unit<HSDS3::MetaTableDescription> meta_table_description_;
  Unit<HSDS3::Metadata> metadata_;
  Unit<HSDS3::Organization> organization_;
  Unit<HSDS3::OrganizationIdentifier> organization_identifier_;
  Unit<HSDS3::Phone> phone_;
  Unit<HSDS3::Program> program_;
  Unit<HSDS3::RequiredDocument> required_document_;
  Unit<HSDS3::Schedule> schedule_;
  Unit<HSDS3::Service> service_;
  Unit<HSDS3::ServiceArea> service_area_;
  Unit<HSDS3::ServiceAtLocation> service_at_location_;
  Unit<HSDS3::Taxonomy> taxonomy_;
  Unit<HSDS3::TaxonomyTerm> taxonomy_term_;
};

template<> Unit<HSDS3::Accessibility>& Application::unit<HSDS3::Accessibility>() { return accessibility_; }
template<> Unit<HSDS3::Address>& Application::unit<HSDS3::Address>() { return address_; }
template<> Unit<HSDS3::Attribute>& Application::unit<HSDS3::Attribute>() { return attribute_; }
template<> Unit<HSDS3::Contact>& Application::unit<HSDS3::Contact>() { return contact_; }
template<> Unit<HSDS3::CostOption>& Application::unit<HSDS3::CostOption>() { return cost_option_; }
template<> Unit<HSDS3::Funding>& Application::unit<HSDS3::Funding>() { return funding_; }
template<> Unit<HSDS3::Language>& Application::unit<HSDS3::Language>() { return language_; }
template<> Unit<HSDS3::Location>& Application::unit<HSDS3::Location>() { return location_; }
template<> Unit<HSDS3::MetaTableDescription>& Application::unit<HSDS3::MetaTableDescription>() { return meta_table_description_; }
template<> Unit<HSDS3::Metadata>& Application::unit<HSDS3::Metadata>() { return metadata_; }
template<> Unit<HSDS3::Organization>& Application::unit<HSDS3::Organization>() { return organization_; }
template<> Unit<HSDS3::OrganizationIdentifier>& Application::unit<HSDS3::OrganizationIdentifier>() { return organization_identifier_; }
template<> Unit<HSDS3::Phone>& Application::unit<HSDS3::Phone>() { return phone_; }
template<> Unit<HSDS3::Program>& Application::unit<HSDS3::Program>() { return program_; }
template<> Unit<HSDS3::RequiredDocument>& Application::unit<HSDS3::RequiredDocument>() { return required_document_; }
template<> Unit<HSDS3::Schedule>& Application::unit<HSDS3::Schedule>() { return schedule_; }
template<> Unit<HSDS3::Service>& Application::unit<HSDS3::Service>() { return service_; }
template<> Unit<HSDS3::ServiceArea>& Application::unit<HSDS3::ServiceArea>() { return service_area_; }
template<> Unit<HSDS3::ServiceAtLocation>& Application::unit<HSDS3::ServiceAtLocation>() { return service_at_location_; }
template<> Unit<HSDS3::Taxonomy>& Application::unit<HSDS3::Taxonomy>() { return taxonomy_; }
template<> Unit<HSDS3::TaxonomyTerm>& Application::unit<HSDS3::TaxonomyTerm>() { return taxonomy_term_; }

// template <>
// DDS::ReturnCode_t Application::insert_and_write<HSDS2::CompleteService>(const HSDS2::CompleteService& element)
// {
//   HSDS2::Service service;
//   incomplete(service, element);

//   if (insert_and_write(service) != DDS::RETCODE_OK) {
//     return DDS::RETCODE_ERROR;
//   }

//   for (const auto& e : element.contacts()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.eligibility()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.funding()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.regular_schedule()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.holiday_schedule()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.language()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.payment_accepted()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.phones()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.required_documents()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.service_area()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   return DDS::RETCODE_OK;
// }

// template <>
// DDS::ReturnCode_t Application::insert_and_write<HSDS2::CompleteLocation>(const HSDS2::CompleteLocation& element)
// {
//   HSDS2::Location location;
//   incomplete(location, element);

//   if (insert_and_write(location) != DDS::RETCODE_OK) {
//     return DDS::RETCODE_ERROR;
//   }

//   for (const auto& e : element.regular_schedule()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.holiday_schedule()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.languages()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.postal_address()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.physical_address()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   for (const auto& e : element.phones()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   // TODO: HSDS is broken. There is no way to populate the service_at_location table.

//   for (const auto& e : element.accessibility_for_disabilities()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   return DDS::RETCODE_OK;
// }

// template <>
// DDS::ReturnCode_t Application::insert_and_write<HSDS2::CompleteContact>(const HSDS2::CompleteContact& element)
// {
//   HSDS2::Contact contact;
//   incomplete(contact, element);

//   if (insert_and_write(contact) != DDS::RETCODE_OK) {
//     return DDS::RETCODE_ERROR;
//   }

//   for (const auto& e : element.phones()) {
//     if (insert_and_write(e) != DDS::RETCODE_OK) {
//       return DDS::RETCODE_ERROR;
//     }
//   }

//   return DDS::RETCODE_OK;
// }

#endif // HSDS_COMMON_APPLICATION_H

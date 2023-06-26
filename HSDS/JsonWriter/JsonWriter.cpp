// Copyright 2023 CommunityUtils Authors

#include "Common.h"

#include <dds/DCPS/JsonValueReader.h>

#include <rapidjson/filereadstream.h>

template <typename T>
void load_and_write(Application& application)
{
  const Unit<T> unit = application.unit<T>();
  const std::string path = application.data_path() + '/' + unit.json_file;
  ACE_DEBUG((LM_INFO, "INFO: load_and_write: Opening %C\n", path.c_str()));
  FILE* fp = fopen(path.c_str(), "r");
  if (fp != NULL) {
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));

    OpenDDS::DCPS::JsonValueReader<rapidjson::FileReadStream> jvr(is);
    if (!jvr.begin_sequence()) {
      ACE_DEBUG((LM_INFO, "INFO: load_and_write: begin_sequence failed (path=%C)\n", path.c_str()));
      fclose(fp);
      return;
    }
    while (jvr.elements_remaining()) {
      if (!jvr.begin_element()) {
        ACE_DEBUG((LM_INFO, "INFO: load_and_write: begin_element failed (path=%C)\n", path.c_str()));
        fclose(fp);
        return;
      }
      T element;
      OpenDDS::DCPS::set_default(element);
      if (!vread(jvr, element)) {
        ACE_DEBUG((LM_INFO, "INFO: load_and_write: vread failed (path=%C)\n", path.c_str()));
        fclose(fp);
        return;
      }
      element.dpmgid(application.dpm_gid());
      if (application.insert_and_write(element) != DDS::RETCODE_OK) {
        ACE_ERROR((LM_ERROR, "ERROR: load_and_write: Could not write\n"));
      }
      // TODO: Check element.
      // Save.
      if (!jvr.end_element()) {
        ACE_DEBUG((LM_INFO, "INFO: load_and_write: end_element failed (path=%C)\n", path.c_str()));
        fclose(fp);
        return;
      }
    }
    if (!jvr.end_sequence()) {
      ACE_DEBUG((LM_INFO, "INFO: load_and_write: end_sequence failed (path=%C)\n", path.c_str()));
      fclose(fp);
      return;
    }

    fclose(fp);
  } else {
    ACE_DEBUG((LM_INFO, "INFO: load_and_write: Could not open %C\n", path.c_str()));
  }
}

int
ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  Application application;
  application.load_environment_variables();
  application.dump_configuration();

  if (application.data_path().empty()) {
    ACE_ERROR((LM_ERROR, "DATA_PATH cannot be empty\n"));
    return EXIT_FAILURE;
  }

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
  application.install_observer();
  if (application.create_topics() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }
  if (application.create_datawriters() != DDS::RETCODE_OK) {
    return EXIT_FAILURE;
  }

  load_and_write<HSDS3::Accessibility>(application);
  load_and_write<HSDS3::Address>(application);
  load_and_write<HSDS3::Attribute>(application);
  load_and_write<HSDS3::Contact>(application);
  load_and_write<HSDS3::CostOption>(application);
  load_and_write<HSDS3::Funding>(application);
  load_and_write<HSDS3::Language>(application);
  load_and_write<HSDS3::Location>(application);
  load_and_write<HSDS3::MetaTableDescription>(application);
  load_and_write<HSDS3::Metadata>(application);
  load_and_write<HSDS3::Organization>(application);
  load_and_write<HSDS3::OrganizationIdentifier>(application);
  load_and_write<HSDS3::Phone>(application);
  load_and_write<HSDS3::Program>(application);
  load_and_write<HSDS3::RequiredDocument>(application);
  load_and_write<HSDS3::Schedule>(application);
  load_and_write<HSDS3::Service>(application);
  load_and_write<HSDS3::ServiceArea>(application);
  load_and_write<HSDS3::ServiceAtLocation>(application);
  load_and_write<HSDS3::Taxonomy>(application);
  load_and_write<HSDS3::TaxonomyTerm>(application);

  GuardWrapper wrapper;
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;

  bool keep_going = true;
  const DDS::Duration_t infinite = { DDS::DURATION_INFINITE_SEC, DDS::DURATION_INFINITE_NSEC };
  while (keep_going) {
    DDS::ReturnCode_t error = ws->wait(active, infinite);
    if (error != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: wait failed!\n"));
    }

    for (unsigned int i = 0; keep_going && i != active.length(); ++i) {
      if (active[i] == wrapper.guard()) {
        keep_going = false;
      }
    }
  }

  ws->detach_condition(wrapper.guard());

  application.shutdown();

  return EXIT_SUCCESS;
}

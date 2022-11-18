#include "Common.h"

#include <ace/Log_Msg.h>

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/JsonValueWriter.h>

#include <CommunitySecurityPlugin.h>

#include <dds/DCPS/StaticIncludes.h>
#ifdef ACE_AS_STATIC_LIBS
#  include <dds/DCPS/RTPS/RtpsDiscovery.h>
#  include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include "TimeTypeSupportImpl.h"

const char auth_ca_path[] = "file:time_writer_security_docs/identity_ca.pem";
const char perm_ca_path[] = "file:time_writer_security_docs/permissions_ca.pem";
const char id_cert_path[] = "file:time_writer_security_docs/identity.pem";
const char id_key_path[] = "file:time_writer_security_docs/identity_key.pem";
const char governance_path[] = "file:time_writer_security_docs/governance.xml.p7s";
const char permissions_path[] = "file:time_writer_security_docs/permissions.xml.p7s";

int
ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  const char* topic_name = getenv("TOPIC");
  if (!topic_name) {
    ACE_ERROR((LM_ERROR, "TOPIC environment variable not set\n"));
    return EXIT_FAILURE;
  }
  ACE_DEBUG((LM_DEBUG, "TOPIC=%C\n", topic_name));

  const char* dpmgid = getenv("DPMGID");
  if (!dpmgid) {
    ACE_ERROR((LM_ERROR, "DPMGID environment variable not set\n"));
    return EXIT_FAILURE;
  }
  ACE_DEBUG((LM_DEBUG, "DPMGID=%C\n", dpmgid));

  Community::install_community_security_plugin();

  DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);

  DDS::DomainParticipantQos part_qos;
  dpf->get_default_participant_qos(part_qos);

  DDS::PropertySeq& props = part_qos.property.value;

  using namespace DDS::Security::Properties;
  append(props, AuthIdentityCA, auth_ca_path);
  append(props, AuthIdentityCertificate, id_cert_path);
  append(props, AuthPrivateKey, id_key_path);
  append(props, AccessPermissionsCA, perm_ca_path);
  append(props, AccessGovernance, governance_path);
  append(props, AccessPermissions, permissions_path);

  DDS::DomainParticipant_var participant =
    dpf->create_participant(Time::TIME_DOMAIN,
                            part_qos,
                            0,
                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

  Time::CurrentTimeTypeSupport_var ts = new Time::CurrentTimeTypeSupportImpl;

  if (ts->register_type(participant, "") != DDS::RETCODE_OK) {
    ACE_ERROR((LM_ERROR, "ERROR: register_type failed\n"));
    return EXIT_FAILURE;
  }

  CORBA::String_var type_name = ts->get_type_name();
  DDS::Topic_var topic =
    participant->create_topic(topic_name,
                              type_name,
                              TOPIC_QOS_DEFAULT,
                              0,
                              OpenDDS::DCPS::DEFAULT_STATUS_MASK);

  if (!topic) {
    ACE_ERROR((LM_ERROR, "ERROR: create_topic failed!\n"));
    return EXIT_FAILURE;
  }

  DDS::Publisher_var publisher =
    participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                  0,
                                  OpenDDS::DCPS::DEFAULT_STATUS_MASK);

  if (!publisher) {
    ACE_ERROR((LM_ERROR, "ERROR: create_publisher failed!\n"));
    return EXIT_FAILURE;
  }

  DDS::DataWriter_var writer =
    publisher->create_datawriter(topic,
                                 DATAWRITER_QOS_DEFAULT,
                                 0,
                                 OpenDDS::DCPS::DEFAULT_STATUS_MASK);

  if (!writer) {
    ACE_ERROR((LM_ERROR, "ERROR: create_datawriter failed!\n"));
    return EXIT_FAILURE;
  }

  Time::CurrentTimeDataWriter_var message_writer =
    Time::CurrentTimeDataWriter::_narrow(writer);

  if (!message_writer) {
    ACE_ERROR((LM_ERROR, "ERROR: narrow failed!\n"));
    return EXIT_FAILURE;
  }

  GuardWrapper wrapper;
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;
  const DDS::Duration_t one_second = {1, 0};

  Time::CurrentTime message;
  message.dpmgid = CORBA::string_dup(dpmgid);

  for (;;) {
    DDS::ReturnCode_t error = ws->wait(active, one_second);
    if (error == DDS::RETCODE_TIMEOUT) {
      DDS::ReturnCode_t error = participant->get_current_time(message.current_time);
      if (error != DDS::RETCODE_OK) {
        ACE_ERROR((LM_ERROR, "ERROR: get_current_time returned %d!\n", error));
      }

      error = message_writer->write(message, DDS::HANDLE_NIL);
      if (error != DDS::RETCODE_OK) {
        ACE_ERROR((LM_ERROR, "ERROR: write returned %d!\n", error));
      }
      std::cout << "Sent " << OpenDDS::DCPS::to_json(message) << std::endl;
    } else if (error == DDS::RETCODE_OK) {
      break;
    } else {
      ACE_ERROR((LM_ERROR, "ERROR: wait returned %d!\n", error));
    }
  }

  ws->detach_condition(wrapper.guard());
  participant->delete_contained_entities();
  dpf->delete_participant(participant);
  TheServiceParticipant->shutdown();

  return EXIT_SUCCESS;
}

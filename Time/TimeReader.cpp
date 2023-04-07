// Copyright 2023 CommunityUtils Authors

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

const char auth_ca_path[] = "file:time_reader_security_docs/identity_ca.pem";
const char perm_ca_path[] = "file:time_reader_security_docs/permissions_ca.pem";
const char id_cert_path[] = "file:time_reader_security_docs/identity.pem";
const char id_key_path[] = "file:time_reader_security_docs/identity_key.pem";
const char governance_path[] = "file:time_reader_security_docs/governance.xml.p7s";
const char permissions_path[] = "file:time_reader_security_docs/permissions.xml.p7s";

int
ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  const char* topic_name = getenv("TOPIC");
  if (!topic_name) {
    ACE_ERROR((LM_ERROR, "TOPIC environment variable not set\n"));
    return EXIT_FAILURE;
  }
  ACE_DEBUG((LM_DEBUG, "TOPIC=%C\n", topic_name));

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

  if (!participant) {
    ACE_ERROR((LM_ERROR, "ERROR: create_participant failed!\n"));
    return EXIT_FAILURE;
  }

  Time::CurrentTimeTypeSupport_var ts =
    new Time::CurrentTimeTypeSupportImpl;

  if (ts->register_type(participant, "") != DDS::RETCODE_OK) {
    ACE_ERROR((LM_ERROR, "ERROR: register_type failed!\n"));
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

  DDS::Subscriber_var subscriber =
    participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                   0,
                                   OpenDDS::DCPS::DEFAULT_STATUS_MASK);

  if (!subscriber) {
    ACE_ERROR((LM_ERROR, "ERROR: create_subscriber failed!\n"));
    return EXIT_FAILURE;
  }

  DDS::DataReaderQos reader_qos;
  subscriber->get_default_datareader_qos(reader_qos);
  reader_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

  DDS::DataReader_var reader =
    subscriber->create_datareader(topic,
                                  reader_qos,
                                  0,
                                  OpenDDS::DCPS::DEFAULT_STATUS_MASK);

  if (!reader) {
    ACE_ERROR((LM_ERROR, "ERROR: create_datareader failed!\n"));
    return EXIT_FAILURE;
  }

  Time::CurrentTimeDataReader_var reader_i =
    Time::CurrentTimeDataReader::_narrow(reader);

  if (!reader_i) {
    ACE_ERROR((LM_ERROR, "ERROR: narrow failed!\n"));
    return EXIT_FAILURE;
  }

  GuardWrapper wrapper;
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;
  const DDS::Duration_t infinite = {DDS::DURATION_INFINITE_SEC, DDS::DURATION_INFINITE_NSEC};

  DDS::ReadCondition_var cond = reader->create_readcondition(DDS::ANY_SAMPLE_STATE,
                                                             DDS::ANY_VIEW_STATE,
                                                             DDS::ANY_INSTANCE_STATE);
  ws->attach_condition(cond);

  bool keep_going = true;
  while (keep_going) {
    DDS::ReturnCode_t error = ws->wait(active, infinite);
    if (error != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: wait failed!\n"));
      continue;
    }

    for (unsigned int i = 0; keep_going && i != active.length(); ++i) {
      if (active[i] == cond) {
        Time::CurrentTime message;
        DDS::SampleInfo info;

        const DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);

        if (error == DDS::RETCODE_OK) {
          if (info.valid_data) {
            std::cout << "Recv " << OpenDDS::DCPS::to_json(message) << std::endl;
          }
        } else {
          ACE_ERROR((LM_ERROR, "ERROR: take_next_sample failed!\n"));
        }
      } else {
        keep_going = false;
      }
    }
  }

  ws->detach_condition(cond);
  ws->detach_condition(wrapper.guard());
  participant->delete_contained_entities();
  dpf->delete_participant(participant);
  TheServiceParticipant->shutdown();

  return 0;
}

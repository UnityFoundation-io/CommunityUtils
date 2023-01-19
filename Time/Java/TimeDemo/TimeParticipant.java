package TimeDemo;

import DDS.*;
import OpenDDS.DCPS.*;
import Time.*;
import java.util.ArrayList;

public class TimeParticipant {

    final static String AccessGovernance = "dds.sec.access.governance";
    final static String AccessPermissions = "dds.sec.access.permissions";
    final static String AccessPermissionsCA = "dds.sec.access.permissions_ca";
    final static String AuthIdentityCA = "dds.sec.auth.identity_ca";
    final static String AuthIdentityCertificate = "dds.sec.auth.identity_certificate";
    final static String AuthPassword = "dds.sec.auth.password";
    final static String AuthPrivateKey = "dds.sec.auth.private_key";

    public TimeParticipant(String[] args, String docsDir) {
        Community.SecurityPlugin.Install();
        final DomainParticipantFactory dpf = TheParticipantFactory.WithArgs(new org.omg.CORBA.StringSeqHolder(args));
        if (dpf == null) {
            throw new RuntimeException("ERROR: Domain Participant Factory not found");
        }

        final DomainParticipantQosHolder qos = new DomainParticipantQosHolder(new DomainParticipantQos());
        qos.value.user_data = new UserDataQosPolicy(new byte[]{});
        qos.value.entity_factory = new EntityFactoryQosPolicy();
        qos.value.property = new PropertyQosPolicy(new Property_t[]{}, new BinaryProperty_t[]{});
        dpf.get_default_participant_qos(qos);

        final String prefix = "file:" + docsDir + '/';
        final ArrayList<Property_t> props = new ArrayList<Property_t>();
        append(props, AuthIdentityCA, prefix + "identity_ca.pem");
        append(props, AuthIdentityCertificate, prefix + "identity.pem");
        append(props, AuthPrivateKey, prefix + "identity_key.pem");
        append(props, AccessPermissionsCA, prefix + "permissions_ca.pem");
        append(props, AccessGovernance, prefix + "governance.xml.p7s");
        append(props, AccessPermissions, prefix + "permissions.xml.p7s");
        qos.value.property.value = props.toArray(qos.value.property.value);

        domain_participant_ = dpf.create_participant(TIME_DOMAIN.value, qos.value, null, DEFAULT_STATUS_MASK.value);
        if (domain_participant_ == null) {
            throw new RuntimeException("ERROR: Domain Participant creation failed");
        }
    }

    public String getTopicName() {
        return System.getenv("TOPIC");
    }

    public String getDpmgid() {
        return System.getenv("DPMGID");
    }

    public DomainParticipant getDomainParticipant() {
        return domain_participant_;
    }

    private static void append(ArrayList<Property_t> list, String name, String value) {
        list.add(new Property_t(name, value, false));
    }

    private DomainParticipant domain_participant_;
}

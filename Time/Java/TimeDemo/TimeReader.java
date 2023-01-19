package TimeDemo;

import DDS.*;
import OpenDDS.DCPS.*;
import Time.*;

public abstract class TimeReader {
    public static void main(String[] args) {
        final TimeParticipant tp = new TimeParticipant(args, "time_reader_security_docs");
        final DomainParticipant dp = tp.getDomainParticipant();

        final CurrentTimeTypeSupportImpl ts = new CurrentTimeTypeSupportImpl();
        if (ts.register_type(dp, "") != RETCODE_OK.value) {
            System.err.println("ERROR: register_type failed");
            return;
        }

        final Topic top = dp.create_topic(tp.getTopicName(), ts.get_type_name(), TOPIC_QOS_DEFAULT.get(), null, DEFAULT_STATUS_MASK.value);
        if (top == null) {
            System.err.println("ERROR: Topic creation failed");
            return;
        }

        final Subscriber sub = dp.create_subscriber(SUBSCRIBER_QOS_DEFAULT.get(), null, DEFAULT_STATUS_MASK.value);
        if (sub == null) {
            System.err.println("ERROR: Subscriber creation failed");
            return;
        }

        final DataReaderQosHolder qos_holder = new DataReaderQosHolder(DATAREADER_QOS_DEFAULT.get());
        sub.get_default_datareader_qos(qos_holder);
        final DataReaderQos dr_qos = qos_holder.value;
        dr_qos.reliability.kind = ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;

        final DataReader dr = sub.create_datareader(top, dr_qos, null, DEFAULT_STATUS_MASK.value);
        final CurrentTimeDataReader mdr = CurrentTimeDataReaderHelper.narrow(dr);
        if (mdr == null) {
            System.err.println("ERROR: CurrentTimeDataReaderHelper.narrow failed");
            return;
        }

        final CurrentTimeHolder mh = new CurrentTimeHolder(new CurrentTime());
        final SampleInfoHolder sih = new SampleInfoHolder(new SampleInfo());
        sih.value.source_timestamp = new Time_t();

        final WaitSet ws = new WaitSet();
        final Duration_t infinite = new Duration_t(DURATION_INFINITE_SEC.value, DURATION_INFINITE_NSEC.value);

        final ReadCondition cond = dr.create_readcondition(ANY_SAMPLE_STATE.value, ANY_VIEW_STATE.value, ANY_INSTANCE_STATE.value);
        ws.attach_condition(cond);

        while (true) {
            final ConditionSeqHolder active = new ConditionSeqHolder(new Condition[]{});
            if (ws.wait(active, infinite) != RETCODE_OK.value) {
                System.err.println("ERROR: wait failed");
                return;
            }

            final CurrentTimeHolder time = new CurrentTimeHolder(new CurrentTime("", new Time_t()));
            final SampleInfoHolder info = new SampleInfoHolder(new SampleInfo());
            info.value.source_timestamp = new Time_t();
            final int error = mdr.take_next_sample(time, info);

            if (error == RETCODE_OK.value) {
                if (info.value.valid_data) {
                    System.out.println(toString(time.value));
                }
            } else {
                System.err.println("ERROR: take_next_sample failed");
                return;
            }
        }
    }

    private static String toString(CurrentTime ct) {
        return "DPMGID: " + ct.dpmgid + " seconds: " + ct.current_time.sec + " nanos: " + ct.current_time.nanosec;
    }
}

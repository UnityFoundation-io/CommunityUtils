package TimeDemo;

import DDS.*;
import OpenDDS.DCPS.*;
import Time.*;

public abstract class TimeWriter {
    public static void main(String[] args) {
        final TimeParticipant tp = new TimeParticipant(args, "time_writer_security_docs");
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

        final Publisher pub = dp.create_publisher(PUBLISHER_QOS_DEFAULT.get(), null, DEFAULT_STATUS_MASK.value);
        if (pub == null) {
            System.err.println("ERROR: Publisher creation failed");
            return;
        }

        final DataWriter dw = pub.create_datawriter(top, DATAWRITER_QOS_DEFAULT.get(), null, DEFAULT_STATUS_MASK.value);
        if (dw == null) {
            System.err.println("ERROR: DataWriter creation failed");
            return;
        }

        final CurrentTimeDataWriter mdw = CurrentTimeDataWriterHelper.narrow(dw);
        final CurrentTime sample = new CurrentTime(tp.getDpmgid(), null);

        while (true) {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException ie) {
                System.err.println("sleep: " + ie);
            }
            Time_tHolder ct = new Time_tHolder();
            if (dp.get_current_time(ct) != RETCODE_OK.value) {
                System.err.println("ERROR: failed to get time from DomainParticipant");
                break;
            }
            sample.current_time = ct.value;
            final int ret = mdw.write(sample, HANDLE_NIL.value);
            if (ret != RETCODE_OK.value) {
                System.err.println("ERROR: write returned " + ret);
                break;
            }
        }
    }
}

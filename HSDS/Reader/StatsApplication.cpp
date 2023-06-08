#include "StatsApplication.h"

// Entry point for collecting all stats data.
void StatsApplication::run()
{
  GuardWrapper wrapper;
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;

  bool keep_going = true;
  while (keep_going) {
    DDS::ReturnCode_t ret = ws->wait(active, application_.reader_stats_interval());
    if (ret == DDS::RETCODE_TIMEOUT) {
      collect_datapoints();
    } else if (ret != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: StatsApplication::run_stats: wait failed!\n"));
    }

    for (unsigned int i = 0; keep_going && i < active.length(); ++i) {
      if (active[i] == wrapper.guard()) {
        keep_going = false;
      }
    }
  }

  ws->detach_condition(wrapper.guard());
}

void StatsApplication::collect_datapoints()
{
  organization_stats_.collect_datapoint();
  location_stats_.collect_datapoint();
  service_stats_.collect_datapoint();
}

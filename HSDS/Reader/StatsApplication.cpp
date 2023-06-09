#include "StatsApplication.h"
#include "Common.h"

#include <dds/DCPS/WaitSet.h>

// Entry point for collecting all stats data.
void StatsApplication::run()
{
  GuardWrapper wrapper;
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(wrapper.guard());
  DDS::ConditionSeq active;

  bool keep_going = true;
  while (keep_going) {
    const DDS::ReturnCode_t ret = ws->wait(active, application_.reader_stats_interval());
    if (ret == DDS::RETCODE_TIMEOUT) {
      collect_datapoints();
    } else if (ret != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, "ERROR: StatsApplication::run_stats: wait failed: %C\n", OpenDDS::DCPS::retcode_to_string(ret)));
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
  const OpenDDS::DCPS::SystemTimePoint now = OpenDDS::DCPS::SystemTimePoint::now();
  organization_stats_.collect_datapoint(now);
  location_stats_.collect_datapoint(now);
  service_stats_.collect_datapoint(now);
}

template<> Stats<HSDS3::Organization>& StatsApplication::get_stats<HSDS3::Organization>()
{ return organization_stats_; }

template<> Stats<HSDS3::Location>& StatsApplication::get_stats<HSDS3::Location>()
{ return location_stats_; }

template<> Stats<HSDS3::Service>& StatsApplication::get_stats<HSDS3::Service>()
{ return service_stats_; }

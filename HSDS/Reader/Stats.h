#ifndef HSDS_READER_STATS_H
#define HSDS_READER_STATS_H

#include "Application.h"

#include <dds/DCPS/TimeTypes.h>

struct DataPoint {
  DCPS::SystemTimePoint timestamp;
  size_t record_count;
  long writer_count;
};

template <typename T>
class Stats {
public:
  Stats(const Application& application) : application_(application) {}

  // Return all records in a duration from now.
  vector<DataPoint> query(DCPS::TimeDuration duration)
  {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, stats_mutex_, vector<DataPoint>());
    const DCPS::SystemTimePoint now = DCPS::SystemTimePoint::now();
    if (now.value() <= duration.value()) {
      return stats_;
    }

    vector<DataPoint> result;
    const DCPS::SystemTimePoint start_timepoint = now - duration;
    for (const_iterator it = stats_.rbegin(); it != stats_.rend(); ++it) {
      if (it->timestamp < start_timepoint) {
        break;
      }
      result.insert(result.begin(), *it);
    }
    return result;
  }

  vector<DataPoint> query()
  {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, stats_mutex_, vector<DataPoint>());
    return stats_;
  }

  void collect_datapoint()
  {
    DataPoint dp;
    {
      ACE_GUARD(ACE_Thread_Mutex, g, application_.get_mutex());
      dp.timestamp = DCPS::SystemTimePoint::now();
      const Unit<T>& unit = application_.unit<T>();
      dp.record_count = unit.container.size();

      DDS::InstanceHandleSeq handles;
      if (unit.reader->get_matched_publications(handles) == DDS::RETCODE_OK) {
        dp.writer_count = handles.length();
      } else {
        // Writer count information is not available.
        dp.writer_count = -1;
      }
    }

    ACE_GUARD(ACE_Thread_Mutex, g, stats_mutex_);
    stats_.push_back(dp);
  }

private:
  Application application_;
  ACE_Thread_Mutex stats_mutex_;

  // TODO(sonndinh): Limit the maximum number of stored data points.
  // Use map with keys are timestamps?
  vector<DataPoint> stats_;
};

#endif // HSDS_READER_STATS_H

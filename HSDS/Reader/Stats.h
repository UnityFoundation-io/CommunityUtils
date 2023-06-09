#ifndef HSDS_READER_STATS_H
#define HSDS_READER_STATS_H

#include "Application.h"

#include <dds/DCPS/TimeTypes.h>

#include <vector>

struct DataPoint {
  OpenDDS::DCPS::SystemTimePoint timestamp;
  size_t record_count;
  long writer_count;
};

template <typename T>
class Stats {
public:
  Stats(Application& application, size_t capacity)
    : application_(application)
    , capacity_(capacity) {}

  // Return all records in a duration from now.
  std::vector<DataPoint> query(OpenDDS::DCPS::TimeDuration duration)
  {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, stats_mutex_, std::vector<DataPoint>());
    const OpenDDS::DCPS::SystemTimePoint now = OpenDDS::DCPS::SystemTimePoint::now();
    if (now.value() <= duration.value()) {
      return convert_to_vector();
    }

    std::vector<DataPoint> result;
    const OpenDDS::DCPS::SystemTimePoint start_timepoint = now - duration;
    for (StatsContainer::const_reverse_iterator it = stats_.rbegin(); it != stats_.rend(); ++it) {
      if (it->first < start_timepoint) {
        break;
      }
      result.insert(result.begin(), it->second);
    }
    return result;
  }

  std::vector<DataPoint> query()
  {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, stats_mutex_, std::vector<DataPoint>());
    return convert_to_vector();
  }

  void collect_datapoint()
  {
    DataPoint dp;
    {
      ACE_GUARD(ACE_Thread_Mutex, g, application_.get_mutex());
      dp.timestamp = OpenDDS::DCPS::SystemTimePoint::now();
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
    stats_.insert(std::make_pair(dp.timestamp, dp));
    if (stats_.size() > capacity_) {
      // Delete the oldest entry.
      stats_.erase(stats_.begin());
    }
  }

private:
  std::vector<DataPoint> convert_to_vector()
  {
    std::vector<DataPoint> result(stats_.size());
    size_t i = 0;
    for (StatsContainer::const_iterator it = stats_.begin(); it != stats_.end(); ++it, ++i) {
      result[i] = it->second;
    }
    return result;
  }

  Application& application_;

  // Maximum number of entries for statistics.
  const size_t capacity_;

  // TODO: Can use a circular buffer for better performance.
  typedef std::map<OpenDDS::DCPS::SystemTimePoint, DataPoint> StatsContainer;
  ACE_Thread_Mutex stats_mutex_;
  StatsContainer stats_;
};

#endif // HSDS_READER_STATS_H

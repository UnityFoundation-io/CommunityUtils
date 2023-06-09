#ifndef HSDS_READER_STATS_APPLICATION_H
#define HSDS_READER_STATS_APPLICATION_H

#include "Stats.h"

class StatsApplication {
public:
  StatsApplication(Application& application, size_t stats_cap)
    : organization_stats_(application, stats_cap)
    , location_stats_(application, stats_cap)
    , service_stats_(application, stats_cap)
    , application_(application)
  {}

  void run();

  template <typename T>
  Stats<T>& get_stats();

  const Application& application() const { return application_; }

private:
  void collect_datapoints();

  Stats<HSDS3::Organization> organization_stats_;
  Stats<HSDS3::Location> location_stats_;
  Stats<HSDS3::Service> service_stats_;

  Application& application_;
};

#endif // HSDS_READER_STATS_APPLICATION_H

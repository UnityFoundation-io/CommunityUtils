#ifndef HSDS_READER_STATS_APPLICATION_H
#define HSDS_READER_STATS_APPLICATION_H

#include "Stats.h"

class StatsApplication {
public:
  StatsApplication(const Application& application)
    : organization_stats_(application)
    , location_stats_(application)
    , service_stats_(application)
  {}

  void run();

  template <typename T>
  const Stats<T>& get_stats() const;

private:
  void collect_datapoint();

  Stats<HSDS3::Organization> organization_stats_;
  Stats<HSDS3::Location> location_stats_;
  Stats<HSDS3::Service> service_stats_;
};

template<> const Stats<HSDS3::Organization>& StatsApplication::get_stats<HSDS3::Organization>() const
{ return organization_stats_; }
template<> const Stats<HSDS3::Location>& StatsApplication::get_stats<HSDS3::Location>() const
{ return location_stats_; }
template<> const Stats<HSDS3::Service>& StatsApplication::gte_stats<HSDS3::Service>() const
{ return service_stats_; }

#endif // HSDS_READER_STATS_APPLICATION_H

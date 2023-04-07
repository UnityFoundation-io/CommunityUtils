#ifndef TIME_COMMON_H
#define TIME_COMMON_H

// Copyright 2023 CommunityUtils Authors

#include <dds/DCPS/GuardCondition.h>
#include <dds/DCPS/security/framework/Properties.h>
#include <dds/DdsDcpsCoreC.h>

#include <ace/Sig_Handler.h>

inline void append(DDS::PropertySeq& props, const char* name, const char* value, bool propagate = false)
{
  const DDS::Property_t prop = {name, value, propagate};
  const unsigned int len = props.length();
  props.length(len + 1);
  props[len] = prop;
}

class GuardWrapper : public ACE_Event_Handler {
public:
  GuardWrapper()
    : gc_(new DDS::GuardCondition)
  {
    handler_.register_handler(SIGTERM, this);
    handler_.register_handler(SIGINT, this);
  }

  int handle_signal(int, siginfo_t*, ucontext_t*)
  {
    gc_->set_trigger_value(true);
    return 0;
  }

  DDS::GuardCondition_var guard() const { return gc_; }

private:
  DDS::GuardCondition_var gc_;
  ACE_Sig_Handler handler_;

};

#endif // TIME_COMMON_H

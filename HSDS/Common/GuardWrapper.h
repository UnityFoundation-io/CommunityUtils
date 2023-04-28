#ifndef HSDS_COMMON_GUARD_WRAPPER_H
#define HSDS_COMMON_GUARD_WRAPPER_H

// Copyright 2023 CommunityUtils Authors

#include <dds/DCPS/GuardCondition.h>

#include <ace/Sig_Handler.h>

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

#endif // HSDS_COMMON_GUARD_WRAPPER_H

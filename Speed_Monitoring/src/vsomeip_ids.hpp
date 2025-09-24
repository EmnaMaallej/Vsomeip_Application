#ifndef VSOMEIP_IDS_HPP
#define VSOMEIP_IDS_HPP

// Service and Instance IDs
const vsomeip::service_t SERVICE_SPEEDREQUEST = 0x1234;
const vsomeip::instance_t INSTANCE_SPEEDREQUEST = 0x0001;
const vsomeip::service_t SERVICE_SPEEDVALUE = 0x5678;
const vsomeip::instance_t INSTANCE_SPEEDVALUE = 0x0001;

// Method and Event IDs
const vsomeip::method_t METHOD_SPEEDREQUEST = 0x0001;
const vsomeip::method_t METHOD_SPEEDVALUE = 0x0002;
const vsomeip::event_t EVENT_SPEEDALERT = 0x0003;

// Eventgroup ID
const vsomeip::eventgroup_t EVENTGROUP_SPEEDALERT = 0x0004;

#endif

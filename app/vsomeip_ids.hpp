#pragma once
#include <vsomeip/vsomeip.hpp>

// ---- Service: Client2 (RPM provider) ----
#define SERVICE_SPEEDREQUEST     0x1111
#define INSTANCE_SPEEDREQUEST    0x0001
#define METHOD_SPEEDREQUEST      0x0001

// ---- Service: Server (Converted speed receiver) ----
#define SERVICE_SPEEDVALUE       0x2222
#define INSTANCE_SPEEDVALUE      0x0001
#define METHOD_SPEEDVALUE        0x0001

// ---- Events ----
#define EVENTGROUP_SPEEDALERT    0x0001
#define EVENT_SPEEDALERT         0x1001

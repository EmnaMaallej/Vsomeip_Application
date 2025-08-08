# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(VSomeIPSpeedProject LANGUAGES CXX)

# Define C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(vsomeip3 3.1.20 REQUIRED) # Specify minimum vsomeip3 version

# Find vsomeip using pkg-config
pkg_check_modules(VSOMEIP REQUIRED vsomeip3>=3.1.20)

# Include directories
include_directories(${VSOMEIP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VSOMEIP_CFLAGS_OTHER}")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Define configuration directory
set(CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Create vsomeip_ids.hpp if it doesn't exist
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vsomeip_ids.hpp")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/vsomeip_ids.hpp" 
"#ifndef VSOMEIP_IDS_HPP
#define VSOMEIP_IDS_HPP

// Service IDs
#define SERVICE_SPEEDREQUEST 0x4321  // svc_SpeedRequest (Client2 provides)
#define SERVICE_SPEEDVALUE   0x1234  // svc_SpeedValue (Server provides)

// Instance IDs
#define INSTANCE_SPEEDREQUEST 0x0001 // Instance for svc_SpeedRequest
#define INSTANCE_SPEEDVALUE   0x0001 // Instance for svc_SpeedValue

// Method IDs
#define METHOD_SPEEDREQUEST 0x0003   // SpeedRequest method (Client2)
#define METHOD_SPEEDVALUE   0x0002   // speedValue method (Server)

// Event IDs
#define EVENT_SPEEDALERT    0x0004   // SpeedAlert_Event

// Event Group IDs (for notifications)
#define EVENTGROUP_SPEEDALERT 0x8001 // SpeedAlert notification group

#endif // VSOMEIP_IDS_HPP
")
endif()

# Define executables
add_executable(server server.cpp)
add_executable(client1 client1.cpp)
add_executable(client2 client2.cpp)

# Link libraries for all executables
foreach(target server client1 client2)
    target_link_libraries(${target}
        ${VSOMEIP_LIBRARIES}
        Threads::Threads
    )
endforeach()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build: Debug Release" FORCE)
endif()

# Generate configuration files with customizable IP addresses
set(SERVER_IP "192.168.0.1" CACHE STRING "Server IP address")
set(CLIENT1_IP "192.168.0.2" CACHE STRING "Client1 IP address")
set(CLIENT2_IP "192.168.0.3" CACHE STRING "Client2 IP address")
set(NETWORK_INTERFACE "eth0" CACHE STRING "Network interface for vSomeIP")

if(NOT EXISTS "${CONFIG_DIR}/server.json")
    file(WRITE "${CONFIG_DIR}/server.json" 
"{
  \"unicast\": \"${SERVER_IP}\",
  \"logging\": {
    \"level\": \"debug\"
  },
  \"applications\": [
    {
      \"name\": \"Server\",
      \"id\": \"0x3333\"
    }
  ],
  \"services\": [
    {
      \"service\": \"0x1234\",
      \"instance\": \"0x0001\",
      \"unreliable\": \"30509\",
      \"methods\": [
        {
          \"method\": \"0x0002\"
        }
      ],
      \"events\": [
        {
          \"event\": \"0x0004\",
          \"is_field\": false,
          \"is_reliable\": true
        }
      ],
      \"eventgroups\": [
        {
          \"eventgroup\": \"0x8001\",
          \"events\": [
            \"0x0004\"
          ]
        }
      ]
    }
  ],
  \"network\": {
    \"interfaces\": [
      {
        \"name\": \"${NETWORK_INTERFACE}\",
        \"ip\": \"${SERVER_IP}\"
      }
    ]
  },
  \"service-discovery\": {
    \"enable\": true,
    \"multicast\": \"224.0.0.1\",
    \"port\": \"30490\",
    \"protocol\": \"udp\",
    \"initial_delay_min\": \"10\",
    \"initial_delay_max\": \"100\",
    \"repetitions_base_delay\": \"200\",
    \"repetitions_max\": \"3\",
    \"ttl\": \"3\",
    \"cyclic_offer_delay\": \"2000\",
    \"request_response_delay\": \"1500\"
  }
}")
endif()

if(NOT EXISTS "${CONFIG_DIR}/client1.json")
    file(WRITE "${CONFIG_DIR}/client1.json" 
"{
  \"unicast\": \"${CLIENT1_IP}\",
  \"logging\": {
    \"level\": \"debug\"
  },
  \"applications\": [
    {
      \"name\": \"Client1\",
      \"id\": \"0x1111\"
    }
  ],
  \"services\": [
    {
      \"service\": \"0x1234\",
      \"instance\": \"0x0001\",
      \"unreliable\": \"30509\",
      \"methods\": [
        {
          \"method\": \"0x0002\"
        }
      ]
    },
    {
      \"service\": \"0x4321\",
      \"instance\": \"0x0001\",
      \"unreliable\": \"30509\",
      \"methods\": [
        {
          \"method\": \"0x0003\"
        }
      ]
    }
  ],
  \"eventgroups\": [
    {
      \"service\": \"0x1234\",
      \"instance\": \"0x0001\",
      \"eventgroup\": \"0x8001\",
      \"events\": [
        \"0x0004\"
      ]
    }
  ],
  \"network\": {
    \"interfaces\": [
      {
        \"name\": \"${NETWORK_INTERFACE}\",
        \"ip\": \"${CLIENT1_IP}\"
      }
    ]
  },
  \"service-discovery\": {
    \"enable\": true,
    \"multicast\": \"224.0.0.1\",
    \"port\": \"30490\",
    \"protocol\": \"udp\",
    \"initial_delay_min\": \"10\",
    \"initial_delay_max\": \"100\",
    \"repetitions_base_delay\": \"200\",
    \"repetitions_max\": \"3\",
    \"ttl\": \"3\",
    \"cyclic_offer_delay\": \"2000\",
    \"request_response_delay\": \"1500\"
  }
}")
endif()

if(NOT EXISTS "${CONFIG_DIR}/client2.json")
    file(WRITE "${CONFIG_DIR}/client2.json" 
"{
  \"unicast\": \"${CLIENT2_IP}\",
  \"logging\": {
    \"level\": \"debug\"
  },
  \"applications\": [
    {
      \"name\": \"Client2\",
      \"id\": \"0x2222\"
    }
  ],
  \"services\": [
    {
      \"service\": \"0x4321\",
      \"instance\": \"0x0001\",
      \"unreliable\": \"30509\",
      \"methods\": [
        {
          \"method\": \"0x0003\"
        }
      ]
    }
  ],
  \"network\": {
    \"interfaces\": [
      {
        \"name\": \"${NETWORK_INTERFACE}\",
        \"ip\": \"${CLIENT2_IP}\"
      }
    ]
  },
  \"service-discovery\": {
    \"enable\": true,
    \"multicast\": \"224.0.0.1\",
    \"port\": \"30490\",
    \"protocol\": \"udp\",
    \"initial_delay_min\": \"10\",
    \"initial_delay_max\": \"100\",
    \"repetitions_base_delay\": \"200\",
    \"repetitions_max\": \"3\",
    \"ttl\": \"3\",
    \"cyclic_offer_delay\": \"2000\",
    \"request_response_delay\": \"1500\"
  }
}")
endif()

# Installation
install(TARGETS server client1 client2 DESTINATION bin)
install(FILES 
    ${CONFIG_DIR}/server.json 
    ${CONFIG_DIR}/client1.json 
    ${CONFIG_DIR}/client2.json 
    DESTINATION config
)

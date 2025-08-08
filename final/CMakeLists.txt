cmake_minimum_required(VERSION 3.10)
project(VSomeIPSpeedProject)

# Définir la version C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trouver les packages requis
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Trouver vsomeip
pkg_check_modules(VSOMEIP REQUIRED vsomeip3)

# Ajouter les chemins d'include
include_directories(${VSOMEIP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Définir les flags de compilation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VSOMEIP_CFLAGS_OTHER}")

# Créer le fichier d'en-tête avec les IDs mis à jour
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

# Exécutable server
add_executable(server server.cpp)
target_link_libraries(server 
    ${VSOMEIP_LIBRARIES}
    Threads::Threads
)
target_compile_definitions(server PRIVATE ${VSOMEIP_CFLAGS_OTHER})

# Exécutable client1
add_executable(client1 client1.cpp)
target_link_libraries(client1 
    ${VSOMEIP_LIBRARIES}
    Threads::Threads
)
target_compile_definitions(client1 PRIVATE ${VSOMEIP_CFLAGS_OTHER})

# Exécutable client2
add_executable(client2 client2.cpp)
target_link_libraries(client2 
    ${VSOMEIP_LIBRARIES}
    Threads::Threads
)
target_compile_definitions(client2 PRIVATE ${VSOMEIP_CFLAGS_OTHER})

# Options de compilation pour debug
set(CMAKE_BUILD_TYPE Debug)
target_compile_options(server PRIVATE -Wall -Wextra -g)
target_compile_options(client1 PRIVATE -Wall -Wextra -g)
target_compile_options(client2 PRIVATE -Wall -Wextra -g)

# Générer les fichiers de configuration par défaut
set(CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR})
if(NOT EXISTS "${CONFIG_DIR}/server.json")
    file(WRITE "${CONFIG_DIR}/server.json" 
"{
  \"unicast\": \"192.168.0.1\",
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
      \"service\": \"0x5678\",
      \"instance\": \"0x0001\",
      \"unreliable\": \"30509\",
      \"methods\": [
        {
          \"method\": \"0x0002\"
        }
      ],
      \"events\": [
        {
          \"event\": \"0x0003\",
          \"is_field\": false,
          \"is_reliable\": true
        }
      ],
      \"eventgroups\": [
        {
          \"eventgroup\": \"0x0004\",
          \"events\": [
            \"0x0003\"
          ]
        }
      ]
    }
  ],
  \"network\": {
    \"interfaces\": [
      {
        \"name\": \"server-veth\",
        \"ip\": \"192.168.0.1\"
      }
    ]
  },
  \"service-discovery\": {
    \"enable\": \"true\",
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
  \"unicast\": \"192.168.0.2\",
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
          \"method\": \"0x0001\"
        }
      ]
    },
    {
      \"service\": \"0x5678\",
      \"instance\": \"0x0001\",
      \"unreliable\": \"30509\",
      \"methods\": [
        {
          \"method\": \"0x0002\"
        }
      ]
    }
  ],
  \"eventgroups\": [
    {
      \"service\": \"0x5678\",
      \"instance\": \"0x0001\",
      \"eventgroup\": \"0x0004\",
      \"events\": [
        \"0x0003\"
      ]
    }
  ],
  \"network\": {
    \"interfaces\": [
      {
        \"name\": \"client1-veth\",
        \"ip\": \"192.168.0.2\"
      }
    ]
  },
  \"service-discovery\": {
    \"enable\": \"true\",
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
  \"unicast\": \"192.168.0.3\",
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
      \"service\": \"0x1234\",
      \"instance\": \"0x0001\",
      \"unreliable\": \"30509\",
      \"methods\": [
        {
          \"method\": \"0x0001\"
        }
      ]
    }
  ],
  \"network\": {
    \"interfaces\": [
      {
        \"name\": \"client2-veth\",
        \"ip\": \"192.168.0.3\"
      }
    ]
  },
  \"service-discovery\": {
    \"enable\": \"true\",
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

# Copier les fichiers de configuration dans le répertoire de build
configure_file(${CONFIG_DIR}/server.json ${CMAKE_CURRENT_BINARY_DIR}/server.json COPYONLY)
configure_file(${CONFIG_DIR}/client1.json ${CMAKE_CURRENT_BINARY_DIR}/client1.json COPYONLY)
configure_file(${CONFIG_DIR}/client2.json ${CMAKE_CURRENT_BINARY_DIR}/client2.json COPYONLY)

# Installation
install(TARGETS server client1 client2 DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/server.json ${CMAKE_CURRENT_BINARY_DIR}/client1.json ${CMAKE_CURRENT_BINARY_DIR}/client2.json DESTINATION /speed/final/build)

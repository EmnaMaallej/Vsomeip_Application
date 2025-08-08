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


#define SERVICE_SPEEDREQUEST 0x4321  
#define SERVICE_SPEEDVALUE   0x1234  

 
#define INSTANCE_SPEEDREQUEST 0x0001 
#define INSTANCE_SPEEDVALUE   0x0001 


#define METHOD_SPEEDREQUEST 0x0003   
#define METHOD_SPEEDVALUE   0x0002   


#define EVENT_SPEEDALERT    0x0004   

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

# Exécutable client1 (updated)
add_executable(client1 client1.cpp)
target_link_libraries(client1 
    ${VSOMEIP_LIBRARIES}
    Threads::Threads
)
target_compile_definitions(client1 PRIVATE ${VSOMEIP_CFLAGS_OTHER})

# Exécutable client2 (unchanged)
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

# Installation
install(TARGETS server client1 client2 DESTINATION bin)
install(FILES server.json client1.json client2.json DESTINATION etc/vsomeip)

cmake_minimum_required(VERSION 3.10)
project(speed_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Messages de log
message(STATUS "Building ${PROJECT_NAME}")

# Recherche de vsomeip
find_package(vsomeip3 REQUIRED)
find_package(Threads REQUIRED)

# Include headers (si vsomeip_ids.hpp est dans include/)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Sources
set(SOURCES_SERVER src/server.cpp)
set(SOURCES_CLIENT1 src/client1.cpp)
set(SOURCES_CLIENT2 src/client2.cpp)

# Ajout des exécutables
add_executable(server ${SOURCES_SERVER})
add_executable(client1 ${SOURCES_CLIENT1})
add_executable(client2 ${SOURCES_CLIENT2})

# Link vsomeip et threads
foreach(target IN LISTS CMAKE_EXECUTABLES)
    target_link_libraries(${target} PRIVATE vsomeip3::vsomeip3 Threads::Threads)
endforeach()

# Installation optionnelle
install(TARGETS server client1 client2
        RUNTIME DESTINATION bin)

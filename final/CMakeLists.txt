cmake_minimum_required(VERSION 3.10)
project(VSomeIPSpeedApplication)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find vSOME/IP package
find_library(VSOMEIP_LIBRARY NAMES vsomeip3 vsomeip)
find_library(VSOMEIP_SD_LIBRARY NAMES vsomeip3-sd vsomeip-sd)
find_path(VSOMEIP_INCLUDE_DIR vsomeip/vsomeip.hpp)

# Find Boost (required by vSOME/IP)
find_package(Boost 1.55 REQUIRED COMPONENTS system filesystem thread log)

# Include directories
include_directories(
    ${VSOMEIP_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/final
)

# Define source files
set(SOURCES
    final/client1.cpp
    final/client2.cpp
    final/server.cpp
)

# Define executables
add_executable(client1 final/client1.cpp)
add_executable(client2 final/client2.cpp)
add_executable(server final/server.cpp)

# Link libraries for each executable
target_link_libraries(client1
    ${VSOMEIP_LIBRARY}
    ${VSOMEIP_SD_LIBRARY}
    ${Boost_LIBRARIES}
    pthread
)

target_link_libraries(client2
    ${VSOMEIP_LIBRARY}
    ${VSOMEIP_SD_LIBRARY}
    ${Boost_LIBRARIES}
    pthread
)

target_link_libraries(server
    ${VSOMEIP_LIBRARY}
    ${VSOMEIP_SD_LIBRARY}
    ${Boost_LIBRARIES}
    pthread
)

# Set vSOME/IP configuration file paths
set(VSOMEIP_CONFIG_FILES
    "${CMAKE_SOURCE_DIR}/client1.json"
    "${CMAKE_SOURCE_DIR}/client2.json"
    "${CMAKE_SOURCE_DIR}/server.json"
)

# Copy configuration files to build directory
foreach(CONFIG_FILE ${VSOMEIP_CONFIG_FILES})
    configure_file(${CONFIG_FILE} ${CMAKE_BINARY_DIR} COPYONLY)
endforeach()

# Installation rules
install(TARGETS client1 client2 server
    RUNTIME DESTINATION bin
)

install(FILES ${VSOMEIP_CONFIG_FILES}
    DESTINATION etc/vsomeip
)

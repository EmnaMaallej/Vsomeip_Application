cmake_minimum_required(VERSION 3.10)
project(VSomeIPSpeedApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(VSOMEIP REQUIRED vsomeip3)

# Add include directories
include_directories(${VSOMEIP_INCLUDE_DIRS})

# Add library directories
link_directories(${VSOMEIP_LIBRARY_DIRS})

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VSOMEIP_CFLAGS_OTHER}")

# Add executable for Client1
add_executable(client1 client1.cpp)
target_link_libraries(client1 ${VSOMEIP_LIBRARIES} pthread)

# Add executable for Client2  
add_executable(client2 client2.cpp)
target_link_libraries(client2 ${VSOMEIP_LIBRARIES} pthread)

# Add executable for Server
add_executable(server server.cpp)
target_link_libraries(server ${VSOMEIP_LIBRARIES} pthread)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

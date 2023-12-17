cmake_minimum_required(VERSION 3.16.3)

project(messagebus)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -g")

find_package(Threads)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Main executable source files
set(MAIN_SOURCE_FILES main.cpp scheduler.cpp codegen/message.grpc.pb.h codegen/message.grpc.pb.cc codegen/message.pb.cc codegen/message.pb.h)

add_executable(main ${MAIN_SOURCE_FILES})

target_link_libraries(main
        Threads::Threads
        protobuf::libprotobuf
        gRPC::grpc++_unsecure
        gRPC::grpc++_reflection
        ${CMAKE_SHARED_LINKER_FLAGS})
set_target_properties(main PROPERTIES OUTPUT_NAME main)

# Test executable source files (replace main.cpp with test.cpp)
set(TEST_SOURCE_FILES test.cpp scheduler.cpp codegen/message.grpc.pb.h codegen/message.grpc.pb.cc codegen/message.pb.cc codegen/message.pb.h)

# Create the test executable
add_executable(test_executable ${TEST_SOURCE_FILES})

# Link libraries for the test executable
target_link_libraries(test_executable
        Threads::Threads
        protobuf::libprotobuf
        gRPC::grpc++_unsecure
        gRPC::grpc++_reflection
        ${CMAKE_SHARED_LINKER_FLAGS})

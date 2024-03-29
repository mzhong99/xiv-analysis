cmake_minimum_required(VERSION 3.0)
project(XIV_Analysis)

find_package(Threads REQUIRED)
set (CMAKE_CXX_STANDARD 20)

add_subdirectory(external/libpcap)
add_subdirectory(external/pe-parse)

include_directories(external/libpcap)
include_directories(external/cpp-subprocess)
include_directories(external/doctest/doctest)
include_directories(external/spdlog/include)

include_directories(src)
include_directories(src/utils)

set(COMMON_SRC
    # src/utils/MappedMemory.cpp
    src/utils/SoftwareBusSubsystem.cpp
    src/NetworkMonitor.cpp
    # src/Oodle.cpp
)

set(TEST_SRC
    test/DocTestMain.cpp
    test/MessageQueueTest.cpp
    test/SubsystemTest.cpp
    test/SoftwareBusSubsystemTest.cpp
    test/TcpSubsystemTest.cpp
    test/ListTest.cpp
)

add_executable(xiv-analysis src/Main.cpp ${COMMON_SRC})
target_link_libraries(xiv-analysis Threads::Threads pcap)

add_executable(unittests ${TEST_SRC} ${COMMON_SRC})
target_link_libraries(unittests Threads::Threads pcap)

cmake_minimum_required(VERSION 3.12)
project(RtcGateway)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_INSTALL_PATH /opt/third_party/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR})
set(THIRD_PARTY_INDEPENDENT_INSTALL FALSE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(cmake/generic.cmake)

include(cmake/external/gflags.cmake)
include(cmake/external/glog.cmake)
include(cmake/external/boost.cmake)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/concurrentqueue)

aux_source_directory(core/thread CORE_SRCS)

add_executable(RtcGateway
        ${CORE_SRCS}
        main.cpp)

add_dependencies(RtcGateway glog gflags boost_system)

target_link_libraries(RtcGateway glog gflags pthread)
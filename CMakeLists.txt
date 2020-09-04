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
include(cmake/external/yaml.cmake)

# third parth
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/concurrentqueue)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/json/single_include)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)

add_definitions(-DDEBUG_SIGNALING)

aux_source_directory(core CORE_SRCS)
aux_source_directory(core/thread CORE_SRCS)
aux_source_directory(core/gateway CORE_SRCS)
aux_source_directory(core/gateway/messenger CORE_SRCS)
aux_source_directory(core/gateway/signaling CORE_SRCS)
aux_source_directory(core/ice CORE_SRCS)
aux_source_directory(core/ice/wrapper CORE_SRCS)
aux_source_directory(core/utils CORE_SRCS)


# for libnice
include_directories(/usr/include/glib-2.0/)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include/)
set(NICE_LIB nice glib-2.0 gobject-2.0 gio-2.0)

add_executable(RtcGateway
        ${CORE_SRCS}
        main.cpp)

add_dependencies(RtcGateway glog gflags boost_system yaml-cpp)

target_link_libraries(RtcGateway glog gflags ${NICE_LIB} pthread)
cmake_minimum_required(VERSION 3.3)
PROJECT(MYSERVER)

# version number
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

configure_file("${PROJECT_SOURCE_DIR}/projectinfo.h.in" "${PROJECT_SOURCE_DIR}/projectinfo1.h")

# 编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -g -O0 -Wall -Werror -pthread")

# 可执行文件存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# 库（静态库/动态库）存放路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(/usr/local/lib/mysql-connector-c++/1.1.7/include)
include_directories(/usr/local/lib/libev/include)
link_directories(/usr/local/lib/libev/lib)
link_directories(/usr/local/lib/protocolBuffer)
link_directories(/usr/local/lib/mysql-connector-c++/1.1.7/lib64)

include_directories(${PROJECT_SOURCE_DIR})
# subdirectory
add_subdirectory(${PROJECT_SOURCE_DIR}/protocol)
add_subdirectory(${PROJECT_SOURCE_DIR}/network)
add_subdirectory(${PROJECT_SOURCE_DIR}/utils)


# .cpp files
aux_source_directory(. SOURCE_FILES)

# executable file
add_executable(jobServer ${SOURCE_FILES})

# shared library need to link to
target_link_libraries(jobServer uuid
        protobuf
        cares
        ev
        muduo_base_cpp11
        muduo_net_cpp11
        utils
        protocol_yfq
        protocol_test
        mysqlcppconn)
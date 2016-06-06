#cmake_minimum_required(VERSION 3.5)
#
## 编译选项
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -g -O0 -Wall -Werror -pthread")
#
#include_directories(/usr/local/apps/mysql-connector/mysql-connector-c++-1.1.6/include)
#link_directories(/usr/local/apps/mysql-connector/mysql-connector-c++-1.1.6/lib64)
#link_directories(/root/clionWorkspace/YfqServer/build/lib)
#
#aux_source_directory(. DIR_SRCS)
#
#add_executable(mysqlc ${DIR_SRCS})
#
#target_link_libraries(mysqlc pthread rt mysqlcppconn muduo_base_cpp11)

#include_directories(/usr/local/lib/mysql-connector-c++/1.1.7/include)
#add_library(mysqldb DbHandler.cpp)
#target_link_libraries(mysqldb  mysqlcppconn)

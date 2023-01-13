cmake_minimum_required(VERSION 3.5)

project(msgpack)

set(msgpack_INCLUDE_DIR "/home/nixiaowei/3rd_lib/msgpack-c/include")

include_directories(${msgpack_INCLUDE_DIR})




add_executable(msgpack test.cpp)



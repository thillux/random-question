cmake_minimum_required(VERSION 2.8)

IF(UNIX)
  add_definitions(-std=c++11)
ENDIF(UNIX)

add_executable(question question.cc)

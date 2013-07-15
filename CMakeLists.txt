cmake_minimum_required(VERSION 2.8)

IF(UNIX)
  add_definitions(-std=c++11)
ENDIF(UNIX)
IF(APPLE)
  # add_definitions didn't work
  # TODO lookup why
  SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -O3 -march=native")
ENDIF(APPLE)

add_executable(question question.cc)

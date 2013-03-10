cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_FLAGS -std=c++11 -stdlib=libc++ -O3 -march=native -Wall -Wextra)

add_executable(question question.cc)

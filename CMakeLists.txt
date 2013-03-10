set(CMAKE_CXX_COMPILER clang++)
add_definitions(-std=c++11 -O3 -march=native -Wall -Wextra)

add_executable(question question.cc)

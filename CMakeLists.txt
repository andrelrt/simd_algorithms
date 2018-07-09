project(simd_algorithms)
cmake_minimum_required(VERSION 2.8)

find_package(Boost 1.50.0 REQUIRED COMPONENTS timer chrono system filesystem)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=native -mtune=native -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -mavx2 -Wall -fno-strict-aliasing")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -msse2 -Wall -fno-strict-aliasing")

enable_testing()

add_subdirectory(binary_search)
add_subdirectory(bubble_sort)
add_subdirectory(test)


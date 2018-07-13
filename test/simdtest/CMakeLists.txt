project(simdtest)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME}
	${SRC_LIST}
)

target_include_directories(${PROJECT_NAME}
	SYSTEM PUBLIC
    ${Boost_INCLUDE_DIRS}
    ../..
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Boost_LIBRARIES}
    libgtest
)
#gtest_discover_test(simd_compare_test)


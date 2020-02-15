cmake_minimum_required(VERSION 3.10)
project(img_filter LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WERROR "Add -Werror flag to build (turns warnings into errors)" ON)

# configure optimization
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPTIMIZATION_FLAGS "-O0 -DDEBUG")
    message("-- Configuring debug build")
else()
    set(OPTIMIZATION_FLAGS "-O3 -DNDEBUG")
    message("-- Configuring release build")
endif()

# Enable extra warnings to adhere to https://github.com/mapbox/cpp/issues/37
set(DESIRED_WARNINGS "-Wall -Wextra -Wconversion -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast -Wno-error=unused-variable -Wshadow -Wfloat-equal -Weffc++")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(DESIRED_WARNINGS "${DESIRED_WARNINGS} -Wmost")
endif()

# Note: -D_GLIBCXX_USE_CXX11_ABI=0 is needed to support mason packages that are precompiled libs
# Currently we only depend on a header only library, but this will help avoid issues when more libs are added via mason
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 ${DESIRED_WARNINGS}")

if (WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()


# Project Version and File
set(PROJECT_VERSION 0.0.1)
set(IMGFILTER_SRC "${PROJECT_SOURCE_DIR}/src")
set(IMGFILTER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(IMGFILTER_EXAMPLES_CPP_DIR "${PROJECT_SOURCE_DIR}/examples")
set(IMGFILTER_SOURCE_FILES ${IMGFILTER_SRC}/realsense_filters.cpp)

file(GLOB ALL_HEADERS ${IMGFILTER_INCLUDE_DIR}/img_filters/*.hpp)
set(IMGFILTER_HEADER_FILES ${IMGFILTER_INCLUDE_DIR}/img_filters.hpp ALL_HEADERS)

# Create Image Filter Library
set(LIB_TYPE STATIC)
add_library(img_filter ${LIB_TYPE} ${IMGFILTER_SOURCE_FILES})


# Set Header and Properties of Image Filter Library
set_target_properties(img_filter PROPERTIES PUBLIC_HEADER "${IMGFILTER_HEADER_FILES}")
target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE:${IMGFILTER_INCLUDE_DIR}>
$<INSTALL_INTERFACE:include>
PRIVATE src)
set_target_properties(img_filter PROPERTIES VERSION ${PROJECT_VERSION})


# Setup Testing and Benchmarks
add_subdirectory("thirdparty/googletest")
add_subdirectory("thirdparty/benchmark")


# file(GLOB TEST_SOURCES test/*.cpp)
# add_executable(unit-tests ${TEST_SOURCES})

# # libbenchmark.a supports threads and therefore needs pthread support
# find_package(Threads REQUIRED)
# file(GLOB BENCH_SOURCES bench/*.cpp)
# add_executable(bench-tests ${BENCH_SOURCES})

# # link benchmark static library to the bench-tests binary so the bench tests know where to find the benchmark impl code
# target_link_libraries(bench-tests ${MASON_PACKAGE_benchmark_STATIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})

cmake_minimum_required(VERSION 3.10)
set(PROJECT_VERSION 0.0.1)
project(img_filter VERSION ${PROJECT_VERSION}
        DESCRIPTION "Simple Image Filter Library"
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WERROR "Add -Werror flag to build (turns warnings into errors)" ON)
#### Configuration
### Configure Optimization
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPTIMIZATION_FLAGS "-O0 -DDEBUG")
    message("-- Configuring debug build")
else()
    set(OPTIMIZATION_FLAGS "-O3 -DNDEBUG")
    message("-- Configuring release build")
endif()

### Enable extra warnings to adhere to https://github.com/mapbox/cpp/issues/37
set(DESIRED_WARNINGS "-Wall -Wextra -Wconversion -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast -Wno-error=unused-variable -Wshadow -Wfloat-equal")
set(MINIMAL_DESIRED_WARNINGS "-Wall -Wextra -pedantic-errors")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(DESIRED_WARNINGS "${DESIRED_WARNINGS} -Wmost")
endif()
separate_arguments(DESIRED_WARNINGS)
separate_arguments(MINIMAL_DESIRED_WARNINGS)

### Set GLOBAL CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS}")
if (WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()


#### Project Dependencies
find_package(OpenMP)
### Google Testing and Benchmarks Libraries
add_subdirectory("thirdparty/googletest")
add_subdirectory("thirdparty/benchmark")


#### Project Files
set(IMGFILTER_SRC "${PROJECT_SOURCE_DIR}/src")
set(IMGFILTER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(IMGFILTER_EXAMPLES_CPP_DIR "${PROJECT_SOURCE_DIR}/examples")
set(IMGFILTER_SOURCE_FILES ${IMGFILTER_SRC}/realsense_filters.cpp)

file(GLOB ALL_HEADERS ${IMGFILTER_INCLUDE_DIR}/img_filters/*.hpp)
set(IMGFILTER_HEADER_FILES ${IMGFILTER_INCLUDE_DIR}/img_filters.hpp ALL_HEADERS)

### Create Image Filter Library
set(LIB_TYPE STATIC)
add_library(img_filter ${LIB_TYPE} ${IMGFILTER_SOURCE_FILES})

### Enable warnings for ONLY this target. Error on them
target_compile_options(img_filter PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${DESIRED_WARNINGS}>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)


#### Set Header and Properties of Image Filter Library
set_target_properties(img_filter PROPERTIES PUBLIC_HEADER "${IMGFILTER_HEADER_FILES}")
target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE:${IMGFILTER_INCLUDE_DIR}>
$<INSTALL_INTERFACE:include>
PRIVATE src)
set_target_properties(img_filter PROPERTIES VERSION ${PROJECT_VERSION})


#### Benchmark Executables
set(IMGFILTER_BENCH_SRC "${PROJECT_SOURCE_DIR}/bench")
add_executable(bench-run ${IMGFILTER_BENCH_SRC}/bench_run.cpp)
target_link_libraries(bench-run benchmark::benchmark OpenMP::OpenMP_CXX img_filter)

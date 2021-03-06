cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
project(atlashdf)

execute_process(COMMAND git submodule update --init --recursive)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS " -fopenmp -march=native -std=c++17 -fPIC -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5_cpp")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

# Set source directory

# Tell CMake that headers are also in SOURCE_DIR
include_directories(${SOURCE_DIR})

set(PYBIND_SOURCES
    src/pybind.cpp

    lib/shapelib/shpopen.c
    lib/shapelib/dbfopen.c
    lib/shapelib/safileio.c
    lib/shapelib/shptree.c
    lib/shapelib/sbnsearch.c

)

# Generate Python module
add_subdirectory(lib/pybind11)
pybind11_add_module(atlashdf ${PYBIND_SOURCES})
set(HIGHFIVE_USE_BOOST OFF)
set(HIGHFIVE_EXAMPLES OFF)
set(HIGHFIVE_BUILD_DOCS OFF)
set(HIGHFIVE_PARALLEL_HDF5 OFF)

add_subdirectory(lib/HighFive/)
#add_subdirectory(lib/shapelib/)

set(SOURCES_TEST_DATASET
    tests/test_dataset.cpp
)

include_directories(
  src
   lib/HighFive/include/highfive
   lib/HighFive/include
   lib/tqdm-cpp
   lib/boost-headers-only
   lib/nlohmann_json
   lib/shapelib
   )


target_link_libraries(atlashdf
PUBLIC HighFive
)
add_executable(test_dataset ${SOURCES_TEST_DATASET})

find_package(OpenMP)
if(OpenMP_CXX_FOUND)

target_link_libraries(test_dataset
PUBLIC HighFive
PUBLIC OpenMP::OpenMP_CXX)


endif()



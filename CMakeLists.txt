cmake_minimum_required(VERSION 3.12)
project(mmtf)

find_package(Boost 1.71 REQUIRED)
find_package(Catch2 2.9 REQUIRED)
find_package(fmt 6.1 REQUIRED)

add_library(janucaria-mmtf INTERFACE)

add_library(janucaria::mmtf ALIAS janucaria-mmtf)

target_include_directories(janucaria-mmtf
  INTERFACE
    src
)

target_compile_options(janucaria-mmtf
  INTERFACE
    -Wall
    -Wpedantic
    -pedantic-errors
)

target_compile_features(janucaria-mmtf
  INTERFACE
    cxx_std_17
)

target_link_libraries(janucaria-mmtf
  INTERFACE
    Boost::headers
    fmt::fmt-header-only
)

add_subdirectory(test)

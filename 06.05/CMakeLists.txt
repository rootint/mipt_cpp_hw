cmake_minimum_required(VERSION 3.16)

################################################################################

project(06.05 VERSION 1.0 LANGUAGES CXX)

################################################################################

set(CMAKE_CXX_STANDARD          23 )
set(CMAKE_CXX_STANDARD_REQUIRED ON )

################################################################################

set(Boost_NO_BOOST_CMAKE ON)

find_package(Boost REQUIRED COMPONENTS filesystem system)

################################################################################

add_library(shared_v1 SHARED source/shared_v1.cpp)
add_library(shared_v2 SHARED source/shared_v2.cpp)

################################################################################

target_link_libraries(shared_v1 PRIVATE Boost::filesystem)
target_link_libraries(shared_v2 PRIVATE Boost::filesystem)

################################################################################

add_executable(main source/main.cpp)

################################################################################

target_link_libraries(main PRIVATE Boost::filesystem ${CMAKE_DL_LIBS})

################################################################################

set_target_properties(shared_v1 shared_v2 main PROPERTIES

    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

################################################################################
cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(libchemist VERSION "${VERSION}" LANGUAGES CXX)

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("${CMAKE_CURRENT_SOURCE_DIR}/libchemist")

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

cpp_find_or_build_dependency(
    sde
    URL github.com/NWChemEx-Project/sde
    PRIVATE TRUE
    BUILD_TARGET sde
    FIND_TARGET nwx::sde
    CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_find_or_build_dependency(
        tiledarray
        URL github.com/NWChemEx-Project/tiledarray
        VERSION developer
        BUILD_TARGET tiledarray
        FIND_TARGET tiledarray
)

cpp_add_library(
    chemist
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/libchemist"
    INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/libchemist"
    DEPENDS nwx::sde tiledarray
)

if("${BUILD_TESTING}")
    cpp_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        VERSION v2.x
        FIND_TARGET Catch2::Catch2
        VERSION v2.x
    )
    cpp_add_tests(
        test_chemist
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/libchemist"
        DEPENDS Catch2::Catch2 chemist
    )
endif()



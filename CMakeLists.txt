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

# N.B. If you update the TA version, you may need to update the MADNESS version
#      in the runtime repo.
cpp_find_or_build_dependency(
    tiledarray
    URL github.com/ValeevGroup/tiledarray
    VERSION a7b866739a01f797afa39c68a1f4263152f9d550
    BUILD_TARGET tiledarray
    FIND_TARGET tiledarray
    CMAKE_ARGS TA_BUILD_UNITTEST=OFF
               BLAS_THREAD_LAYER=sequential
               MADNESS_ENABLE_CEREAL=ON
)

cpp_find_or_build_dependency(
    sde
    URL github.com/NWChemEx-Project/sde
    PRIVATE TRUE
    BUILD_TARGET sde
    FIND_TARGET nwx::sde
    CMAKE_ARGS BUILD_TESTING=OFF
)

find_package(Boost REQUIRED COMPONENTS container)

cpp_add_library(
    chemist
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/libchemist"
    INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/libchemist"
    DEPENDS nwx::sde tiledarray Boost::container
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
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/libchemist"
        DEPENDS Catch2::Catch2 chemist
    )
endif()

cmake_minimum_required(VERSION 3.14)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(chemist VERSION "${VERSION}" LANGUAGES CXX)

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("${CMAKE_CURRENT_SOURCE_DIR}/src" 
                 "${CMAKE_CURRENT_SOURCE_DIR}/include")

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

cpp_find_or_build_dependency(
    tensorwrapper
    URL github.com/NWChemEx-Project/TensorWrapper
    PRIVATE TRUE
    BUILD_TARGET tensorwrapper
    FIND_TARGET nwx::tensorwrapper
    CMAKE_ARGS BUILD_TESTING=OFF
)

find_package(Boost REQUIRED COMPONENTS container)

cpp_add_library(
    chemist
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/chemist"
    INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/chemist"
    DEPENDS nwx::tensorwrapper Boost::container
)

include(nwx_python_mods)
cppyy_make_python_package(PYTHONIZE MPI PACKAGE chemist NAMESPACES chemist TA DEPPACKAGES tensorwrapper)

if("${BUILD_TESTING}")
    cpp_find_or_build_dependency(
        pluginplay
        URL github.com/NWChemEx-Project/PluginPlay
        PRIVATE TRUE
        BUILD_TARGET pluginplay
        FIND_TARGET nwx::pluginplay
        CMAKE_ARGS BUILD_TESTING=OFF
    )
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
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/chemist"
        DEPENDS nwx::pluginplay Catch2::Catch2 chemist
    )
endif()

# Copyright 2022 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.14)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(chemist VERSION "${VERSION}" LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    nwx_cmake
    GIT_REPOSITORY https://github.com/NWChemEx-Project/NWXCMake
)
FetchContent_MakeAvailable(nwx_cmake)
list(APPEND CMAKE_MODULE_PATH "${nwx_cmake_SOURCE_DIR}/cmake")

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cmaize)
include(nwx_versions)
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("${CMAKE_CURRENT_SOURCE_DIR}/src"
                 "${CMAKE_CURRENT_SOURCE_DIR}/include")

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBIND11_PYBINDINGS "Build Python bdinings with pybind11?" OFF)
option(BUILD_CPPYY_PYBINDINGS "Build Python bindings with Cppyy?" OFF)

cmaize_find_or_build_dependency(
    tensorwrapper
    URL github.com/NWChemEx-Project/TensorWrapper
    PRIVATE TRUE
    VERSION ${NWX_TENSORWRAPPER_VERSION}
    BUILD_TARGET tensorwrapper
    FIND_TARGET nwx::tensorwrapper
    CMAKE_ARGS BUILD_TESTING=OFF
#               BUILD_PYBIND11_PYBINDINGS=${BUILD_PYBIND11_PYBINDINGS}
)

find_package(Boost REQUIRED)

cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/chemist"
    INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include/chemist"
    DEPENDS tensorwrapper Boost::boost
)

# N.B. this is a no-op if BUILD_PYBIND11_PYBINDINGS is not turned on
include(nwx_pybind11)
nwx_add_pybind11_module(
    ${PROJECT_NAME}
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/python"
    DEPENDS "${PROJECT_NAME}"
)

if("${BUILD_CPPYY_PYBINDINGS}")
    include(nwx_python_mods)
    cppyy_make_python_package(PYTHONIZE MPI PACKAGE chemist NAMESPACES chemist TA DEPPACKAGES tensorwrapper)
endif()

if("${BUILD_TESTING}")
    set(CXX_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/cxx")
    set(PYTHON_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/python")
    set(EXAMPLES_SRC_DIR "${CXX_TEST_DIR}/doc_snippets")

    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
        VERSION ${NWX_CATCH2_VERSION}
    )
    cmaize_add_library(
        ${PROJECT_NAME}_examples
        SOURCE_DIR ${EXAMPLES_SRC_DIR}
        INCLUDE_DIRS ${EXAMPLES_SRC_DIR}
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )
    cmaize_add_tests(
        test_unit_chemist
        SOURCE_DIR "${CXX_TEST_DIR}/unit_tests"
        INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/chemist"
        DEPENDS Catch2 chemist
    )
    cmaize_add_tests(
        test_integration_chemist
        SOURCE_DIR "${CXX_TEST_DIR}/integration_tests"
        INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/chemist"
        DEPENDS Catch2 chemist
    )
    cmaize_add_tests(
        test_${PROJECT_NAME}_docs
        SOURCE_DIR ${EXAMPLES_SRC_DIR}
        INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )
    # N.B. these are no-ops if BUILD_PYBIND11_PYBINDINGS is not turned on
    nwx_pybind11_tests(
        py_chemist "${PYTHON_TEST_DIR}/unit_tests/test_chemist.py"
        SUBMODULES parallelzone
    )
    nwx_pybind11_tests(
        py_${PROJECT_NAME}_docs "${PYTHON_TEST_DIR}/doc_snippets/test_doc_snippets.py"
        SUBMODULES parallelzone
    )
endif()


cmaize_add_package(${PROJECT_NAME} NAMESPACE nwx::)

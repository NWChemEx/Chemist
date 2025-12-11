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

# Downloads common CMake modules used throughout NWChemEx
include(cmake/get_nwx_cmake.cmake)

#Sets the version to whatever git thinks it is
#include(get_version_from_git)
#get_version_from_git(pluginplay_version "${CMAKE_CURRENT_LIST_DIR}")
set(chemist_version 1.0.0)
project(chemist VERSION "${chemist_version}" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(get_cmaize)
include(nwx_cxx_api_docs)

### Paths ###
set(CHEMIST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CHEMIST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(CHEMIST_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(CHEMIST_EXP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/experimental")


nwx_cxx_api_docs("${CHEMIST_SOURCE_DIR}" "${CHEMIST_INCLUDE_DIR}")

### Options ###
cmaize_option_List(
    BUILD_TESTING OFF "Should we build the tests?"
    BUILD_PYBIND11_PYBINDINGS ON "Build Python bindings with pybind11?"
    ENABLE_SIGMA OFF "Should we enable Sigma for uncertainty tracking?"
)

### Dependencies ###
include(get_utilities)

include(get_parallelzone)

include(get_tensorwrapper)

find_package(Boost REQUIRED)

cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${CHEMIST_SOURCE_DIR}/chemist"
    INCLUDE_DIRS "${CHEMIST_INCLUDE_DIR}/chemist"
    DEPENDS tensorwrapper parallelzone utilities Boost::boost
)

# N.B. this is a no-op if BUILD_PYBIND11_PYBINDINGS is not turned on
include(nwx_pybind11)
nwx_add_pybind11_module(
    ${PROJECT_NAME}
    SOURCE_DIR "${CHEMIST_SOURCE_DIR}/python"
    DEPENDS "${PROJECT_NAME}"
)

if("${BUILD_TESTING}")
    set(CXX_TEST_DIR "${CHEMIST_TESTS_DIR}/cxx")
    set(PYTHON_TEST_DIR "${CHEMIST_TESTS_DIR}/python")
    set(EXAMPLES_SRC_DIR "${CXX_TEST_DIR}/doc_snippets")

    include(get_catch2)

    cmaize_add_library(
        ${PROJECT_NAME}_examples
        SOURCE_DIR ${EXAMPLES_SRC_DIR}
        INCLUDE_DIRS ${EXAMPLES_SRC_DIR}
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    cmaize_add_tests(
        test_unit_chemist
        SOURCE_DIR "${CXX_TEST_DIR}/unit_tests"
        INCLUDE_DIRS "${CHEMIST_SOURCE_DIR}/chemist"
        DEPENDS Catch2 chemist
    )

    cmaize_add_tests(
        test_${PROJECT_NAME}_docs
        SOURCE_DIR ${EXAMPLES_SRC_DIR}
        INCLUDE_DIRS ${CHEMIST_SOURCE_DIR}/${PROJECT_NAME}
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    # N.B. these are no-ops if BUILD_PYBIND11_PYBINDINGS is not turned on
    nwx_pybind11_tests(
        py_chemist
        "${PYTHON_TEST_DIR}/unit_tests/test_chemist.py"
        SUBMODULES parallelzone
    )

    nwx_pybind11_tests(
        py_${PROJECT_NAME}_docs
        "${PYTHON_TEST_DIR}/doc_snippets/test_doc_snippets.py"
        SUBMODULES parallelzone
    )
endif()


cmaize_add_package(
    ${PROJECT_NAME} NAMESPACE nwx::
)

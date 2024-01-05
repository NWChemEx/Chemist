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

include(nwx_versions)
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
    BUILD_PYBIND11_PYBINDINGS OFF "Build Python bdinings with pybind11?"
    ENABLE_EXPERIMENTAL_FEATURES OFF "Build features which are not 1.0-ed yet?"
)

### Dependencies ###
cmaize_find_or_build_dependency(
    pluginplay
    URL github.com/NWChemEx/PluginPlay
    BUILD_TARGET pluginplay
    FIND_TARGET nwx::pluginplay
    CMAKE_ARGS BUILD_TESTING=OFF
)

# TODO: uncomment when higher version of CMaize is used by NWX
# cmaize_find_or_build_optional_dependency(
#     tensorwrapper
#     ENABLE_EXPERIMENTAL_FEATURES
#     URL github.com/NWChemEx/TensorWrapper
#     PRIVATE TRUE
#     VERSION ${NWX_TENSORWRAPPER_VERSION}
#     BUILD_TARGET tensorwrapper
#     FIND_TARGET nwx::tensorwrapper
#     CMAKE_ARGS BUILD_TESTING=OFF
# )


find_package(Boost REQUIRED)

cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${CHEMIST_SOURCE_DIR}/chemist"
    INCLUDE_DIRS "${CHEMIST_INCLUDE_DIR}/chemist"
    DEPENDS pluginplay Boost::boost
)

if("${ENABLE_EXPERIMENTAL_FEATURES}")
    cmaize_find_or_build_dependency(
        tensorwrapper
        URL github.com/NWChemEx/TensorWrapper
        PRIVATE TRUE
        VERSION ${NWX_TENSORWRAPPER_VERSION}
        BUILD_TARGET tensorwrapper
        FIND_TARGET nwx::tensorwrapper
        CMAKE_ARGS BUILD_TESTING=OFF
    )

    cmaize_add_library(
        experimental_${PROJECT_NAME}
        SOURCE_DIR "${CHEMIST_EXP_DIR}/src/chemist"
        INCLUDE_DIRS "${CHEMIST_EXP_DIR}/include/chemist"
        DEPENDS ${PROJECT_NAME} tensorwrapper
    )

else()
    add_library(experimental_${PROJECT_NAME} INTERFACE)
endif()

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

    if("${ENABLE_EXPERIMENTAL_FEATURES}")
        cmaize_add_tests(
            test_unit_experimental_${PROJECT_NAME}
            SOURCE_DIR "${CHEMIST_EXP_DIR}/tests/cxx/unit_tests"
            INCLUDE_DIRS "${CHEMIST_EXP_DIR}/src/chemist"
            DEPENDS Catch2::Catch2 experimental_${PROJECT_NAME}
        )

        nwx_pybind11_tests(
            py_experimental_${PROJECT_NAME}
            "${CHEMIST_EXP_DIR}/tests/python/unit_tests/test_python.py"
            SUBMODULES parallelzone
        )

    endif()

endif()


cmaize_add_package(
    ${PROJECT_NAME} experimental_${PROJECT_NAME} NAMESPACE nwx::
)

################################################################################
#
# This is the effective top-level CMakeLists.txt file for the current NWChemEx
# library.  You shouldn't have to modify it.
#
################################################################################

#We require C++14 get it out of the way early
set(CMAKE_CXX_STANDARD 14)


############################
#
# Alias some paths for sanity
#
############################

#The root of the NWChemExBase build system
set(NWXBASE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#The directory that called us
get_filename_component(SUPER_PROJECT_ROOT ${NWXBASE_ROOT} DIRECTORY)

#The NWChemExBase cmake folder
set(NWXBASE_CMAKE ${NWXBASE_ROOT}/cmake)

#The location of the macros
set(NWXBASE_MACROS ${NWXBASE_CMAKE}/macros)

#The location of our find scripts
set(NWXBASE_FIND_SCRIPTS ${NWXBASE_CMAKE}/find_external)

#The location of our build scripts
set(NWXBASE_BUILD_SCRIPTS ${NWXBASE_CMAKE}/build_external)

#Effective root of the file system during the build
set(STAGE_DIR ${CMAKE_BINARY_DIR}/stage)

#During the build, path where we install things
set(STAGE_INSTALL_DIR ${STAGE_DIR}${CMAKE_INSTALL_PREFIX})

#Where we install the tests (different to avoid installing them)
set(TEST_STAGE_DIR ${CMAKE_BINARY_DIR}/test_stage)

#Source and include our macros
list(APPEND CMAKE_MODULE_PATH ${NWXBASE_MACROS} ${NWXBASE_FIND_SCRIPTS}
                              ${NWXBASE_BUILD_SCRIPTS})

list(APPEND CMAKE_PREFIX_PATH "${STAGE_INSTALL_DIR}")
list(APPEND CMAKE_MODULE_PATH "${STAGE_INSTALL_DIR}")

include(OptionMacros)
include(Flags)
include(DependencyMacros)

################################################################################
#
# Read some built-in CMake variables, report their values.
#
################################################################################

option_w_default(CMAKE_BUILD_TYPE Release)
option_w_default(BUILD_SHARED_LIBS TRUE)

#Make a list of all CMake variables that should be passed to all dependencies
bundle_cmake_args(CORE_CMAKE_OPTIONS CMAKE_CXX_COMPILER CMAKE_C_COMPILER
    CMAKE_BUILD_TYPE BUILD_SHARED_LIBS CMAKE_INSTALL_PREFIX CMAKE_CXX_STANDARD
    CMAKE_VERSION)

bundle_cmake_list(CORE_CMAKE_LISTS CMAKE_PREFIX_PATH CMAKE_INSTALL_RPATH
    CMAKE_MODULE_PATH)

bundle_cmake_strings(CORE_CMAKE_STRINGS CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG)

################################################################################
#
# Handle NWChemEx's special options, report their values.
#
################################################################################
option_w_default(BUILD_TESTS TRUE)
#Directory where your source is
option_w_default(NWX_SRC_DIR ${SUPER_PROJECT_ROOT}/${PROJECT_NAME})
#Directory where your tests are
option_w_default(NWX_TEST_DIR ${SUPER_PROJECT_ROOT}/${PROJECT_NAME}_Test)
#Name of variable containing your project's dependencies
option_w_default(NWX_DEPENDENCIES "${${PROJECT_NAME}_DEPENDS}")
#Do you want verbose printing on the CMake configuration?
option_w_default(NWX_DEBUG_CMAKE TRUE)

################################################################################
#
# Build the main library
#
################################################################################
include(ExternalProject)
ExternalProject_Add(${PROJECT_NAME}_External
        SOURCE_DIR ${NWX_SRC_DIR}
        CMAKE_ARGS -DSUPER_PROJECT_ROOT=${SUPER_PROJECT_ROOT}
                   -DNWX_DEBUG_CMAKE=${NWX_DEBUG_CMAKE}
                   ${CORE_CMAKE_OPTIONS}
        BUILD_ALWAYS 1
        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${STAGE_DIR}
        CMAKE_CACHE_ARGS ${CORE_CMAKE_LISTS}
                         ${CORE_CMAKE_STRINGS}
        -DNWX_DEPENDENCIES:STRING=${NWX_DEPENDENCIES}
)

################################################################################
#
# Find the dependencies of the project, queue them for building if not found   #
#
################################################################################

foreach(depend NWChemExBase ${NWX_DEPENDENCIES})
    find_or_build_dependency(${depend} was_found)
    if(was_found)
        list(APPEND DEPENDS_WE_FOUND ${depend})
    else()
        list(APPEND DEPENDS_WERE_BUILDING ${depend})
    endif()
    add_dependencies(${PROJECT_NAME}_External ${depend}_External)
endforeach()

################################################################################
#                                                                              #
# Build the main library's tests if the user requested them to be built.  Will #
# use the staged ProjectConfig.cmake file.                                     #
#                                                                              #
################################################################################
if(${BUILD_TESTS})
    ExternalProject_Add(${PROJECT_NAME}_Tests_External
            SOURCE_DIR ${NWX_TEST_DIR}
            CMAKE_ARGS -DSUPER_PROJECT_ROOT=${SUPER_PROJECT_TEST}
                       -DNWX_DEBUG_CMAKE=${NWX_DEBUG_CMAKE}
                       ${CORE_CMAKE_OPTIONS}
                       ${CORE_CMAKE_STRINGS}
            BUILD_ALWAYS 1
            INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${TEST_STAGE_DIR}
            CMAKE_CACHE_ARGS ${CORE_CMAKE_LISTS}
    )
    add_dependencies(${PROJECT_NAME}_Tests_External ${PROJECT_NAME}_External)

    # This file will allow us to run ctest in the top-level build dir
    # Basically it just defers to the actual top-level CTestTestfile.cmake in the
    # build directory for this project
    file(WRITE ${CMAKE_BINARY_DIR}/CTestTestfile.cmake
               "subdirs(test_stage${CMAKE_INSTALL_PREFIX}/tests)")
endif()

# Install the staging directory
install(DIRECTORY ${STAGE_INSTALL_DIR}/
        DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)

################################################################################
#
# Let the user know all the settings we worked out
#
################################################################################
message(STATUS
        "----------------------------------------------------------------------"
        "--------")
message(STATUS "Summary of ${PROJECT_NAME} Configuration Settings:")
message(STATUS
        "----------------------------------------------------------------------"
        "--------")
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Found the following dependencies: ${DEPENDS_WE_FOUND}")
message(STATUS "Will build: ${DEPENDS_WERE_BUILDING}")

#####################
# uninstall target
#####################
#configure_file(
#"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
#"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
#IMMEDIATE @ONLY)
#
#add_custom_target(uninstall
#  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

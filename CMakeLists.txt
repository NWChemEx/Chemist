cmake_minimum_required(VERSION 3.6)
include(cmake/Macros.cmake)
start_hunter() #Must be before project command

project(LibChemist VERSION 0.0.0 LANGUAGES CXX)

### Options ###
option(BUILD_TESTS "Should we build the tests?" ON)

### Dependencies ####
hunter_add_package(Utilities)
find_package(Utilities CONFIG)

### Target ###
add_subdirectory(LibChemist)

### Testing ###
if(${BUILD_TESTS})
    enable_testing()
    add_subdirectory(LibChemist_Test)
endif()

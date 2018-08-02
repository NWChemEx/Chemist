cmake_minimum_required(VERSION 3.8)
file(
        DOWNLOAD
        https://github.com/NWChemEx-Project/DeveloperTools/raw/master/cmake.tar.gz
        ${CMAKE_BINARY_DIR}/cmake.tar.gz)
execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/cmake.tar.gz
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
include(${CMAKE_BINARY_DIR}/cmake/Macros.cmake)
start_hunter(LOCAL) #Must be before project command

project(LibChemist VERSION 0.0.0 LANGUAGES CXX)

### Options ###
option(BUILD_TESTS "Should we build the tests?" ON)
### Dependencies ####
foreach(depend Utilities bphash cereal pybind11 SDE TAMM)
    hunter_add_package(${depend})
    find_package(Utilities CONFIG REQUIRED)
endforeach()


### Target ###
add_subdirectory(LibChemist)

### Testing ###
if(${BUILD_TESTS})
    enable_testing()
    add_subdirectory(LibChemist_Test)
endif()

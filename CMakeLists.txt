cmake_minimum_required(VERSION 3.8)
project(libchemist VERSION 1.0.0)
set(CPP_NAMESPACE LibChemist)
find_package(cpp REQUIRED)
cpp_option(BUILD_TESTS ON)
cpp_find_or_build_dependency(Catch2 URL github.com/catchorg/Catch2)
cpp_find_or_build_dependency(sde URL github.com/NWChemEx-Project/SDE PRIVATE
                             CMAKE_ARGS BUILD_TESTS=OFF)
#Not on CPP yet
find_package(tamm CONFIG REQUIRED)

add_subdirectory(LibChemist)

### Testing ###
if(${BUILD_TESTS})
    enable_testing()
    add_subdirectory(LibChemist_Test)
endif()

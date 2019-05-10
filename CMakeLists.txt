cmake_minimum_required(VERSION 3.8)
project(libchemist VERSION 1.0.0)
set(CPP_NAMESPACE libchemist)
find_package(cpp REQUIRED)
cpp_option(BUILD_TESTS ON)

cpp_find_or_build_dependency(
    NAME CMakeBuild
    URL github.com/NWChemEx-Project/CMakeBuild
    PRIVATE
	CMAKE_ARGS BUILD_TESTS=OFF
)

cpp_find_or_build_dependency(
    NAME tamm
    URL github.com/NWChemEx-Project/TAMM
    PRIVATE
    DEPENDS CMakeBuild
    CMAKE_ARGS BUILD_TESTS=OFF
)

cpp_find_or_build_dependency(
    NAME utilities
    URL github.com/ryanmrichard/utilities
    BRANCH iterator_fix
    PRIVATE
    CMAKE_ARGS BUILD_TESTS=OFF
)

cpp_find_or_build_dependency(
    NAME sde
    URL github.com/NWChemEx-Project/sde
    PRIVATE
    CMAKE_ARGS BUILD_TESTS=OFF
)

add_subdirectory(libchemist)

### Testing ###
if(BUILD_TESTS)
    cpp_find_or_build_dependency(NAME Catch2 URL github.com/catchorg/Catch2)
    enable_testing()
    add_subdirectory(tests)
endif()

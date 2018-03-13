cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(LibChemist-Test VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)

foreach(name TestAtom TestBasisShell TestBasisSet  TestMolecule)
    add_cxx_unit_test(${name})
endforeach()

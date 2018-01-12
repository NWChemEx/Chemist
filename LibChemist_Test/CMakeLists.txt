cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(LibChemist-Test VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)

foreach(name TestAtom TestAtomicInfo TestBasisSet TestBasisShell
             TestBasisSetParser TestSetOfAtoms TestSetOfAtomsParser)
    add_cxx_unit_test(${name})
endforeach()

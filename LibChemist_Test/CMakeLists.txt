cmake_minimum_required(VERSION 3.1)
project(LibChemist-Test VERSION 0.0.0 LANGUAGES CXX)
find_package(LibChemist REQUIRED)
include(TargetMacros)

foreach(name TestAtom TestAtomicInfo TestBasisSet TestBasisShell
             TestBasisSetParser TestSetOfAtoms TestSetOfAtomsParser)
    add_cxx_unit_test(${name} LibChemist)
endforeach()

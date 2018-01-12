cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(LibChemist-SRC VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)
set(LibChemist_SRCS lut/AtomicInfo.cpp
                    Atom.cpp
                    BasisSet.cpp
                    BasisSetParser.cpp
                    BasisShell.cpp
                    SetOfAtoms.cpp
                    SetOfAtomsParser.cpp
                    ShellTypes.cpp
)
set(LibChemist_INCLUDES lut/AtomicInfo.hpp
                        Atom.hpp
                        BasisSet.hpp
                        BasisSetParser.hpp
                        BasisShell.hpp
                        SetOfAtoms.hpp
                        SetOfAtomsParser.hpp
                        ShellTypes.hpp
                        Utilities.hpp
)
set(LibChemist_DEFINITIONS "")
set(LibChemist_LINKFLAGS "")
nwchemex_add_library(LibChemist LibChemist_SRCS
                                 LibChemist_INCLUDES
                                 LibChemist_DEFINTIONS
                                 LibChemist_LINKFLAGS
)

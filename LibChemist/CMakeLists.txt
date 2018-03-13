cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(LibChemist-SRC VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)
set(LibChemist_SRCS Atom.cpp
                    BasisSet.cpp
                    BasisShell.cpp
                    Molecule.cpp
                    ShellTypes.cpp
)
set(LibChemist_INCLUDES Atom.hpp
                        BasisSet.hpp
                        BasisShell.hpp
                        Molecule.hpp
                        ShellTypes.hpp
)
set(LibChemist_DEFINITIONS "")
set(LibChemist_LINKFLAGS "")
nwchemex_add_library(LibChemist LibChemist_SRCS
                                 LibChemist_INCLUDES
                                 LibChemist_DEFINTIONS
                                 LibChemist_LINKFLAGS
)

set(
	LIBCHEMIST_HEADERS AOBasisSet.hpp
	                   AOShell.hpp
	 				   Atom.hpp
                       BasisSetManager.hpp
                       LibChemist.hpp
                       Molecule.hpp
	                   MoleculeStreamParser.hpp
                       MoleculeManager.hpp
                       PeriodicTable.hpp
                       Implementations/AoSFacade.hpp
                       Defaults/PropertyTypes.hpp
)
include(Defaults/bases/AddBasis.cmake)
set(
    LIBCHEMIST_SRCS AOBasisSet.cpp
                    AOShell.cpp
                    Atom.cpp
                    BasisSetManager.cpp
                    Molecule.cpp
		            MoleculeStreamParser.cpp
                    MoleculeManager.cpp
                    PeriodicTable.cpp
                    Defaults/NWXBasisSetManagerPIMPL.cpp
                    Defaults/NWXPeriodicTablePIMPL.cpp
                    Defaults/NWXMoleculeManagerPIMPL.cpp
                    ${LIBCHEMIST_BASIS_SOURCE}
)

cpp_add_library(
	chemist
    SOURCES ${LIBCHEMIST_SRCS}
	INCLUDES ${LIBCHEMIST_HEADERS}
    DEPENDS sde::sde tamm
)
cpp_install(TARGETS chemist)

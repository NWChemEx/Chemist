set(
	LIBCHEMIST_HEADERS AOBasisSet.hpp
	                   AOShell.hpp
	 		   Atom.hpp
			   BasisSetManager.hpp
			   BasisSetMap.hpp
                           LibChemist.hpp
                           Molecule.hpp
      		           MoleculeStreamParser.hpp
                           MoleculeManager.hpp
                           OrbitalSpace.hpp
                           PeriodicTable.hpp
                           Implementations/AoSFacade.hpp
			   Defaults/NWXBasisList.hpp
		           Defaults/NWXAtomicDensities.hpp
)

include(Defaults/bases/AddBasis.cmake)

include(Defaults/atomic_densities/AddDensity.cmake)

set(
    LIBCHEMIST_SRCS AOBasisSet.cpp
                    AOShell.cpp
                    Atom.cpp
                    BasisSetManager.cpp
		    BasisSetMap.cpp
                    Molecule.cpp
		    MoleculeStreamParser.cpp
                    MoleculeManager.cpp
                    PeriodicTable.cpp
                    Defaults/NWXBasisSetManagerPIMPL.cpp
                    Defaults/NWXPeriodicTablePIMPL.cpp
                    Defaults/NWXMoleculeManagerPIMPL.cpp
		    Defaults/NWXAtomicDensities.cpp
                    ${LIBCHEMIST_BASIS_SOURCE}
		    ${LIBCHEMIST_DENSITY_SOURCE}
)

cpp_add_library(
		chemist
		SOURCES ${LIBCHEMIST_SRCS}
		INCLUDES ${LIBCHEMIST_HEADERS}
		DEPENDS sde::sde CMakeBuild tamm
)

cpp_install(TARGETS chemist)

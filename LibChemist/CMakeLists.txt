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

add_nwx_library(
        NAME chemist
        SOURCES ${LIBCHEMIST_SRCS}
        DEPENDS Utilities::utilities SDE::SDE ${TAMM_LIBRARY}
)
install_targets(
        TARGETS chemist
        INCLUDES ${LIBCHEMIST_HEADERS}
)

set(
    LIBCHEMIST_HEADERS AOBasisSet.hpp
                       AOShell.hpp
                       Atom.hpp
                       LibChemist.hpp
                       Molecule.hpp
                       MoleculeManager.hpp
                       PeriodicTable.hpp
                       Implementations/AoSFacade.hpp
)
include(Defaults/bases/AddBasis.cmake)
set(
    LIBCHEMIST_SRCS AOBasisSet.cpp
                    AOShell.cpp
                    Atom.cpp
                    BasisSetManager.cpp
                    Molecule.cpp
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
        DEPENDS Utilities::utilities
)
install_targets(
        TARGETS chemist
        INCLUDES ${LIBCHEMIST_HEADERS}
)

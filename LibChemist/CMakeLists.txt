set(
    LIBCHEMIST_HEADERS AOBasisSet.hpp AOShell.hpp Atom.hpp Molecule.hpp
        PeriodicTable.hpp Implementations/AoSFacade.hpp
)
set(
    LIBCHEMIST_SRCS AOBasisSet.cpp AOShell.cpp Atom.cpp Molecule.cpp
        PeriodicTable.cpp Defaults/NWXPeriodicTablePIMPL.cpp
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

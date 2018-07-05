set(
    LIBCHEMIST_HEADERS Atom.hpp BasisSet.hpp BasisShell.hpp Molecule.hpp
                       ShellTypes.hpp
)

set(
    LIBCHEMIST_SRCS Atom.cpp BasisSet.cpp BasisShell.cpp Molecule.cpp
                    ShellTypes.cpp
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

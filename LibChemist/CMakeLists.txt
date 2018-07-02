set(
    LIBCHEMIST_HEADERS Atom.hpp BasisSet.hpp BasisShell.hpp Molecule.hpp
                       ShellTypes.hpp
)

set(
    LIBCHEMIST_SRCS Atom.cpp BasisSet.cpp BasisShell.cpp Molecule.cpp
                    ShellTypes.cpp
)

add_nwx_library(chemist "${LIBCHEMIST_SRCS}" "${LIBCHEMIST_HEADERS}")
target_link_libraries(chemist Utilities::utilities)

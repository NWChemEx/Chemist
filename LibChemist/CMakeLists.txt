set(
    LIBCHEMIST_HEADERS
        Implementations/AOShellPIMPL.hpp
        Implementations/AtomPIMPL.hpp
        AOShell.hpp
        Atom.hpp
)
set(
    LIBCHEMIST_SRCS AOShell.cpp Atom.cpp
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

set(
    LIBCHEMIST_HEADERS Implementations/AtomPIMPL.hpp
        Atom.hpp
)
set(
    LIBCHEMIST_SRCS Atom.cpp
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

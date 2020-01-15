list(APPEND LIBCHEMIST_SRCS
     basis_set/ao_basis_set/ao_basis_set.cpp
     basis_set/center/center.cpp
     basis_set/contracted_gaussian/contracted_gaussian.cpp
     basis_set/contracted_gaussian/contracted_gaussian_view.cpp
     basis_set/primitive/primitive.cpp
     basis_set/primitive/primitive_view.cpp
     basis_set/shell/shell.cpp
     basis_set/shell/shell_view.cpp
)

list(APPEND LIBCHEMIST_HEADERS
     basis_set/ao_basis_set/ao_basis_set.hpp
     basis_set/center/center.hpp
     basis_set/contracted_gaussian/contracted_gaussian.hpp
     basis_set/contracted_gaussian/contracted_gaussian_view.hpp
     basis_set/detail_/flattened_view.hpp
     basis_set/primitive/primitive.hpp
     basis_set/primitive/primitive_view.hpp
     basis_set/shell/shell.hpp
     basis_set/shell/shell_view.hpp
     basis_set/basis_set.hpp
)
set(LIBCHEMIST_SRCS "${LIBCHEMIST_SRCS}" PARENT_SCOPE)
set(LIBCHEMIST_HEADERS "${LIBCHEMIST_HEADERS}" PARENT_SCOPE)

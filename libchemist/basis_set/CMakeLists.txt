list(APPEND LIBCHEMIST_SRCS
     basis_set/contracted_gaussian/contracted_gaussian.cpp
     basis_set/contracted_gaussian/contracted_gaussian_view.cpp
     basis_set/primitive/primitive.cpp
     basis_set/primitive/primitive_view.cpp
)

list(APPEND LIBCHEMIST_HEADERS
     basis_set/contracted_gaussian/contracted_gaussian.hpp
     basis_set/contracted_gaussian/contracted_gaussian_view.hpp
     basis_set/primitive/primitive.hpp
)
set(LIBCHEMIST_SRCS "${LIBCHEMIST_SRCS}" PARENT_SCOPE)
set(LIBCHEMIST_HEADERS "${LIBCHEMIST_HEADERS}" PARENT_SCOPE)

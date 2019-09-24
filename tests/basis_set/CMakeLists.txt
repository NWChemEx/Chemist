list(APPEND LIBCHEMIST_TESTS
     basis_set/contracted_gaussian/detail_/cgto_pimpl.cpp
     basis_set/contracted_gaussian/contracted_gaussian.cpp
     basis_set/primitive/detail_/primitive_pimpl.cpp
     basis_set/primitive/primitive.cpp
     basis_set/primitive/primitive_view.cpp
)
set(LIBCHEMIST_TESTS "${LIBCHEMIST_TESTS}" PARENT_SCOPE)

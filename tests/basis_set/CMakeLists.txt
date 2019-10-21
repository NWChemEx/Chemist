list(APPEND LIBCHEMIST_TESTS
     basis_set/center/detail_/center_pimpl.cpp
     basis_set/center/center.cpp
     basis_set/contracted_gaussian/detail_/cgto_pimpl.cpp
     basis_set/contracted_gaussian/contracted_gaussian.cpp
     basis_set/contracted_gaussian/contracted_gaussian_view.cpp
     basis_set/primitive/detail_/primitive_pimpl.cpp
     basis_set/primitive/primitive.cpp
     basis_set/primitive/primitive_view.cpp
     basis_set/shell/detail_/shell_pimpl.cpp
     basis_set/shell/shell.cpp
     basis_set/shell/shell_view.cpp
)
set(LIBCHEMIST_TESTS "${LIBCHEMIST_TESTS}" PARENT_SCOPE)

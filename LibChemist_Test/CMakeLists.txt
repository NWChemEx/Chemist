set(
    LIBCHEMIST_TESTS test_main.cpp TestAtom.cpp TestBasisSet.cpp
                     TestBasisShell.cpp TestMolecule.cpp
)

add_catch_cxx_tests(test_libchemist "${LIBCHEMIST_TESTS}" chemist)

set(
    LIBCHEMIST_TESTS test_main.cpp
        #TestAOBasisSet.cpp TestAOShell.cpp
        TestAtom.cpp
        TestAoSFacade.cpp
)

add_catch_cxx_tests(
        NAME test_libchemist
        SOURCES ${LIBCHEMIST_TESTS}
        DEPENDS chemist
)

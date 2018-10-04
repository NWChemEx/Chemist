set(
    LIBCHEMIST_TESTS test_main.cpp
                     TestAOBasisSet.cpp
                     TestAOShell.cpp
                     TestAoSFacade.cpp
                     TestAtom.cpp
                     TestLibChemist.cpp
                     TestMolecule.cpp
		     TestMoleculeFileParser.cpp
                     TestPeriodicTable.cpp
)

add_catch_cxx_tests(
        NAME test_libchemist
        SOURCES ${LIBCHEMIST_TESTS}
        DEPENDS chemist
)

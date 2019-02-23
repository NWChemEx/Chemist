set(LIBCHEMIST_TESTS test_main.cpp
                     TestAOBasisSet.cpp
                     TestAOShell.cpp
                     TestAoSFacade.cpp
                     TestAtom.cpp
		             TestBasisSetMap.cpp
                     TestLibChemist.cpp
                     TestMolecule.cpp
	                 TestMoleculeStreamParser.cpp
                     TestPeriodicTable.cpp
		             TestPropertyTypes.cpp
)

cpp_add_executable(
	test_libchemist
	SOURCES ${LIBCHEMIST_TESTS}
	DEPENDS chemist Catch2::Catch2
)

add_test(NAME test_libchemist COMMAND test_libchemist)

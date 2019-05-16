set(LIBCHEMIST_TESTS test_main.cpp
        			 ao_basis_set.cpp
					 ao_shell.cpp
					 aos_facade.cpp
					 atom.cpp
					 basis_set/detail_/gaussian_ao_pimpl.cpp
					 basis_set/detail_/gaussian_pimpl.cpp
					 basis_set/gaussian_ao.cpp
					 basis_set/gaussian.cpp
					 basis_set_map.cpp
					 libchemist.cpp
					 molecule.cpp
					 molecule_stream_parser.cpp
    				 periodic_table.cpp
)

cpp_add_executable(
	test_libchemist
	SOURCES ${LIBCHEMIST_TESTS}
	DEPENDS chemist Catch2::Catch2
)

add_test(NAME test_libchemist COMMAND test_libchemist)

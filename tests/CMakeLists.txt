add_subdirectory(basis_set)
add_subdirectory(detail_)
add_subdirectory(point)
add_subdirectory(tutorials)
list(APPEND LIBCHEMIST_TESTS test_main.cpp
	 basis_set.cpp
					 ao_shell.cpp
					 aos_facade.cpp
					 atom.cpp
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

add_subdirectory(point)
list(APPEND LIBCHEMIST_HEADERS ao_basis_set.hpp
	                   ao_shell.hpp
					   atom.hpp
					   basis_set_manager.hpp
					   basis_set_map.hpp
					   libchemist.hpp
		       		   molecule.hpp
					   molecule_stream_parser.hpp
					   molecule_manager.hpp
					   orbital_space.hpp
					   periodic_table.hpp
					   detail_/aos_facade.hpp
        			   defaults/nwx_basis_list.hpp
					   defaults/nwx_atomic_densities.hpp
)

include(defaults/bases/add_basis.cmake)

include(defaults/atomic_densities/add_density.cmake)

list(APPEND LIBCHEMIST_SRCS ao_basis_set.cpp
	         	    ao_shell.cpp
					atom.cpp
					basis_set_manager.cpp
					basis_set_map.cpp
					molecule.cpp
					molecule_stream_parser.cpp
					molecule_manager.cpp
					periodic_table.cpp
					defaults/nwx_basis_set_manager_pimpl.cpp
					defaults/nwx_periodic_table_pimpl.cpp
					defaults/nwx_molecule_manager_pimpl.cpp
					defaults/nwx_atomic_densities.cpp
                    ${LIBCHEMIST_BASIS_SOURCE}
		    		${LIBCHEMIST_DENSITY_SOURCE}
)

cpp_add_library(
		chemist
		SOURCES ${LIBCHEMIST_SRCS}
		INCLUDES ${LIBCHEMIST_HEADERS}
		DEPENDS sde::sde CMakeBuild tamm
)

cpp_install(TARGETS chemist)

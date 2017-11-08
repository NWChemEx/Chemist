# - Try to find blas
#
#  In order to aid find_package the user may set BLAS_ROOT_DIR to the root of
#  the installed blas.
#
#  Once done this will define
#  BLAS_LAPACK_FOUND - System has blas, lapack
#  BLAS_LAPACK_INCLUDE_DIR - The blas include directories
#  BLAS_LAPACK_LIBRARIES - The libraries needed to use blas

#Prefer BLAS_ROOT_DIR if the user specified it
if(NOT DEFINED BLAS_ROOT_DIR)
    find_package(PkgConfig)
    pkg_check_modules(PC_BLAS_LAPACK QUIET blas)
endif()

find_path(BLAS_LAPACK_INCLUDE_DIR cblas.h
      HINTS ${PC_BLAS_LAPACK_INCLUDEDIR}
            ${PC_BLAS_LAPACK_INCLUDE_DIRS}
      PATHS ${BLAS_ROOT_DIR})

find_library(BLAS_LIBRARY NAMES libblas${CMAKE_STATIC_LIBRARY_SUFFIX}
 libcblas${CMAKE_STATIC_LIBRARY_SUFFIX}
         HINTS ${PC_BLAS_LAPACK_LIBDIR}
               ${PC_BLAS_LAPACK_LIBRARY_DIRS}
         PATHS ${BLAS_ROOT_DIR}
    )

find_library(CBLAS_LIBRARY NAMES libcblas${CMAKE_STATIC_LIBRARY_SUFFIX}
            HINTS ${PC_BLAS_LAPACK_LIBDIR}
                  ${PC_BLAS_LAPACK_LIBRARY_DIRS}
            PATHS ${BLAS_ROOT_DIR}
       )

find_library(LAPACK_LIBRARY NAMES liblapack${CMAKE_STATIC_LIBRARY_SUFFIX}
    liblapacke${CMAKE_STATIC_LIBRARY_SUFFIX}
            HINTS ${PC_LAPACK_LIBDIR}
                  ${PC_LAPACK_LIBRARY_DIRS}
            PATHS ${BLAS_ROOT_DIR}
       )

find_library(LAPACKE_LIBRARY NAMES liblapacke${CMAKE_STATIC_LIBRARY_SUFFIX}
            HINTS ${PC_LAPACK_LIBDIR}
                  ${PC_LAPACK_LIBRARY_DIRS}
            PATHS ${BLAS_ROOT_DIR}
      )       

set(BLAS_LAPACK_LIBRARIES ${BLAS_LIBRARY} ${CBLAS_LIBRARY} 
      ${LAPACK_LIBRARY} ${LAPACKE_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Blas_Lapack DEFAULT_MSG
                              BLAS_LAPACK_LIBRARIES
                              BLAS_LAPACK_INCLUDE_DIR)



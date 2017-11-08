################################################################################
#
#  Now we build NWX_Catch, which is our installed version of Catch. It
#  differs from the default one in that we've installed the header and we've
#  taken the liberty of compiling the main function into a library to help
#  with compile times.
#
################################################################################

set(CATCH_PREFIX ${CMAKE_BINARY_DIR}/catch)
set(CATCH_HEADER_FILE_INITIAL
    ${CATCH_PREFIX}/src/NWX_Catch${TARGET_SUFFIX}/single_include/catch.hpp)
include(ExternalProject)
ExternalProject_Add(NWX_Catch_External
                    PREFIX ${CATCH_PREFIX}
                    GIT_REPOSITORY
                    https://github.com/philsquared/Catch.git
                    UPDATE_COMMAND ""
                    CONFIGURE_COMMAND ""
                    BUILD_COMMAND ""
                    INSTALL_COMMAND ""
                    )
install(FILES ${CATCH_HEADER_FILE_INITIAL} DESTINATION include/catch/)

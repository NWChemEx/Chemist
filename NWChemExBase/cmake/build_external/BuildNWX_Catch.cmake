################################################################################
#
#  Now we build NWX_Catch, which is our installed version of Catch. It
#  differs from the default one in that we've installed the header and we've
#  taken the liberty of compiling the main function into a library to help
#  with compile times.
#
################################################################################

set(CATCH_PREFIX ${CMAKE_BINARY_DIR}/catch)
set(CATCH_HEADER_FILE
        ${CATCH_PREFIX}/src/NWX_Catch_External/single_include/catch.hpp)
include(ExternalProject)
ExternalProject_Add(NWX_Catch_External
        PREFIX ${CATCH_PREFIX}
        GIT_REPOSITORY https://github.com/philsquared/Catch.git
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${CATCH_HEADER_FILE}
            ${STAGE_DIR}/${CMAKE_INSTALL_PREFIX}/include/catch/catch.hpp
        )

cmake_minimum_required(VERSION 3.1)
project(NWChemExBase VERSION 0.0.0 LANGUAGES NONE)
file(GLOB __PLB_FILES "${PLB_PREFIX}/cmake/*.cmake")
install(FILES ${__PLB_FILES}
        DESTINATION share/cmake/NWChemExBase
)
configure_file("${PLB_PREFIX}/cmake/NWChemExBaseConfig.cmake.in"
               NWChemExBaseConfig.cmake @ONLY
)
install(FILES ${CMAKE_BINARY_DIR}/NWChemExBaseConfig.cmake
        DESTINATION share/cmake/NWChemExBase
)

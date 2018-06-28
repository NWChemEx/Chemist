#This file contains macros that are useful for taking care of boilerplate in
#CMakeLists.txt.  They may require tweaking to satisfy your particular
#situation.

# Wraps the boilerplate for starting Hunter.  Must be a macro and not a function
# because Hunter introduces some variables it needs into the global namespace.
# To force Hunter to use a hunter/Config.cmake file for your project add "LOCAL"
# to the end of the HunterGate command.
macro(start_hunter)
    #Change this and the next line to change the version of hunter used
    set(__HUNTER_VERSION "v0.22.14")
    set(__HUNTER_SHA1 "f194eab02248f7d8792f7fc0158f6194d005bf86")

    #Assemble the GitHub URL to use
    set(__GH_URL "https://github.com/ruslo/hunter/archive/")
    set(__GH_URL "${__GH_URL}${__HUNTER_VERSION}.tar.gz")

    #Include and call Hunter
    include("cmake/HunterGate.cmake")
    HunterGate(URL "${__GH_URL}" SHA1 "${__HUNTER_SHA1}" LOCAL)
endmacro()

#Wraps the boilerplate associated with making a Catch test.
#Arguments:
# - __name : A string that will be used for the name of the test and the binary
# - __srcs : A list of  source files that should be compiled together to produce
#            the binary
# - __targets: A list of targets that the tests need to link to
function(add_catch_cxx_tests __name __srcs __targets)
    enable_testing()
    hunter_add_package(Catch)
    find_package(Catch2 CONFIG REQUIRED)
    add_executable(${__name} "${__srcs}")
    target_link_libraries(${__name} PRIVATE Catch2::Catch ${__targets})
    add_test(NAME "${__name}" COMMAND ${__name})
endfunction()


#Writes the files needed for find_package(${PROJECT_NAME} CONFIG)
#Resulting target can be linked against as ${PROJECT_NAME}::${__target}
#Header files will be installed in ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
#Arguments:
#   - __target: The name of the target to install
#   - __headers: A list of headers with paths relative to the invocation of this
#                function.
#Uses the variables:
#   - PROJECT_NAME
#   - PROJECT_VERSION
function(install_targets __targets __headers)
    # Introduce variables:
    # * CMAKE_INSTALL_LIBDIR
    # * CMAKE_INSTALL_BINDIR
    # * CMAKE_INSTALL_INCLUDEDIR
    include(GNUInstallDirs)

    # Layout. This works for all platforms:
    #   * <prefix>/lib*/cmake/<PROJECT-NAME>
    #   * <prefix>/lib*/
    #   * <prefix>/include/
    set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

    # Directory where the generated files will be stored.
    set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

    # Configuration
    set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
    set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
    set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
    set(namespace "${PROJECT_NAME}::")

    # Include module with fuction 'write_basic_package_version_file'
    include(CMakePackageConfigHelpers)

    # Configure '<PROJECT-NAME>ConfigVersion.cmake'
    # Use:
    #   * PROJECT_VERSION
    write_basic_package_version_file(
        "${version_config}" COMPATIBILITY SameMajorVersion
    )

    # Configure '<PROJECT-NAME>Config.cmake'
    # Use variables:
    #   * TARGETS_EXPORT_NAME
    #   * PROJECT_NAME
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in"
        "${project_config}"
        INSTALL_DESTINATION "${config_install_dir}"
    )

    # Install targets
    install(
            TARGETS ${__targets}
            EXPORT "${TARGETS_EXPORT_NAME}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    #Install headers
    #
    #This next part can be a bit tricky.  We assume that ${__headers} is a list
    #of header files relative to the directory this function was called from.
    #The tricky part is that relative path may contain subdirectories.  If we
    #just tell CMake to install the headers it won't make the subdirectories.
    #To avoid this, we loop over the files, grabbing the directories (if they
    #exist) and append them to the path.
    foreach(__header ${__headers})
        get_filename_component(__dir ${__header} DIRECTORY)
        install(
            FILES ${__header}
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${__dir}"
        )
    endforeach()

    # Signal the need to install the config files we just made
    install(
            FILES "${project_config}" "${version_config}"
            DESTINATION "${config_install_dir}"
    )
    install(
            EXPORT "${TARGETS_EXPORT_NAME}"
            NAMESPACE "${namespace}"
            DESTINATION "${config_install_dir}"
    )
endfunction()

#Wraps the boilerplate associated with making a library.
#This function assumes that your sources and includes are all contained in some
#directory, call it "root".  Within "root" there may be additional directory
#structure.  Furthermore, it is assumed that all source files include header
#files relative to "root".
#Arguments:
#    - __name: The name of the target, will also be used to name the library
#    - __srcs: A list of the src files associated with the library
#    - __headers: A list of the header files to install
#Uses the variables:
#    - PROJECT_SOURCE_DIR: Used as build include path
function(add_nwx_library __name __srcs __headers)
    add_library(${__name} "${__srcs}")
    target_include_directories(
            ${__name} PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    target_compile_features(${__name} PUBLIC cxx_std_14)
    install_targets(${__name} "${__headers}")
endfunction()

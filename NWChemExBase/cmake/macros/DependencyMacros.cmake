################################################################################
#
# These are macros for finding dependencies.
#
################################################################################

include(DebuggingMacros)
include(UtilityMacros)

#
# Macro for finding a dependency that is presumed built.  It is assumed that
# whatever name is given
# to this macro, is a name that can be directly fed to CMake's find_package
# macro.  Furthermore it is assumed that the result is given back as the
# standard variables, e.g.
# XXX_INCLUDE_DIRS, XXX_LIBRARIES, etc. where XXX is the name of the library in
# all capital letters.
#
# Syntax: find_dependency(<name> <include> <lib> <flags> <required>)
#    - name     : The name to be passed to find_package
#    - include  : The variable to append the includes on to
#    - lib      : The variable to append the libraries on to
#    - flags    : The variable to append the compile-time flags on to
#
function(find_dependency __name _include_dirs _libraries _definitions)
    find_package(${__name} REQUIRED)
    string(TOUPPER ${__name} __NAME)
    if(${__name}_FOUND OR ${__NAME}_FOUND)
        foreach(__VAR_TYPE _INCLUDE_DIRS _LIBRARIES _DEFINITIONS)
            string(TOLOWER ${__VAR_TYPE} __var_type)
            set(__var ${__NAME}${__VAR_TYPE})#
            set(__parent_var ${${__var_type}})
            is_valid(${__var} has_var)
            if(has_var)
                list(APPEND ${__parent_var} ${${__var}})
                set(${__parent_var} ${${__parent_var}} PARENT_SCOPE)
            endif()
        endforeach()
    endif()
endfunction()

#
# Macro for finding a dependency and building it if it is not found.  Either way
# a target with the name of the dependency suffixed with "_External" will be
# added.
#    - name : The case-sensitive name for the dependency
#    - was_found : a variable whose value will be true if name was found
#
function(find_or_build_dependency __name __was_found)
    if(TARGET ${__name}_External)
        debug_message(STATUS "${__name} already handled.")
    else()
        find_package(${__name} QUIET)
        string(TOUPPER ${__name} __NAME)
        if(${__name}_FOUND OR ${__NAME}_FOUND)
            set(${__was_found} TRUE PARENT_SCOPE)
            debug_message("Found ${__name}:")
            foreach(__VAR_TYPE INCLUDE_DIRS LIBRARIES DEFINITIONS)
                set(__var ${__NAME}_${__VAR_TYPE})
                is_valid_and_true(${__var} has_var)
                if(has_var)
                    debug_message("${__VAR_TYPE}: ${${__var}}")
                endif()
            endforeach()
            add_library(${__name}_External INTERFACE)
        else()
            debug_message("Unable to locate ${__name}.  Building one instead.")
            include(Build${__name})
        endif()
    endif()
endfunction()

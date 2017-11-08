################################################################################
#
#  These are macros to aid in debugging CMake scripts
#
################################################################################

#
# prints the path to the current file
#
# Syntax: debug_file_info()
#
function(debug_file_info)
    message(STATUS "In ${CMAKE_CURRENT_LIST_FILE}")
endfunction()

#
# prints the name of a variable and its current value
#
# Syntax: debug_print(<variable>)
function(debug_print _variable)
    debug_file_info()
    message(STATUS "\$\{${_variable}\} =(${${_variable}})")
endfunction()

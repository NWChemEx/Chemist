include(UtilityMacros.cmake)
include(AssertMacros.cmake)

is_valid(not_defined test1)
assert(NOT test1)

set(blank_string "")
is_valid(blank_string test2)
assert(NOT test2)

set(valid_string "this_is_valid")
is_valid(valid_string test3)
assert(test3)

set(valid_list item1 item2)
is_valid(valid_list test4)
assert(test4)

set(is_valid_false FALSE)
is_valid_and_true(is_valid_false test5)
assert(NOT test5)

set(is_valid_true TRUE)
is_valid_and_true(is_valid_true test6)
assert(test6)

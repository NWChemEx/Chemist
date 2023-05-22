#pragma once
#include <pybind11/pybind11.h>

namespace chemist {

using python_module_type = pybind11::module_;

using python_module_reference = python_module_type&;

template<typename... Args>
using python_class_type = pybind11::class_<Args...>;

} // namespace chemist

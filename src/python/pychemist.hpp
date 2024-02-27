/*
 * Copyright 2023 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <pybind11/pybind11.h>

namespace chemist {

using python_module_type = pybind11::module_;

using python_module_reference = python_module_type&;

template<typename... Args>
using python_class_type = pybind11::class_<Args...>;

template<typename... Args>
using python_enum_type = pybind11::enum_<Args...>;

/** @brief Convenience function for converting pybind11::args into a std::vector
 *
 *  When creating Python bindings for C++ functions which take a variadic number
 *  of inputs, the Python arguments will come into C++ as a pybind11::args
 *  object. This function wraps the process of unwrapping the pybind11::args
 *  object into a buffer, assuming that each object has the same C++ type.
 *
 *  @tparam T The C++ type each member of `args` is expected to be. The user
 *            must specify @p T because the compiler can not deduce it.
 *
 *  @param[in] args The pack of variadic python arguments to unpack.
 *  @return A std::vector<T> containing the unwrapped arguments.
 *
 *  @throw std::bad_alloc if there's a problem allocating the buffer. Strong
 *                        throw guarantee.
 */
template<typename T>
auto args_to_buffer(pybind11::args args) {
    std::vector<T> buffer(args.size());
    for(decltype(args.size()) i = 0; i < args.size(); ++i)
        buffer[i] = args[i].cast<T>();
    return buffer;
}

} // namespace chemist

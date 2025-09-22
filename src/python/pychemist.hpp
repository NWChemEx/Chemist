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
#include <pybind11/native_enum.h>
#include <pybind11/pybind11.h>

namespace chemist {

using python_module_type = pybind11::module_;

using python_module_reference = python_module_type&;

template<typename... Args>
using python_class_type = pybind11::class_<Args...>;

template<typename... Args>
using python_enum_type = pybind11::native_enum<Args...>;

/** @brief Convenience function for checking if a pybind11 object can be
 *         cast to  type T
 *
 *  @tparam T The C++ type we are trying to convert the pybind11 object to.
 *  @tparam U The type of the pybind11 object holding the Python object. Assumed
 *            to be a type like `pybind11::object`
 *
 *  As far as we are aware, pybind11 does not provide a mechanism for
 *  determining if a cast will pass without trying the cast. This function
 *  wraps the process of trying the cast and reports whether or not the cast
 *  passed.
 *
 *  @param[in] pybind_object The wrapped Python object we are trying to convert.
 *
 *  @return True if @p pybind_object can be converted to an object of type
 *          @p T and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename T, typename U>
bool can_cast(U pybind_object) {
    try {
        pybind_object.template cast<T>();
        return true;
    } catch(...) {}
    return false;
}

/** @brief Converts a pybind11 container to a std::vector<T>
 *
 *  @tparam T The C++ type of the resulting container's elements.
 *  @tparam U The type of the pybind11 object we are converting. @p U is
 *            expected to be a type like `pybind11::list` or `pybind11::args`.
 *            Each element of @p U must be convertible to type @p T.
 *
 *  This function wraps the process of unwrapping a Python container into a
 *  C++ container. Such processes must be done element-by-element.
 *
 *  @param[in] container The Python container to unwrap.
 *
 *  @return A C++ container filled with C++ objects of type @p T.
 *
 *  @throw std::bad_alloc if there is a problem allocating the return. Strong
 *                        throw guarantee.
 */
template<typename T, typename U>
auto pybind_to_buffer(U container) {
    std::vector<T> buffer;
    for(auto& element : container) buffer.push_back(element.template cast<T>());
    return buffer;
}

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
    return pybind_to_buffer<T>(args);
}

} // namespace chemist

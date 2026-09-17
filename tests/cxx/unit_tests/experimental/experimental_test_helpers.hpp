/*
 * Copyright 2026 NWChemEx-Project
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
#include <type_traits>
#include <wtf/wtf.hpp>

namespace test_chemist {

/** @brief Pulls a concrete double back out of a type-erased value.
 *
 *  The experimental classes keep the concrete floating-point type out of
 *  their API, so a test which wants to compare against a literal has to name
 *  the type itself. Both owning Float objects and FloatView objects convert
 *  to the parameter type, so one overload covers both.
 *
 *  @param[in] value The type-erased value to unwrap.
 *
 *  @return The value, as a double.
 *
 *  @throw std::runtime_error if @p value is not holding a double. Strong
 *                            throw guarantee.
 */
inline double as_double(wtf::fp::FloatView<const wtf::fp::Float> value) {
    return value.value<double>();
}

/** @brief Detects whether `set_x` can be called on a @p T.
 *
 *  The experimental setters are removed by SFINAE when the object aliases
 *  read-only state, rather than being present and throwing. This trait is
 *  what lets a test assert that, since "does not compile" can not be a
 *  runtime assertion.
 */
///@{
template<typename T, typename = void>
struct has_setters : std::false_type {};

template<typename T>
struct has_setters<T, std::void_t<decltype(std::declval<T&>().set_x(0.0))>>
  : std::true_type {};

template<typename T>
inline constexpr bool has_setters_v = has_setters<T>::value;
///@}

} // namespace test_chemist

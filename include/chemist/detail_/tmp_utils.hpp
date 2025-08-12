/*
 * Copyright 2024 NWChemEx-Project
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

/** @file tmp_utils.hpp
 *
 *  This file contains template meta-programming utility traits needed for
 *  implementing many of the classes in the ChemicalSystem hierarchy (notably
 *  the views).
 */

#pragma once
#include <type_traits>

namespace chemist::detail_ {

/** @brief Enables implicit conversion from a mutable view to a read-only view.
 *
 *  As an example say we have a class `View<U>` where `U` may be either `T` or
 *  `const T`. `View<T>` objects should be assignable to `View<const T>` objects
 *  (basically the equivalent of doing `double y; const double& x = y`), but
 *  not the other way around. Since `View<T>` and `View<const T>` are different
 *  types, the conversion will not happen automatically and we must define an
 *  implicit conversion. We also only want this conversion to only be callable
 *  when the *this argument is `View<const T>` and the explict argument is of
 *  type `View<T>`.
 *
 *  The enable_mutable_to_const_t variable template can be used to ensure that a
 *  conversion is only considered under these conditions (this happens via
 *  SFINAE). To use this variable template looks like:
 *
 *  ```
 *  template<typename T, typename = enable_mutable_to_const<T>>
 *  View(const View<T>& other) :
 *  ```
 *
 *  @note When using this variable template like the code snippet shows do NOT
 *        mark the ctor as explicit (or it won't be considered unless you
 *        explicitly cast).
 *
 *  @tparam U The template type parameter of the view which will be the *this
 *            argument of the conversion.
 *  @tparam T the template type parameter of the view which we are trying to
 *            convert from.
 */
template<typename T, typename U>
using enable_mutable_to_const_t =
  std::enable_if_t<std::is_const_v<U> && std::is_same_v<const T, U>>;

} // namespace chemist::detail_

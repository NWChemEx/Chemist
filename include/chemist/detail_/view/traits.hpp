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
#include <type_traits>

namespace chemist::detail_ {

/** @brief Code factorization for TMP associated with views.
 *
 *  A pattern commonly used throughout Chemist involves a container returning
 *  views of its elements instead of the actual elements. Each view is typically
 *  a class like `View<T>` with the idea being that the template class
 *  `View`` acts like a reference to an object of type `T`. If `T` is an
 *  unqualified type (notably is not const-qualified), than the view acts like
 *  a read/write reference. If `T` is however a const-qualified type, than the
 *  view behaves like a read-only reference. Suffice it to say that properly
 *  implementing a const-correct view requires some template meta-programming,
 *  which we have factored out into the `ViewTraits` class.
 *
 *  @tparam T The type the view is a view of, e.g. for PointView<U> T would be
 *            U.
 */
template<typename T>
struct ViewTraits {
    /// The non-cv-qualified version of T
    using type = std::remove_cv_t<T>;

    /// The const-qualified version of T
    using const_type = const type;

    /// Was @p T actually const-qualified?
    static constexpr bool is_const_v = std::is_same_v<T, const type>;

    /** @brief Makes an un-qualified type @p U have the same const-ness as @p T
     *
     *  This helper will add `const` to @p U if @p T is const-qualified.
     *  Otherwise the helper will return @p U. For example:
     *
     *  ```
     *  using T = Point<float>;
     *
     *  // X will be int
     *  using X = ViewTraits<T>::apply_const<int>;
     *
     *  // Y will be const int
     *  using Y = ViewTraits<const T>::apply_const<int>;
     *  ```
     *
     *  @tparam U The type we want to parallel the const-ness of @p T.
     */
    template<typename U>
    using apply_const = std::conditional_t<is_const_v, const U, U>;

    /** @brief Makes an un-qualified type @p U into a reference with the same
     *         const-ness as @p T
     *
     *  This helper will add `const` and a reference to @p U if @p T is
     *  const-qualified. Otherwise the helper will return a mutable reference
     *  to @p U. For example:
     *
     *  ```
     *  using T = Point<float>;
     *
     *  // X will be int&
     *  using X = ViewTraits<T>::apply_const_ref<int>;
     *
     *  // Y will be const int&
     *  using Y = ViewTraits<const T>::apply_const_ref<int>;
     *  ```
     *
     *  @tparam U The type to turn into a reference with const-ness paralleling
     *            that of @p T.
     */
    template<typename U>
    using apply_const_ref = std::conditional_t<is_const_v, const U&, U&>;

    /** @brief Makes an un-qualified type @p U into a pointer with the same
     *         const-ness as @p T
     *
     *  This helper will add `const` and a pointer to @p U if @p T is
     *  const-qualified. Otherwise the helper will return a mutable pointer
     *  to @p U. For example:
     *
     *  ```
     *  using T = Point<float>;
     *
     *  // X will be int*
     *  using X = ViewTraits<T>::apply_const_ptr<int>;
     *
     *  // Y will be const int*
     *  using Y = ViewTraits<const T>::apply_const_ptr<int>;
     *  ```
     *
     *  @tparam U The type to turn into a pointer with const-ness paralleling
     *            that of @p T.
     */
    template<typename U>
    using apply_const_ptr = std::conditional_t<is_const_v, const U*, U*>;
};

} // namespace chemist::detail_

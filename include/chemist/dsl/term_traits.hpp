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

#pragma once
#include <chemist/dsl/dsl_fwd.hpp>

namespace chemist::dsl {

/** @brief Class used to work out the DSL traits for an object of type @p T.
 *
 *  @tparam T The type we are working out the traits for. Can be mutable or
 *            const-qualified
 *
 *  The DSL component is designed to work with a myriad of object types. In
 *  order for this to work we need to know how to take both mutable and
 *  read-only references to the object. This trait works out the types of those
 *  references. Users can specialize this class to modify the behavior of the
 *  DSL for their type
 */
template<typename T>
class TermTraits {
public:
    /// Can @p T be expressed as `const U` where U is a non-const type?
    static constexpr bool is_const_v = std::is_const_v<T>;

    /** @brief  The type of @p T with all qualifiers removed
     *
     *  The TermTraits class allows the template type parameter to be qualified.
     *  This member type strips all of the qualifiers off. This member type is
     *  used throughout this class for:
     *
     *  - Deriving new types (e.g., adding an ampersand creates the reference
     *    type).
     *  - Determining if two TermTraits objects describe the same type
     */
    using value_type = std::decay_t<T>;

    /** @brief The type of a read-only reference to an object of type @p T.
     *
     *  When a DSL term contains an object of type @p T, requests to access the
     *  term via a read-only reference will return an object of this type. This
     *  class defines that type as `const value_type&`.
     */
    using const_reference = const value_type&;

    /** @brief The type of a (possibly) mutable reference to an object of
     *         type @p T.
     *
     *  When a DSL term contains an object of type @p T, requests to access the
     *  term as a mutable reference will return an object of this type. This
     *  class defines that type as `const_reference` if @p T is a const-
     *  qualified type and `value_type&` if @p T is not const-qualified.
     */
    using reference =
      std::conditional_t<is_const_v, const_reference, value_type&>;

    /** @brief Is @p T part of the DSL layer?
     *
     *  Terms that are part of the DSL layer are often unnamed temporaries and
     *  their storage must be handled specially. This member variable is used
     *  to determine if @p T either derives from dsl::Term, or if it is a
     *  floating point type (floating point types are often specified inline as
     *  if they were part of the DSL).
     */
    static constexpr bool is_dsl_term_v =
      std::is_base_of_v<Term<value_type>, value_type> ||
      std::is_floating_point_v<value_type>;

    /** @brief The type terms will hold @p T as.
     *
     *  The terms of the DSL capture the inputs as the DSL is built up. When a
     *  term needs to hold an object of type @p T it does so by storing it as
     *  an object of type `holder_type`. For leaf objects (objects not part of
     *  the DSL) the default is to hold them as `reference` objects. This means
     *  that the DSL is NOT responsible for their lifetime. If @p T is an
     *  object that is part of the DSL then @p T is captured by value (DSL
     *  terms are expected to be light-weight and temporary).
     */
    using holder_type =
      std::conditional_t<is_dsl_term_v, value_type, reference>;
};

} // namespace chemist::dsl
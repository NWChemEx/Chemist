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

/** @brief Base class for all elements of the DSL.
 *
 *  @tparam DerivedType Type of the object *this is implementing.
 *
 *  Users of the DSL need to implement operator+, operator-, etc. for their
 *  leaves. The returns of those functions are DSL Term objects. Those objects
 *  can then further be composed. The Term class implements further
 *  composition with DSL objects.
 */
template<typename DerivedType>
class Term {
public:
    /** @brief Adds *this to @p rhs.
     *
     *  @tparam RHSType The type of @p rhs.
     *
     *  This method will create an object representing left addition by *this
     *  to @p rhs.
     *
     *  @param[in] rhs The object to *this will be added.
     *
     *  @throw ??? Throws if creation of the new DSL term throws. Same throw
     *             guarantee.
     */
    template<typename RHSType>
    auto operator+(RHSType&& rhs) {
        auto& lhs      = static_cast<DerivedType&>(*this);
        using no_ref_t = std::remove_reference_t<RHSType>;
        return Add<DerivedType, no_ref_t>(lhs, std::forward<RHSType>(rhs));
    }

    /** @brief Subtracts @p rhs from this.
     *
     *  @tparam RHSType The type of @p rhs.
     *
     *  This method will create an object representing subtracting @p rhs from
     *  *this.
     *
     *  @param[in] rhs The object to be subtracted from *this.
     *
     *  @throw ??? Throws if creation of the new DSL term throws. Same throw
     *             guarantee.
     */
    template<typename RHSType>
    auto operator-(RHSType&& rhs) {
        auto& lhs      = static_cast<DerivedType&>(*this);
        using no_ref_t = std::remove_reference_t<RHSType>;
        return Subtract<DerivedType, no_ref_t>(lhs, std::forward<RHSType>(rhs));
    }

    /** @brief Multiplies *this to @p rhs.
     *
     *  @tparam RHSType The type of @p rhs.
     *
     *  This method will create an object representing left multiplication by
     *  *this to @p rhs.
     *
     *  @param[in] rhs The object *this will be multiply.
     *
     *  @throw ??? Throws if creation of the new DSL term throws. Same throw
     *             guarantee.
     */
    template<typename RHSType>
    auto operator*(RHSType&& rhs) {
        auto& lhs      = static_cast<DerivedType&>(*this);
        using no_ref_t = std::remove_reference_t<RHSType>;
        return Multiply<DerivedType, no_ref_t>(lhs, std::forward<RHSType>(rhs));
    }

    /** @brief Divides *this by @p rhs.
     *
     *  @tparam RHSType The type of @p rhs.
     *
     *  This method will create an object representing division of
     *  *this by @p rhs.
     *
     *  @param[in] rhs The object to divide *this by.
     *
     *  @throw ??? Throws if creation of the new DSL term throws. Same throw
     *             guarantee.
     */
    template<typename RHSType>
    auto operator/(RHSType&& rhs) {
        auto& lhs      = static_cast<DerivedType&>(*this);
        using no_ref_t = std::remove_reference_t<RHSType>;
        return Divide<DerivedType, no_ref_t>(lhs, std::forward<RHSType>(rhs));
    }
};

} // namespace chemist::dsl
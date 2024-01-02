/*
 * Copyright 2022 NWChemEx-Project
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
#include "chemist/potentials/electrostatic.hpp"
#include <cstddef>
#include <typeindex>

namespace chemist::operators::detail_ {

/** @brief An abstract base class for Operator types
 *
 *  This class provides the basic API specifications for operator types to
 *  satisfy `Any` type requirements and to allow for their identification and
 *  manipulation in a type-erased manner.
 *
 *  All Operator types are to be derived from this class.
 */
class OperatorBase {
public:
    /// Type of the scalar multiplying the operator
    using scalar_type = double;

    std::unique_ptr<OperatorBase> clone() const { return clone_impl(); }

    scalar_type& coefficient() noexcept { return m_c_; }

    scalar_type coefficient() const noexcept { return m_c_; }

    /// Polymorphic comparison of this Operator instance with another
    inline bool is_equal(const OperatorBase& other) const noexcept;

    /** @brief Non-polymorphic equality comparison of OperatorBase instances
     *
     *  Two OperatorBase instances are equal if they contain the same exact
     *  scale factor.
     *
     *  @param[in] other Operator instance to compare
     *  @return    True if the OperatorBase instances are equal and false
     *             otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const OperatorBase& other) const;

    /** @brief Non-polymorphic inequality comparison of Operator instances
     *
     *  Two OperatorBase instances are different if they contain different
     *  scale factors.
     *
     *  @param[in] other Operator instance to compare
     *  @return    False if the OperatorBase instances are equal and true
     *             otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const OperatorBase& other) const;

    /** @brief Returns a string containing the operator as a symbol.
     *
     *  This function creates a string representation of the present operator.
     *  The string is suitable for use in math expressions and is not a string
     *  representation of the operator's state.
     *
     *  @return A symbol representing the operator.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory for
     *                        the resulting string. Strong throw guarantee.
     */
    std::string as_string() const { return as_string_impl(); }

    /// Polymorphic defaulted no-throw dtor
    virtual ~OperatorBase() noexcept = default;

protected:
    /// Defaulted default noexcept ctor
    OperatorBase() noexcept = default;
    /// Defaulted copy ctor, protected to avoid slicing
    OperatorBase(const OperatorBase&) noexcept = default;
    /// Defaulted move ctor, protected to avoid slicing
    OperatorBase(OperatorBase&&) noexcept = default;
    ///
    explicit OperatorBase(scalar_type c) : m_c_(c){};
    /// Defaulted copy assignment, protected to avoid slicing
    OperatorBase& operator=(const OperatorBase&) = default;
    /// Defaulted move assignment, protected to avoid slicing
    OperatorBase& operator=(OperatorBase&&) = default;

    /// Derived implementation of comparison function.
    virtual bool is_equal_impl(const OperatorBase&) const noexcept = 0;
    /// Derived implementation of polymorphic copy
    virtual std::unique_ptr<OperatorBase> clone_impl() const = 0;
    /// Derived implementation of as_string
    virtual std::string as_string_impl() const = 0;

private:
    scalar_type m_c_ = 1.0;
};

// -----------------------------------------------------------------------------
// -------------------------- Inline Implementations ---------------------------
// -----------------------------------------------------------------------------

inline bool OperatorBase::operator==(const OperatorBase& other) const {
    return m_c_ == other.m_c_;
}

inline bool OperatorBase::operator!=(const OperatorBase& other) const {
    return !((*this) == other);
}

inline bool OperatorBase::is_equal(const OperatorBase& other) const noexcept {
    return is_equal_impl(other) and other.is_equal_impl(*this);
}

} // namespace chemist::operators::detail_

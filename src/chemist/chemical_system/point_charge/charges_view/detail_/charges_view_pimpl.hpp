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
#include <cassert>
#include <chemist/chemical_system/point_charge/charges_view/charges_view.hpp>

namespace chemist::detail_ {

/** @brief Establishes the API for all PIMPLs used to implement ChargesView.
 *
 *  This is an abstract base class establishing the API the ChargesView class
 *  should rely on when interacting with its PIMPL.
 *
 *  @tparam ChargesType The cv-qualified type that *this will act like it is
 *                      referencing.
 */
template<typename ChargesType>
class ChargesViewPIMPL {
private:
    /// Type *this implements
    using parent_type = ChargesView<ChargesType>;

public:
    /// Type of a pointer to *this
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    /// Type of a mutable reference to a point charge in *this
    using reference = typename parent_type::reference;

    /// Type of a read-only reference to a point charge in *this
    using const_reference = typename parent_type::const_reference;

    /// Traits class defining the types for the Charges class
    using charges_traits = typename parent_type::charges_traits;

    /// Traits class defining the types for the PointCharge class
    using point_charge_traits = typename parent_type::point_charge_traits;

    /// Type used for indexing and offsets
    using size_type = typename parent_type::size_type;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /// No state, so no-op nothrow ctor
    ChargesViewPIMPL() = default;

    /// No state, so just no-op nothrow copy ctor
    ChargesViewPIMPL(const ChargesViewPIMPL&) = default;

    /// Implemented generically by ChargesView
    ChargesViewPIMPL& operator=(const ChargesViewPIMPL&) = delete;

    /// Implemented generically by ChargesView
    ChargesViewPIMPL(ChargesViewPIMPL&&) = delete;

    /// Implemented generically by ChargesView
    ChargesViewPIMPL& operator=(ChargesViewPIMPL&&) = delete;

    /// No throw, default polymorphic dtor
    virtual ~ChargesViewPIMPL() noexcept = default;

    /// Makes a polymorphic deep copy by dispatching to clone_
    pimpl_pointer clone() const { return clone_(); }

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the PointSet piece of *this
     *
     * Every Charges PIMPL contains the data to create a PointSet object. This
     * method is used to access that data as if it were in a PointSet object.
     *
     * @return A view of the piece of *this which acts like a PointSet.
     *
     * @throw None No throw guarantee.
     */
    point_set_reference point_set() { return point_set_(); }

    /** @brief Returns the PointSet piece of *this
     *
     * This is the same as the non-const version except that the resulting
     * view is read-only.
     *
     * @return A view of the piece of *this which acts like a PointSet.
     *
     * @throw None No throw guarantee.
     */
    const_point_set_reference point_set() const { return point_set_(); }

    /// Returns mutable reference to the i-th point charge, implemented by at_
    reference operator[](size_type i) noexcept { return at_(i); }

    /// Makes read-only reference to i-th point charge, uses at_ const
    const_reference operator[](size_type i) const noexcept { return at_(i); }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /// The number of point charges in *this, implemented by size_
    size_type size() const noexcept { return size_(); }

    /// Polymorphic value comparison, implemented by are_equal_
    bool are_equal(const ChargesViewPIMPL& rhs) const noexcept {
        return are_equal_(rhs);
    }

protected:
    /// Will actually implement are_equal_ for the derived class
    template<typename DerivedType>
    bool are_equal_impl_(const ChargesViewPIMPL& other) const noexcept;

    /// Derived class should override to implement clone
    virtual pimpl_pointer clone_() const = 0;

    /// Derived class should override to implement point_set()
    virtual point_set_reference point_set_() = 0;

    /// Derived class should override to implement point_set() const
    virtual const_point_set_reference point_set_() const = 0;

    /// Derived class should overrideto implement mutable at, operator[]
    virtual reference at_(size_type i) noexcept = 0;

    /// Derived class should override to implement read-only at, operator[]
    virtual const_reference at_(size_type i) const noexcept = 0;

    /// Derived class should override to implement size
    virtual size_type size_() const noexcept = 0;

    /// Derived class should implement by calling are_equal_impl_
    virtual bool are_equal_(const ChargesViewPIMPL& rhs) const noexcept = 0;
};

// -----------------------------------------------------------------------------
// -- Out of line definitions
// -----------------------------------------------------------------------------

template<typename ChargesType>
template<typename DerivedType>
bool ChargesViewPIMPL<ChargesType>::are_equal_impl_(
  const ChargesViewPIMPL& other) const noexcept {
    const auto* pthis = dynamic_cast<const DerivedType*>(this);
    assert(pthis != nullptr); // Coding error if this happens
    const auto* pother = dynamic_cast<const DerivedType*>(&other);

    // Same derived type can just delegate to operator==
    if(pother != nullptr) { return (*pthis) == (*pother); }

    // Need to manually check through common API.

    // Short-circuit if different sizes
    if(size() != other.size()) return false;

    // Compare elements in the range [0, size()), n.b. is no-op if size() == 0
    for(size_type i = 0; i < size(); ++i)
        if((*this)[i] != other[i]) return false;

    // Getting here means they're the same
    return true;
}

} // namespace chemist::detail_

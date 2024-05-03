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
#include <chemist/point/point_set_view/point_set_view.hpp>

namespace chemist::detail_ {

/** @brief Establishes the common API for all PIMPLs implementing PointSetView
 *
 *  @tparam PointSetType Type of the PointSet being aliased.
 *
 *  This class is an abstract base class which defines the API for all PIMPLs
 *  that implement PointSetView objects. New PIMPLs are made by inheriting from
 *  this class template.
 */
template<typename PointSetType>
class PointSetViewPIMPL {
private:
    /// Type of this class
    using my_type = PointSetViewPIMPL<PointSetType>;

public:
    /// Type *this implements
    using parent_type = PointSetView<PointSetType>;

    /// Type of pointer the parent uses to hold *this
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    /// Type defining the types associated with a Point in *this
    using point_traits_type = typename parent_type::point_traits_type;

    /// Type used for indexing and offsets
    using size_type = typename parent_type::size_type;

    /// Type of a mutable reference to an element of *this
    using reference = typename parent_type::reference;

    /// Type of a read-only reference to an element in *this
    using const_reference = typename parent_type::const_reference;

    /// No-op default ctor
    PointSetViewPIMPL() = default;

    /// No-op copy ctor (no state to copy)
    PointSetViewPIMPL(const PointSetViewPIMPL&) = default;

    /// Implemented generically by PointSetView
    PointSetViewPIMPL& operator=(const PointSetViewPIMPL&) = delete;

    /// Implemented generically by PointSetView
    PointSetViewPIMPL(PointSetViewPIMPL&&) = delete;

    /// Implemented generically by PointSetView
    PointSetViewPIMPL& operator=(PointSetViewPIMPL&&) = delete;

    /// Default dtor
    virtual ~PointSetViewPIMPL() noexcept = default;

    /// Shallow polymorhic copy
    pimpl_pointer clone() const { return clone_(); }

    /// Returns the number of elements in *this
    size_type size() const noexcept { return size_(); }

    /// Returns a mutable reference to the i-th element in *this
    reference operator[](size_type i) { return at_(i); }

    /// Returns a read-only reference to the i-th element in *this
    const_reference operator[](size_type i) const { return at_(i); }

    /// Polymorphic views alias value equal points
    bool are_equal(const PointSetViewPIMPL& other) const noexcept {
        return are_equal_(other);
    }

protected:
    /// Derived classes should implement are_equal_ by calling this method and
    /// setting DerivedType to their type.
    template<typename DerivedType>
    bool are_equal_impl_(const PointSetViewPIMPL& other) const noexcept;

    /// Dervied class should overwrite to implement are_equal
    virtual bool are_equal_(const PointSetViewPIMPL& other) const noexcept = 0;

    /// Derived class overwrites to implement clone
    virtual pimpl_pointer clone_() const = 0;

    /// Derived class overwrites to implement size
    virtual size_type size_() const noexcept = 0;

    /// Derived class overwrites to implement mutable element access
    virtual reference at_(size_type i) = 0;

    /// Derived class overwrites to implement read-only element access
    virtual const_reference at_(size_type i) const = 0;
};

// -----------------------------------------------------------------------------
// -- Out of line definitions
// -----------------------------------------------------------------------------

template<typename PointSetType>
template<typename DerivedType>
bool PointSetViewPIMPL<PointSetType>::are_equal_impl_(
  const PointSetViewPIMPL& other) const noexcept {
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

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
#include <cassert>
#include <chemist/chemical_system/nucleus/nuclei_view/nuclei_view.hpp>

namespace chemist::detail_ {

/** @brief Defines the API all NucleiView PIMPLs must implement.
 *
 *  To implement a new NucleiView derive from this class and implement:
 *  - pimpl_pointer clone() const
 *  - reference get_nuclei(size_type)
 *  - const_reference get_nuclei(size_type) const
 *  - size_type size() const noexcept
 *  - bool are_equal(const NucleiViewPIMPL&) const noexcept
 */
template<typename NucleiType>
class NucleiViewPIMPL {
private:
    /// Type *this implements
    using parent_type = NucleiView<NucleiType>;

    /// Type of *this
    using my_type = NucleiViewPIMPL<NucleiType>;

public:
    /// Type nuclei_view_type is a view of
    using nuclei_type = typename parent_type::nuclei_type;

    /// Type of a pointer to *this
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    /// Type used to represent a nucleus
    using value_type = typename parent_type::value_type;

    /// Mutable reference to a nucleus
    using reference = typename parent_type::reference;

    /// Read-only reference to a nucleus
    using const_reference = typename parent_type::const_reference;

    /// Class holding traits of the Charges piece of *this
    using charges_traits = typename parent_type::charges_traits;

    /// Class holding traits of the Nucleus objects
    using nucleus_traits = typename parent_type::nucleus_traits;

    /// Type nuclei_view_type uses for indexing
    using size_type = typename parent_type::size_type;

    /// No-op because *this has no state
    NucleiViewPIMPL() noexcept = default;

    /// No-op polymorphic dtor,
    virtual ~NucleiViewPIMPL() noexcept = default;

    /** @brief Makes a polymorphic deep copy of *this.
     *
     *  When called this method will engage the copy ctor of the most derived
     *  class in order to create a deep copy of *this. The copy is returned
     *  polymorphically via a pointer to the base class.
     *
     *  This method is ultimately implemented by calling clone_. The derived
     *  class is responsible for overriding clone_ appropriately.
     *
     *  @return A pointer to a deep copy of *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    pimpl_pointer clone() const { return clone_(); }

    /** @brief Returns a reference to Nucleus @p i.
     *
     *  This function is implemented by calling get_nuke_. The derived class
     is
     *  responsible for overriding get_nuke_ in order to implement this
     *  function.
     *
     *  @param[in] i The offset of the requested nucleus. @p i should be in
     the
     *             range [0, size()).
     *
     *  @return A reference to the requested Nucleus. The Nucleus is mutable.
     *
     *  @throw None This function performs no bounds checks (bounds checks
     are
     *              deferred to the NucleiView class). Passing a value of @p
     i
     *              which is out of bounds will lead to undefined behavior.
     */
    reference get_nuke(size_type i) { return get_nuke_(i); }

    /** @brief Returns a read-only reference to Nucleus @p i.
     *
     *  This method is the same as the non-const version except that it
     *  returns a read-only reference.
     *
     *  @param[in] i The offset of the requested nucleus. Must be in the
     range
     *             [0, size()).
     *
     *  @return A read-only reference to the @p i-th nucleus.
     *
     *  @throw None This function performs no bounds checks (bounds checks
     are
     *              done by the NucleiView class which calls the PIMPL).
     While
     *              passing an out of bounds value for @p i will not raise an
     *              error, it is undefined behavior and the program is likely
     *              to crash.
     */
    const_reference get_nuke(size_type i) const { return get_nuke_(i); }

    /** @brief Determines the number of nuclei in *this
     *
     *  This method is ultimately implemented by overrideing size_.
     *
     *  @return The number of nuclei in *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return size_(); }

    /** @brief Polymorphic value equality.
     *
     *  This method will traverse the class hierarchy of *this ensuring that
     *  @p rhs has the same hierarchy.
     */
    bool are_equal(const NucleiViewPIMPL& rhs) const noexcept {
        return are_equal_(rhs) && rhs.are_equal_(*this);
    }

protected:
    /// Derived class should use to implement are_equal_
    template<typename DerivedType>
    bool are_equal_impl_(const NucleiViewPIMPL& rhs) const noexcept;

    /// Derived class overrides to implement clone
    virtual pimpl_pointer clone_() const = 0;

    /// Derived class overrides to implement get_nuke()
    virtual reference get_nuke_(size_type i) = 0;

    /// Derived class overrides to implement get_nuke() const
    virtual const_reference get_nuke_(size_type i) const = 0;

    /// Derived class overrides to implement size
    virtual size_type size_() const noexcept = 0;

    /// Derived class overrides to implement are_equal
    virtual bool are_equal_(const NucleiViewPIMPL& rhs) const noexcept = 0;
};

// -----------------------------------------------------------------------------
// -- Out of line definitions
// -----------------------------------------------------------------------------

template<typename NucleiType>
template<typename DerivedType>
bool NucleiViewPIMPL<NucleiType>::are_equal_impl_(
  const NucleiViewPIMPL& other) const noexcept {
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
        if(this->get_nuke(i) != other.get_nuke(i)) return false;

    // Getting here means they're the same
    return true;
}

} // namespace chemist::detail_

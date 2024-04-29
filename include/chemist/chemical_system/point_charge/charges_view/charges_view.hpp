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
#include <chemist/chemical_system/point_charge/charges.hpp>
#include <chemist/traits/point_charge_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename ChargesType>
class ChargesViewPIMPL;
}

/** @brief Enables treating state like it's a reference to a Charges object.
 *
 *  @tparam ChargesType The cv-qualified type *this is acting like.
 *
 *  ChargesView objects act like an alias of a Charges object. In practice, the
 *  Charges object need not actually exist and the ChargesView object can
 *  allocate state which is not tied to a Charges object.
 */
template<typename ChargesType>
class ChargesView
  : public utilities::IndexableContainerBase<ChargesView<ChargesType>> {
private:
    /// Type of *this
    using my_type = ChargesView<ChargesType>;

    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    /// Base type of all PIMPLs capable of implementing *this
    using pimpl_type = detail_::ChargesViewPIMPL<ChargesType>;

    /// Type of a pointer to a PIMPL's base piece
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Traits of the Charges object *this aliases
    using charges_traits = ChemistClassTraits<ChargesType>;

    /// Type of an actual mutable reference to a Charges object (not a view)
    using charges_reference = typename charges_traits::reference;

    /// Traits of the PointSet piece of *this
    using point_set_traits = typename charges_traits::point_set_traits;

    /// Type of the PointSet piece of *this
    using point_set_type = typename point_set_traits::value_type;

    /// Type acting like a mutable reference to the PointSet piece of *this
    using point_set_reference = typename point_set_traits::view_type;

    /// Traits of the PointCharge objects in *this
    using point_charge_traits = typename charges_traits::point_charge_traits;

    /// Type of the PointCharge objects in *this
    using value_type = typename point_charge_traits::value_type;

    /// Type acting like a mutable reference to a PointCharge object in *this
    using reference = typename point_charge_traits::view_type;

    /// Type acting like a read-only reference to a PointCharge object in *this
    using const_reference = typename point_charge_traits::const_view_type;

    /// Type of a mutable pointer to a point charge's charge
    using charge_pointer = typename point_charge_traits::charge_pointer;

    /// Unsigned integer type used for indexing and offsets
    using typename base_type::size_type;

    /** @brief Aliases an empty Charges object
     *
     *  Default created ChargesView objects act like they alias empty Charges
     *  objects.
     *
     *  @throw None No throw guarantee
     */
    ChargesView() noexcept;

    /** @brief Creates a ChargesView object which aliases @p charges.
     *
     *  This ctor allows a traditional reference to a Charges object to be used
     *  like it is a ChargesView object.
     *
     *  @param[in] charges The object *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state. Strong
     *                        throw guarantee.
     */
    explicit ChargesView(charges_reference charges);

    /** @brief Creates a new alias to the Charges object aliased by @p other.
     *
     *  This ctor is a shallow copy of the aliased Charges object and a deep
     *  copy of the state in @p other used to alias the Charges object. In other
     *  words after this call *this will alias the same Charges object as
     *  @p other, but *this can be made to alias a different Charges object
     *  without affecting @p other.
     *
     *  @param[in] other The ChargesView being copied.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state. Strong
     *                        throw guarantee.
     *
     */
    ChargesView(const ChargesView& other);

    /** @brief Causes *this to alias the same Charges object as @p rhs.
     *
     *  This method overwrites the state in *this with a copy of @p rhs. See
     *  the copy ctor's description for details on the copy process.
     *
     *  @param[in] rhs The ChargesView being copied.
     *
     *  @return *this after replacing its state with a copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state. Strong
     *                        throw guarantee.
     */
    ChargesView& operator=(const ChargesView& rhs);

    /** @brief Creates a new ChargesView by taking ownership of the state in
     *         @p other
     *
     *  @param[in,out] other The ChargesView object to take the state from.
     *                       After calling this ctor @p other will be in a state
     *                       consistent with aliasing an empty Charges object.
     *
     *  @throw None No throw guarantee.
     */
    ChargesView(ChargesView&& other) noexcept;

    /** @brief Overwrites the state in *this with that in @p rhs.
     *
     *  @param[in,out] rhs The ChargesView object to take the state from.
     *                     After calling this ctor @p rhs will be in a state
     *                     consistent with aliasing an empty Charges object.
     *
     *  @return *this after transferring ownership of the state in @p rhs to
     *          *this.
     *
     *  @throw None No throw guarantee.
     */
    ChargesView& operator=(ChargesView&& rhs) noexcept;

    /// Default no-throw dtor
    ~ChargesView() noexcept;

    /** @brief Determines if *this aliases the same Charges object as @p rhs.
     *
     *  This method will compare the Charges objects aliased by *this and
     *  @p rhs NOT the state used to alias the Charges object. Thinking of
     *  ChargesView as pointer-like that's to say that this method compares the
     *  value being pointed to NOT the addresses of those values.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return True if *this compares equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ChargesView& rhs) const noexcept;

private:
    // Allows base class to access implementations
    friend base_type;

    /// Used internally to check if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// Used internally to wrap the logic associated with cloning the PIMPL
    pimpl_pointer clone_pimpl_() const;

    /// Implements base_type::operator[]() and base_type::at()
    reference at_(size_type i);

    /// Implements base_type::operator[]() const and base_type::at() const
    const_reference at_(size_type i) const;

    /// Implements base_type::size()
    size_type size_() const noexcept;

    /// The actual object holding the state of *this
    pimpl_pointer m_pimpl_;
};

extern template class ChargesView<Charges<float>>;
extern template class ChargesView<const Charges<float>>;
extern template class ChargesView<Charges<double>>;
extern template class ChargesView<const Charges<double>>;

} // namespace chemist

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
#include <chemist/point_charge/charges.hpp>
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

    /// Is @p ChargesType a const-qualified type?
    static constexpr bool am_i_const_v =
      std::is_same_v<const std::remove_cv_t<ChargesType>, ChargesType>;

    /// Used to enable/disable conversions from Charges objects
    template<typename T>
    using enable_if_assign_from_charges_t = std::enable_if_t<
      !am_i_const_v &&
      std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<ChargesType>>>;

public:
    /// Base type of all PIMPLs capable of implementing *this
    using pimpl_type = detail_::ChargesViewPIMPL<ChargesType>;

    /// Type of a pointer to a PIMPL's base piece
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Traits of the Charges object *this aliases
    using charges_traits = ChemistClassTraits<ChargesType>;

    /// Type of the non-cv-qualified Charges object aliased by *this
    using charges_type = typename charges_traits::value_type;

    /// Type of an actual mutable reference to a Charges object (not a view)
    using charges_reference = typename charges_traits::reference;

    /// Type of a read-only reference to a Charges object (not a view)
    using const_charges_reference = typename charges_traits::const_reference;

    /// Traits of the PointSet piece of *this
    using point_set_traits = typename charges_traits::point_set_traits;

    /// Type of the PointSet piece of *this
    using point_set_type = typename point_set_traits::value_type;

    /// Type acting like a mutable reference to the PointSet piece of *this
    using point_set_reference = typename point_set_traits::view_type;

    /// Type acting like a read-only reference to the PointSet piece of *this
    using const_point_set_reference =
      typename point_set_traits::const_view_type;

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
    ChargesView(charges_reference charges);

    /** @brief Creates a ChargesView that aliases @p points and @p pq.
     *
     *  This ctor is used to alias a PointSet that is aliasing its points and a
     *  a set of contiguous charges.
     *
     *  @param[in] points A view of the points.
     *  @param[in] pq A pointer such that `*(pq + i)` is the charge for
     *                `points[i]`.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state. Strong
     *                        throw guarantee.
     */
    ChargesView(point_set_reference points, charge_pointer pq);

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

    /** @brief Assigns a copy of @p other to the aliased Charges object.
     *
     *  @tparam ChargesType2 The type of @p other. Must be Charges or a cv-
     *                      qualified variant of Charges in order to participate
     *                      in overload resolution.
     *  @tparam <anonymous> Used to disable this method via SFINAE if *this is
     *                      not a view of a mutable Charges object or if
     *                      @p ChargesType2 is not a cv-qualified variant of
     *                      Charges.
     *
     *  This method is used to change the state of the Charges object aliased
     *  by *this so that it is a deep copy of the state owned by @p other.
     *
     *  @return *this after updating the state of the aliased Charges object.
     *
     *  @throw std::runtime_error if *this and @p other have different sizes.
     *                            Strong throw guarantee.
     */
    template<typename ChargesType2,
             typename = enable_if_assign_from_charges_t<ChargesType2>>
    ChargesView& operator=(const ChargesType2& other) {
        if(this->size() != other.size())
            throw std::runtime_error("Must be same size");
        for(size_type i = 0; i < other.size(); ++i)
            (*this)[i] = other[i].as_point_charge();
        return *this;
    }

    /// Default no-throw dtor
    ~ChargesView() noexcept;

    /** @brief Returns a mutable reference to the PointSet piece of *this.
     *
     *  Charges objects are ultimately a set of points, each of which also
     *  carries a charge. This method is used to access the PointSet piece of
     *  *this.
     *
     *  @return A mutable reference to the piece of *this which behaves like a
     *          PointSet object.
     *
     *  @throw None No throw guarantee.
     */
    point_set_reference point_set() noexcept;

    /** @brief Returns a read-only reference to the PointSet piece of *this.
     *
     *  This method behaves identical to the non-const version except that the
     *  resulting reference is read-only.
     *
     *  @return A read-only reference to the piece of *this which behaves like a
     *          PointSet object.
     *
     *  @throw None No throw guarantee.
     */
    const_point_set_reference point_set() const noexcept;

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

    /** @brief Compares *this to an actual Charges object.
     *
     *  This method compares the charges object aliased by *this to the one
     *  contained in @p rhs.
     *
     *  @param[in] rhs The object to compare *this to.
     *
     *  @return True if *this aliases an object which compares equal to @p rhs
     *          and false otherwise.
     *
     *  @throw std::bad_alloc if at_() throws when called. Strong throw
     *         guarantee.
     */
    bool operator==(const_charges_reference rhs) const;

    /** @brief Determines if *this is different than an actual Charges object
     *
     *  This method simply negates ChargesView::operator==(Charges).
     *
     *  @param[in] rhs The Charges object to compare to.
     *
     *  @return False if *this aliases an object that is equal to @p rhs and
     *          true otherwise.
     *
     *  @throw std::bad_alloc if operator== throws. Strong throw guarantee.
     */
    bool operator!=(const_charges_reference rhs) const {
        return !(*this == rhs);
    }

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

/** @brief Compares a Charges to a ChargesView for equality.
 *
 *  This is the same method as ChargesView::operator==(const Charges&) except
 *  that the Charges object is on the left side of the ==.
 *
 *  @param[in] lhs The Charges object to compare against the object aliased by
 *                 @p rhs/
 *  @param[in] rhs The ChargesView object to compare against @p lhs.
 *
 *  @return True if @p lhs compares equal to the Charges object aliased by
 *          @p rhs and false otherwise.
 *
 *  @throw std::bad_alloc if ChargeView::operator==(Charges) throws. Strong
 *                        throw guarantee.
 */
template<typename ChargeType1, typename ChargesType2>
bool operator==(const Charges<ChargeType1>& lhs,
                const ChargesView<ChargesType2>& rhs) {
    return rhs == lhs;
}

/** @brief Determines if @p lhs is different than @p rhs
 *
 *  This simply negates the result of operator== with the same input types.
 *
 *  @param[in] lhs The Charges object being compared to the Charges object
 *                 aliased by @p rhs.
 *  @param[in] rhs The view associated with the Charges object to compare to
 *                 @p lhs.
 *
 *  @return False if @p lhs compares equal to the value aliased by @p rhs and
 *          true otherwise.
 *
 *  @throw std::bad_alloc if ChargeView::operator==(Charges) throws. Strong
 *                        throw guarantee.
 */
template<typename ChargeType1, typename ChargesType2>
bool operator!=(const Charges<ChargeType1>& lhs,
                const ChargesView<ChargesType2>& rhs) {
    return !(lhs == rhs);
}

/** @brief Prints a ChargesView object
 *
 *  @tparam ChargesType The type of the Charges object aliased by @p charges.
 *
 *  This method simply loops over the PointChargeView objects in @p charges
 *  and prints them.
 *
 *  @param[in] os The stream to print to.
 *  @param[in] charges The object to print out.
 *
 *  @return Returns @p os after adding @p charges to it.
 *  @throw std::ios_base_failure if anything goes wrong while writing. Weak
 *                               throw guarantee.
 */
template<typename ChargesType>
std::ostream& operator<<(std::ostream& os,
                         const ChargesView<ChargesType>& charges) {
    for(decltype(charges.size()) i = 0; i < charges.size(); i++) {
        os << charges[i] << std::endl;
    }
    return os;
}

extern template class ChargesView<Charges<float>>;
extern template class ChargesView<const Charges<float>>;
extern template class ChargesView<Charges<double>>;
extern template class ChargesView<const Charges<double>>;

} // namespace chemist

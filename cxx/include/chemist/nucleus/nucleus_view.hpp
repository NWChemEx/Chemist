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
#include <chemist/detail_/view_traits.hpp>
#include <chemist/nucleus/nucleus_class.hpp>
#include <chemist/point_charge/point_charge_view.hpp>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

namespace chemist {

/** @brief Allows representing data as if it were a Nucleus object.
 *
 *  NucleusView objects act like references to Nucleus objects. More
 *  specifically, the state inside a NucleusView is an alias of data owned
 *  by another object (usually a Nuclei object or related classes).
 *
 *  @tparam NucleusType The type *this will function as. Assumed to be either
 *                      `Nucleus` or `const Nucleus`.
 */
template<typename NucleusType>
class NucleusView
  : public PointChargeView<std::remove_reference_t<typename ChemistClassTraits<
      NucleusType>::point_charge_traits::reference>> {
private:
    /// Type of *this
    using my_type = NucleusView<NucleusType>;

    /// Type defining the traits for the elements of *this
    using traits_type = ChemistClassTraits<NucleusType>;

    /// Type defining the traits for the charge part of *this
    using point_charge_traits = typename traits_type::point_charge_traits;

    /// Type *this inherits from
    using base_type = PointChargeView<
      std::remove_reference_t<typename point_charge_traits::reference>>;

public:
    // -- Nucleus type--------------------------------------------------------

    /// Type of an un-qualified Nucleus object
    using nucleus_type = typename traits_type::value_type;

    /// Type of a Nucleus reference with const-ness paralleling @p NucleusType
    using nucleus_reference = typename traits_type::reference;

    /// Type of a read-only reference to a Nucleus
    using const_nucleus_reference = typename traits_type::const_reference;

    /// Type of the name
    using name_type = typename traits_type::name_type;

    /// Reference to the name with const-ness paralleling @p NucleusType
    using name_reference = typename traits_type::name_reference;

    /// Type of a read-only reference to the name
    using const_name_reference = typename traits_type::const_name_reference;

    /// Type of a mutable pointer to the name
    using name_pointer = typename traits_type::name_pointer;

    /// Type of the atomic number
    using atomic_number_type = typename traits_type::atomic_number_type;

    /// Reference to atomic number with const-ness paralleling @p NucleusType
    using atomic_number_reference =
      typename traits_type::atomic_number_reference;

    /// Type of a read-only reference to the atomic number
    using const_atomic_number_reference =
      typename traits_type::const_atomic_number_reference;

    /// Type of a mutable pointer to the atomic_number
    using atomic_number_pointer = typename traits_type::atomic_number_pointer;

    /// Type of the nucleus's mass
    using mass_type = typename traits_type::mass_type;

    /// Reference to mass with const-ness paralleling @p NucleusType
    using mass_reference = typename traits_type::mass_reference;

    /// Type of a read-only reference to the mass
    using const_mass_reference = typename traits_type::const_mass_reference;

    /// Pointer to a mutable mass
    using mass_pointer = typename traits_type::mass_pointer;

    /// Type of the nucleus' coordiantes
    using coord_type = typename nucleus_type::coord_type;

    // -- PointCharge types ----------------------------------------------------

    /// Type of PointCharge an object of type @p nucleus_type inherits from
    using point_charge_type = typename point_charge_traits::value_type;

    /// PointChargeView with const-ness paralleling @p NucleusType
    using charge_view_type = typename point_charge_traits::view_type;

    /// Type of a read-only PointChargeView
    using const_charge_view = typename point_charge_traits::const_view_type;

    // -- CTors ----------------------------------------------------------------

    /** @brief Creates a new NucleusView which is a view of @p nuke.
     *
     *  This ctor will create a NucleusView which aliases the state of the
     *  provided Nucleus. The caller is obligated to ensure that the lifetime
     *  of @p nuke exceeds that of the resulting view.
     *
     *  @param[in] nuke The nucleus *this will be a view of.
     *
     *  @throw None No throw guarantee.
     */
    NucleusView(nucleus_reference nuke);

    /** @brief Creates a new NucleusView with the provided state.
     *
     *  This ctor is primarily intended to be called from the Nuclei class.
     *  The inputs to this ctor are state managed by the caller. The resulting
     *  NucleusView will alias the provided state and the caller is responsible
     *  for ensuring the provided state's lifetime exceeds that of the resulting
     *  view.
     *
     *  @param[in] name The name of the nucleus
     *  @param[in] Z The atomic number *this will alias.
     *  @param[in] m The mass *this will alias.
     *  @param[in] q An existing view of a PointCharge on which to build *this.
     *
     *  @throw None No throw guarantee.
     */
    NucleusView(name_reference name, atomic_number_reference Z,
                mass_reference m, charge_view_type q);

    /** @brief Sets the Nucleus aliased by *this to a copy of @p other
     *
     *  This ctor will change the Nucleus object *this aliases so that it is
     *  a copy of @p other.
     *
     *  @tparam NucleusType2 The type of the @p other.
     *  @tparam <Anonymous> This template parameter is used to disable this
     *                      function from consideration when the type of
     *                      @p other is not a Nucleus object with compatible
     *                      cv-qualifiers.
     *
     *  @param[in] other The Nucleus we are aliasing.
     *
     *  @throw None No throw guarantee.
     */
    template<typename NucleusType2, typename = std::enable_if_t<std::is_same_v<
                                      std::decay_t<NucleusType2>, NucleusType>>>
    NucleusView& operator=(NucleusType2&& other) {
        base_type::operator=(std::forward<point_charge_type>(other));
        (*m_pname_) = other.name();
        (*m_pZ_)    = other.Z();
        (*m_pmass_) = other.mass();
        return *this;
    }

    // -- Accessors ------------------------------------------------------------

    /** @brief Provides access to the name.
     *
     *  These methods can be used to retrieve the aliased name. The
     *  non-const method returns a reference whose const-ness parallels that
     *  of @p NucleusType (i.e., if @p NucleusType is const-qualified so too
     *  will be the resulting name).
     *
     *  @return The aliased name by reference.
     *
     *  @throw None No throw guarantee
     */
    ///@{
    name_reference name() noexcept { return *m_pname_; }
    const_name_reference name() const noexcept { return *m_pname_; }
    ///@}

    /** @brief Provides access to the atomic number.
     *
     *  These methods can be used to retrieve the aliased atomic number. The
     *  non-const method returns a reference whose const-ness parallels that
     *  of @p NucleusType (i.e., if @p NucleusType is const-qualified so too
     *  will be the resulting atomic number).
     *
     *  @return The aliased atomic number by reference.
     *
     *  @throw None No throw guarantee
     */
    ///@{
    atomic_number_reference Z() noexcept { return *m_pZ_; }
    const_atomic_number_reference Z() const noexcept { return *m_pZ_; }
    ///@}

    /** @brief Provides access to the mass.
     *
     *  These methods retrieve the aliased mass and return it by reference. The
     *  non-const method returns a reference whose const-ness parallels that
     *  of @p NucleusType (i.e., if @p NucleusType is const-qualified so too
     *  will be the resulting mass).
     *
     *  @return The aliased mass, by reference.
     *
     *  @throw None No throw guarantee.
     */
    /// @{
    mass_reference mass() noexcept { return *m_pmass_; }
    const_mass_reference mass() const noexcept { return *m_pmass_; }
    ///@}

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  These operators compare the state aliased by *this to the respective
     *  state owned (or aliased, depending on the overload) by @p rhs.
     *
     *  @param[in] rhs The Nucleus-like object we are comparing to.
     *
     *  @return True if all state aliased by *this is value equal to the state
     *          owned/aliased by @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    bool operator==(const_nucleus_reference rhs) const noexcept;

    template<typename T>
    bool operator==(const NucleusView<T>& rhs) const noexcept;
    ///@}

    /** @brief Determines if *this is different than @p rhs.
     *
     *  We define "different" as not being value equal. Hence this method
     *  simply negates the result of calling operator==.
     *
     *  @param[in] rhs The Nucleus-like object we are comparing to.
     *
     *  @return False if @p rhs is value equal to *this and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    bool operator!=(const_nucleus_reference rhs) const noexcept {
        return !(*this == rhs);
    }

    template<typename T>
    bool operator!=(const NucleusView<T>& rhs) const noexcept {
        return !(*this == rhs);
    }
    ///@}

    /** @brief Converts *this into a Nucleus object.
     *
     *  This method will deep-copy the state aliased by *this and use it to
     *  create a new Nucleus object.
     *
     *  @return A Nucleus object containing a deep-copy of the state aliased
     *          by *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    nucleus_type as_nucleus() const;

    operator NucleusView<const nucleus_type>() const noexcept;

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @param[in,out] other The NucleusView to exchange state with. After this
     *                       method has been called @p other will contain the
     *                       state which previously was in *this.
     *
     *  @throw None no throw guarantee.
     */
    void swap(NucleusView& other) noexcept;

private:
    /// Pointer to the aliased name
    name_pointer m_pname_;

    /// Pointer to the aliased atomic number
    atomic_number_pointer m_pZ_;

    /// Pointer to the aliased mass
    mass_pointer m_pmass_;
};

/**
 * @relates NucleusView
 * @brief Makes it so the NucleusView class can be printed out.
 *
 * @param os The output stream to print to.
 * @param ni The NucleusView instance to print to the stream.
 * @return The output stream containing the Nucleus instance.
 * @throws std::ios_base::failure if anything goes wrong while writing. Weak
 *         throw guarantee.
 */
template<typename NucleusType>
std::ostream& operator<<(std::ostream& os,
                         const NucleusView<NucleusType>& view);

/// Same as NucleusView::operator==, but when a Nucleus is the LHS
template<typename NucleusType>
bool operator==(const Nucleus& lhs, const NucleusView<NucleusType>& rhs) {
    return rhs == lhs;
}

/// Same as NucleusView::operator!=, but when a Nucleus is the LHS
template<typename NucleusType>
bool operator!=(const Nucleus& lhs, const NucleusView<NucleusType>& rhs) {
    return rhs != lhs;
}

} // namespace chemist

#include "nucleus_view.ipp"

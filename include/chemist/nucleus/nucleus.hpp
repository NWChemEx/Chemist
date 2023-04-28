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
#include <cereal/types/string.hpp>
#include <chemist/point_charge/point_charge.hpp>
#include <string>

namespace chemist {

/** @brief Class representing the (point) nucleus of an atom.
 *
 *  The Nucleus class extends the ``PointCharge`` class by additionally storing
 *  a mass and an atomic number.
 *
 */
class Nucleus : public PointCharge<double> {
public:
    // -- Nucleus types -------------------------------------------------------

    /// A string-like type for storing the name
    using name_type = std::string;

    /// A read/write reference to the name
    using name_reference = name_type&;

    /// A read-only reference to the name
    using const_name_reference = const name_type&;

    /// Integral type used to store the atomic number
    using atomic_number_type = std::size_t;

    /// Type used for read/write references to the atomic number
    using atomic_number_reference = atomic_number_type&;

    /// Type used for read-only references to the atomic number
    using const_atomic_number_reference = const atomic_number_type&;

    /// Floating-point type used to store the mass
    using mass_type = double;

    /// Type used for read/write references to the nucleus's mass
    using mass_reference = mass_type&;

    /// Type for read-only references to the mass
    using const_mass_reference = const mass_type&;

    // -- PointCharge types ----------------------------------------------------

    /// Type of a point charge, i.e., the class *this inherits from
    using point_charge_type = PointCharge<double>;

    /// Type of the point charge's charge
    using charge_type = typename point_charge_type::charge_type;

    // -- Point types ----------------------------------------------------------

    /// Floating-point type of each Cartesian coordinate
    using coord_type = point_charge_type::coord_type;

    /** @brief Creates a new nucleus with the specified properties.
     *
     *  The Nucleus class adds an atomic number and a mass to a point charge.
     *  The provided constructors can respectively be used to create:
     *
     *  1. A defaulted nucleus (all properties set to zero).
     *  2. A nucleus sitting at the origin (charge equal to atomic number).
     *  3. A nucleus sitting at a specified point (charge still equal to
     *     atomic number).
     *  4. A nucleus sitting at a specified point, with a user-spefied charge.
     *
     *  @note A priori users may wonder why overload 4 even exists. At the
     *        moment the main answer is because the class hierarchy naturally
     *        admits it. Theoretically, we would like to expand Chemist to
     *        include unit literals. If we do that, then note that the charge
     *        of the nucleus only equals the atomic number in atomic units;
     *        i.e., in many other unit systems they are not equal and different
     *        methods are needed. More practically, the atomic number is
     *        guaranteed to be an integer and is thus easier to use for
     *        indexing, where as charge is guaranteed to be floating-point and
     *        is thus easier to use in computations.
     *
     *  @param[in] name The name for the nucleus. At present this is the atomic
     *               symbol associated with @p Z and is used only for logging
     *               purposes.
     *  @param[in] Z The atomic number, i.e., Z==1 for hydrogen, Z==2 for
     *               helium, etc. If not specified defaults to 0.
     *  @param[in] m The mass (in atomic units) of the nucleus. If not specified
     *               defaults to 0.0.
     *  @param[in] x The x-coordinate of where the nucleus is centered.
     *               If not specified defaults to the origin.
     *  @param[in] y The y-coordinate of where the nucleus is centered.
     *               If not specified defaults to the orgin.
     *  @param[in] z The z-coordinate of where the nucleus is centered.
     *               If not specefied defaults to the origin.
     *  @param[in] q The charge of the nucleus (in atomic units). If not
     *               specified defaults to @p Z.
     *
     *  @throw std::bad_alloc if the allocating the Point's PIMPL fails. Strong
     *                        throw guarantee.
     */
    ///@{
    Nucleus() : point_charge_type(), m_name_(), m_Z_(0), m_mass_(0.0) {}
    Nucleus(name_type name, atomic_number_type Z, mass_type m);
    Nucleus(name_type name, atomic_number_type Z, mass_type m, coord_type x,
            coord_type y, coord_type z);
    Nucleus(name_type name, atomic_number_type Z, mass_type m, coord_type x,
            coord_type y, coord_type z, charge_type q);
    ///@}

    /** @brief Get/set the name of the nucleus.
     *
     *  While atomic numbers are useful for progmatically identifying a nucleus,
     *  most users would rather see "H" or "He" instead of "Z==1" or "Z==2",
     *  respectively. The name of the nucleus is the string to print when the
     *  user wants to know the identity of the nucleus.
     *
     *  @return A read/write reference to the nucleus's name.
     *
     *  @throw None No throw guarantee.
     */
    name_reference name() noexcept { return m_name_; }

    /** @brief Gets the name of the nucleus.
     *
     *  This method is the same as the non-const version, but the returned
     *  reference is read-only.
     *
     *  @return A read-only reference to the nucleus's name.
     *
     *  @throw None No throw guarantee.
     */
    const_name_reference name() const noexcept { return m_name_; }

    /** @brief Get/set the atomic number of the nucleus
     *
     *  The atomic number of a nucleus is the number of protons in the nucleus.
     *  This method allows you to access the atomic number of the nucleus, in
     *  a mutable state, so that you can do on the spot alchemy (like turning
     *  lead into gold).
     *
     *  @return A read/write reference to the nucleus's atomic number.
     *
     *  @throw None No throw guarantee.
     */
    atomic_number_reference Z() noexcept { return m_Z_; }

    /** @brief Gets the atomic number of the nucleus
     *
     *  This method is the same as the non-const version, but the returned
     *  reference is read-only.
     *
     *  @return A read-only reference to the atomic number.
     *
     *  @throw None No throw guarantee.
     *
     */
    const_atomic_number_reference Z() const noexcept { return m_Z_; }

    /** @brief Get/set the mass of the nucleus.
     *
     *  @return A read/write reference to the nucleus's mass
     *
     *  @throw None No throw guarantee.
     */
    mass_reference mass() noexcept { return m_mass_; }

    /** @brief Get the mass of the nucleus.
     *
     *  @return A read-only reference to the nucleus's mass.
     *
     *  @throw None No throw guarantee.
     */
    const_mass_reference mass() const noexcept { return m_mass_; }

    /** @brief Compares two nuclei for value equality.
     *
     *  Two Nucleus instances are the same if their:
     *  - respective Point base instances compare equal,
     *  - atomic numbers are equal, and
     *  - masses are value equal.
     *
     *  @param[in] rhs The Nucleus being compared to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Nucleus& rhs) const noexcept;

    /** @brief Determines if two nuclei are different.
     *
     *  Two Nucleus instances are different if their value equality operator
     *  returns false.
     *
     *  @param[in] rhs  The Nucleus instance being compared to *this.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Nucleus& rhs) const noexcept;

    /** @brief Serializes *this into @p ar.
     *
     *  @tparam Archive The type of the archive we are serializing *this in to.
     *                  Assumed to be the type of a Cereal OutputArchive object.
     *
     *  @param[in] ar The archive we are serializing in to.
     *
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserializes @p ar into *this
     *
     *  @tparam Archive The type of the archive we are deserializing in to
     *                  *this. Assumed to be the type of a Cereal InputArchive
     *                   object.
     *
     *  @param[in] ar The archive we are deserializing from.
     */
    template<typename Archive>
    void load(Archive& ar);

private:
    /// The nucleus's name
    name_type m_name_;

    /// The nucleus's atomic number
    atomic_number_type m_Z_;

    /// The nucleus's mass (in a.u.)
    mass_type m_mass_;
};

} // namespace chemist

#include "nucleus.ipp"

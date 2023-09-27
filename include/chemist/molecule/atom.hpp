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
#include "chemist/nucleus/nucleus.hpp"
#include <iomanip>
#include <ios>

namespace chemist {

/** @brief A class for holding the details of an atomic unit.
 *
 *  Atoms are considered the first tier of input to a computational chemistry
 *  program.  That is, most types of computational chemistry algorithms need to
 *  know where in space the atoms are located and what their chemical identity
 *  is.
 */
class Atom {
public:
    /// The type of the nucleus
    using nucleus_type = Nucleus;

    /// The type of a read/write reference to the Nucleus
    using nucleus_reference = nucleus_type&;

    /// The type of a read-only reference to the Nucleus
    using const_nucleus_reference = const nucleus_type&;

    /// The integral type used for the atomic number
    using atomic_number_type = nucleus_type::atomic_number_type;

    /// A read/write reference to the atomic number
    using atomic_number_reference = nucleus_type::atomic_number_reference;

    /// A read-only reference to the atomic number
    using const_atomic_number_reference =
      nucleus_type::const_atomic_number_reference;

    /// The floating-point type used to store the mass
    using mass_type = nucleus_type::mass_type;

    /// The type of a read/write reference to the mass
    using mass_reference = nucleus_type::mass_reference;

    /// The type of a read-only reference to the mass
    using const_mass_reference = nucleus_type::const_mass_reference;

    /// The floating-point type of the nuclear charge
    using charge_type = nucleus_type::charge_type;

    /// The type of a read/write reference to the nuclear charge
    using charge_reference = nucleus_type::charge_reference;

    /// The type of a read-only reference to the nuclear charge
    using const_charge_reference = nucleus_type::const_charge_reference;

    /// The floating-point type used to store each coordinate
    using coord_type = nucleus_type::coord_type;

    /// A read/write reference to a coordinate
    using coord_reference = nucleus_type::coord_reference;

    /// A read-only reference to a coordinate
    using const_coord_reference = nucleus_type::const_coord_reference;

    /// A string-like type for storing the name of the Atom instance
    using name_type = typename nucleus_type::name_type;

    /// A read/write reference to the name of the Atom
    using name_reference = typename nucleus_type::name_reference;

    /// A read-only reference to the natom of the Atom
    using const_name_reference = typename nucleus_type::const_name_reference;

    /// The type of a counting number
    using size_type = nucleus_type::size_type;

    /// A read/write reference to a countable
    using size_reference = size_type&;

    /// A read-only reference to a countable
    using const_size_reference = const size_type&;

    /**
     * @brief Makes a default constructed Atom instance.
     *
     * A default constructed Atom is centered at the origin and has no
     * properties.
     *
     * @throw None No throw guarantee.
     */
    Atom() :
      m_nuke_(), n_electrons_(0){} // Explicitly initialize members
    ;

    /**
     * @defgroup Copy/Move CTors and Assignment Operators
     *
     * @brief CTors and assignment operators for transferring the state of an
     *        Atom instance.
     *
     * @param[in] rhs The Atom instance to copy/move.  For move operations
     *            @p rhs is in a valid, but otherwise undefined state.
     *
     * @throw std::bad_alloc The copy ctor/assignment operator throws if
     * there is insufficient memory to perform the copy.
     */
    ///@{
    Atom(const Atom& rhs)     = default;
    Atom(Atom&& rhs) noexcept = default;
    Atom& operator=(const Atom& rhs) = default;
    Atom& operator=(Atom&& rhs) noexcept = default;
    ///@}

    /**
     * @defgroup State-based Ctors
     *
     * @brief CTors for providing the initial state of the Atom instance.
     *
     * @param[in] s The (case-sensitive) atomic symbol for the nucleus.
     * @param[in] Z The number of protons in the nucleus, i.e. the atomic
     *              number.
     * @param[in] m The mass of the nucleus (in a.u.)
     * @param[in] x The x-coordinate where the nucleus is centered (in a.u.)
     * @param[in] y The y-coordinate where the nucleus is centered (in a.u.)
     * @param[in] z The z-coordinate where the nucleus is centered (in a.u.)
     * @param[in] q The charge of the nucleus (in a.u.)
     *
     * @throw std::bad_alloc if the allocation fails. Strong throw guarantee.
     */
    ///@{
    Atom(name_type s, atomic_number_type Z, mass_type m, coord_type x,
         coord_type y, coord_type z);

    Atom(name_type s, atomic_number_type Z, mass_type m, coord_type x,
         coord_type y, coord_type z, charge_type q);

    /** @brief Primary ctor
     *
     *  This ctor allows the caller to set all of the Atom's state directly.
     *
     * @param[in] s The (case-sensitive) atomic symbol for the nucleus.
     * @param[in] Z The number of protons in the nucleus, i.e. the atomic
     *              number.
     * @param[in] m The mass of the nucleus (in a.u.)
     * @param[in] x The x-coordinate where the nucleus is centered (in a.u.)
     * @param[in] y The y-coordinate where the nucleus is centered (in a.u.)
     * @param[in] z The z-coordinate where the nucleus is centered (in a.u.)
     * @param[in] q The charge of the nucleus (in a.u.)
     * @param[in] n_electrons The number of electrons in the atom (defaults to
     * Z)
     *
     * @throw std::bad_alloc if the allocation fails. Strong throw guarantee.
     */
    Atom(name_type s, atomic_number_type Z, mass_type m, coord_type x,
         coord_type y, coord_type z, charge_type q, size_type n_electrons);
    ///@}

    /// Default dtor
    ~Atom() noexcept = default;

    /**
     * @defgroup Getters/Setters
     *
     * @brief Returns the method's namesake property
     *
     * The returned value is read/write for non-const Atom instances and
     * read-only for const instances.
     *
     * @return The requested value, by mutable or read-only reference.
     *
     * @throw None. No throw guarantee.
     */
    ///@{
    name_reference name() noexcept { return nucleus().name(); }
    const_name_reference name() const noexcept { return nucleus().name(); }

    nucleus_reference nucleus() noexcept { return m_nuke_; }
    const_nucleus_reference nucleus() const noexcept { return m_nuke_; }

    atomic_number_reference Z() noexcept { return nucleus().Z(); }
    const_atomic_number_reference Z() const noexcept { return nucleus().Z(); }

    mass_reference mass() noexcept { return nucleus().mass(); }
    const_mass_reference mass() const noexcept { return nucleus().mass(); }

    charge_reference nuclear_charge() noexcept { return nucleus().charge(); }
    const_charge_reference nuclear_charge() const noexcept;

    size_reference n_electrons() noexcept { return n_electrons_; }
    const_size_reference n_electrons() const noexcept { return n_electrons_; }
    ///@}

    /** @brief Returns the net charge of the atom.
     *
     * The net charge (in atomic units) on the atom is defined as the
     * nuclear charge (also in atomic units) minus the number of electrons.
     * For example, a helium atom (nuclear charge of +2.0) with 3 electrons,
     * has a charge of -1.0.
     *
     * @return The net charge of the atom.
     *
     * @throw None No throw guarantee.
     * @return The net charge of the atom.
     */
    charge_type charge() const noexcept {
        return nucleus().charge() - n_electrons_;
    }

    /** @brief Nuclear coordinate getter/setters
     *
     *  @param[in] q Used to request the q-th component of the nuclear position,
     *               *i.e.*, q==0 returns the x component, q == 1 the y, etc.
     *
     *  @return A mutable reference if *this is non-const, and a read-only
     *          reference if *this is const.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3). Strong
     *                           throw guarantee.
     */
    ///@{
    coord_reference coord(size_type q) { return nucleus().coord(q); }
    const_coord_reference coord(size_type q) const;

    coord_reference x() { return coord(0); }
    const_coord_reference x() const { return coord(0); }

    coord_reference y() { return coord(1); }
    const_coord_reference y() const { return coord(1); }

    coord_reference z() { return coord(2); }
    const_coord_reference z() const { return coord(2); }
    ///@}

    /** @brief Serializes the atom.
     *
     *  @param[in,out] ar The archive instance being used for serialization.
     *                    After this call @p ar will contain this instance's
     *                    serialized state.
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserializes the atom.
     *
     *  @param[in,out] ar The archive instance which contains the serialized
     *                    state for this instance. After this call @p ar will
     *                    no longer contain this instance's serialized state.
     */
    template<typename Archive>
    void load(Archive& ar);

private:
    /// The atom's nucleus
    nucleus_type m_nuke_;

    /// Number of electrons
    size_type m_n_electrons_ = 0;

}; // End Atom

/**
 * @defgroup Atom comparison operators
 * @relates Atom
 * @brief Allows one to compare two atom instances for exact equality.
 *
 * Two atom instances are defined as equal if their nuclei are identical and the
 * number of electrons are the same. *N.B* that floating-point comparisons are
 * bit-wise with zero tolerance for deviation, *i.e.*, 1.99999999999999
 * != 2.00000000000000
 *
 * @param[in] lhs The Atom instance on the left of the equivalence operation
 * @param[in] rhs The Atom instance on the right of the equivalence
 * operation
 * @return Whether the two atoms obey the requested equivalence relation.
 *
 * @throw none all comparisons are no throw guarantee.
 */
///@{
bool operator==(const Atom& lhs, const Atom& rhs) noexcept;

inline bool operator!=(const Atom& lhs, const Atom& rhs) noexcept {
    return !(lhs == rhs);
}
///@}

/**
 * @relates Atom
 * @brief Makes it so the Atom class can be printed out.
 *
 * @param os The output stream to print to.
 * @param ai The Atom instance to print to the stream.
 * @return The output stream containing the atom instance.
 * @throws std::ios_base::failure if anything goes wrong while writing. Weak
 *         throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const Atom& ai);

} // namespace chemist

#include "atom.ipp"

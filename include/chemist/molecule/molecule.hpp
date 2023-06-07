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
#include <chemist/molecule/atom.hpp>
#include <chemist/nucleus/nuclei.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
/// Forward declaration of class that holds the molecule's implementation
class MolPIMPL;
} // namespace detail_

/** @brief Represents a set of atoms.
 *
 *  When atoms come together they become a molecule. Within the Born-Oppenheimer
 *  picture of quantum chemistry, the nuclei of the molecule remain as distinct
 *  points, but the electrons delocalize across multiple atoms. The point is,
 *  that once atoms have been combined to form a molecule we can no longer, in
 *  general, associate electrons with a specific atom. This class models this
 *  by having Atom instances be the inputs, but users of the Molecule can only
 *  get back Nucleus instances or total electronic properties (charge and
 *  multiplicity).
 *
 *  @note While our approach of "dissolving" the atoms upon insertion may be
 *        atypical, note that this is entirely consistent with traditional
 *        inputs (charge, multiplicity, nuclear identities, and Cartesian
 *        coordinates of the nuclei).
 */
class Molecule : public utilities::IndexableContainerBase<Molecule> {
private:
    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<Molecule>;

public:
    /// The type of the PIMPL
    using pimpl_type = detail_::MolPIMPL;

    /// The type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of an atom contained in this class
    using atom_type = Atom;

    /// Type of an initializer list filled with Atom objects
    using atom_initializer_list = std::initializer_list<atom_type>;

    /// Type representing the set of nuclei
    using nuclei_type = Nuclei;

    /// Type of a reference to the set of nuclei
    using nuclei_reference = nuclei_type&;

    /// Type of a read-only reference to the set of nuclei
    using const_nuclei_reference = const nuclei_type&;

    /// Type of a nucleus
    using value_type = typename nuclei_type::value_type;

    /// Type of a read/write reference to a nucleus
    using reference = typename nuclei_type::reference;

    /// Type of a read-only reference to a nucleus
    using const_reference = typename nuclei_type::const_reference;

    /// Type used to count things (like electrons and multiplicities)
    using size_type = typename nuclei_type::size_type;

    /// Signed-integral type used to hold the molecule's charge
    using charge_type = int;

    /** @brief Makes a molecule with no nuclei, no charge, and a multiplicity
     *         of 1
     *
     *  The default constructed molecule contains no nuclei or electrons. It
     *  thus has a charge of 0 and a total spin of 0 (*i.e.*, a multiplicity of
     *  1).
     *
     *  @throw None No throw guarantee.
     */
    Molecule() noexcept;

    /** @brief Creates a new Molecule which is a deep-copy of @p rhs.
     *
     *  @param[in] rhs The instance we are deep copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory for
     *                        the copied state. Strong throw guarantee.
     */
    Molecule(const Molecule& rhs);

    /** @brief Create a new Molecule by taking the state from @p rhs.
     *
     *  @param[in,out] rhs The object we are taking the state of. After this
     *                     call @p rhs will be in a state consistent with
     *                     default initialization.
     *
     *  @throw None No throw guarantee.
     */
    Molecule(Molecule&& rhs) noexcept;

    /** @brief Overrides the state in *this with a deep-copy of @p rhs.
     *
     *  This method will release the state currently held by *this and then
     *  replace it with a deep copy of the state in @p rhs.
     *
     *  @param[in] rhs The object we are copying.
     *
     *  @return *this after replacing its state with a deep-copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the internal
     *                        state of *this.
     */
    Molecule& operator=(const Molecule& rhs);

    /** @brief Overrides the state in *this with the state in @p rhs.
     *
     *  @param[in,out] rhs The object we are taking the state from. After this
     *                     operation @p rhs will be in a state consistent with
     *                     default initialization.
     *
     *  @throw None No throw guarantee.
     */
    Molecule& operator=(Molecule&& rhs) noexcept;

    /** @brief Initializes the Molecule with the provided atoms.
     *
     *  This ctor is primarily used to initialize Molecule objects for tests
     *  and tutorials. The resulting Molecule will have a charge based on the
     *  number of electrons assigned to each atom in @p atoms, and the
     *  multiplicity will be set to 1 if the number of electrons is even and
     *  2 if the number of electrons are odd.
     *
     *  @param[in] atoms The initial atoms to occupy this molecule.
     *
     *  @throw std::bad_alloc if allocating the state for *this fails. Strong
     *                        throw guarantee.
     */
    Molecule(atom_initializer_list atoms);

    /** @brief Creates a Molecule with the specified charge and
     *         multiplicity.
     *
     *  This ctor can be used to set the charge and multiplicity of the
     *  Molecule, in addition to specifying the initial set of atoms. The charge
     *  and multiplicity provided to this constructor will override whatever
     *  charge is worked out from the number of electrons in @p atoms.
     *
     *  @param[in] charge The charge the newly created molecule should have.
     *  @param[in] multiplicity The multiplicity the created molecule should
     *                          have.
     *  @param[in] atoms The set of atoms to initialize *this to.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    Molecule(charge_type charge, size_type multiplicity,
             atom_initializer_list atoms);

    /** @brief Constructs a molecule from a set of nuclei.
     *
     *  @param[in] charge The net charge of the molecule. The negative of the
     *                    net number of electrons.
     *  @param[in] multiplicity This is 2S+1, where S is the total spin of the
     *                          electrons.
     *  @param[in] nuclei This is the nuclear framework in which the electrons
     *                    move about.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    Molecule(charge_type charge, size_type multiplicity, nuclei_type nuclei);

    /// Default dtor
    ~Molecule() noexcept;

    /** @brief Function used to add an additional atom to the molecule
     *
     * Given a molecule that contains @f$N@f$ atoms, this will make the provided
     * atom the @f$N+1@f$-th atom, which resides at index @f$N@f$.
     *
     * @note The caller is responsible for updating the multiplicity of *this
     *       if needed.
     *
     * @param value the atom to add to the molecule.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *                       PIMPL (if *this does not already have one) or to
     *                       copy @p value.  Strong throw guarantee.
     */
    void push_back(atom_type value);

    /** @brief Provides access to the set of nuclei.
     *
     *  In the typical quantum chemistry approximations, a molecule is comprised
     *  of a nuclear framework comprised of classical point charges and
     *  electrons that move about in that framework. This method allows the
     *  user to directly interact with the nuclei.
     *
     *  Note that adding/removing nuclei through this method will NOT update
     *  the number of electrons or multiplicity of the molecule (after all
     *  you're only adding nuclei). Thus if the user is really trying to add
     *  atoms (nucleus + electrons) use `push_back` (or similar mechanisms).
     *
     *  @return A read/write reference to the nuclear framework.
     *
     *  @throw std::bad_alloc if *this has no PIMPL and allocating a PIMPL
     *                        fails. Strong throw guarantee. If *this has a
     *                        PIMPL this method is no throw guarantee.
     */
    nuclei_reference nuclei();

    /** @brief Read-only access to the set of nuclei.
     *
     *  This method is the same as th non-const version, except that the
     *  returned set is read-only.
     *
     *  @throw std::runtime_error if *this has no PIMPL. Strong throw guarantee.
     *                            If *this has a PIMPL, this method is no-throw
     *                            guarantee.
     */
    const_nuclei_reference nuclei() const;

    /** @brief The number of electrons in this molecule.
     *
     *  Each Atom instance provided to *this also provides electrons (by default
     *  the number of which is equal to the atomic number, but this can be
     *  configured on a per atom-basis by the user if need be). Internally,
     *  we store the charge of the molecule and adjust it as atoms are added
     *  (or when the user calls set_charge). This method will return the sum
     *  of each atom's atomic number less the charge.
     *
     *  @return The number of electrons which have been added to *this,
     *          adjusting for the charge of the system.
     *
     *  @throw None No throw guarantee.
     */
    size_type n_electrons() const noexcept;

    /** @brief The charge of the molecule (in a.u.)
     *
     *  The charge of the molecule is the sum of each nucleus's atomic number
     *  less the total number of electrons. A positive charge indicates a net
     *  loss in electrons, whereas a negative charge indicates a net excess of
     *  electrons (relative to neutrality).
     *
     *  @return The charge (in a.u.) of *this
     *
     *  @throw None No throw guarantee.
     */
    charge_type charge() const noexcept;

    /** @brief Used to manually set the number of electrons.
     *
     *  Adding Atom instances changes the number of electrons. Sometimes we
     *  just know that a system is electron-rich or electron-deficient, but
     *  do not know which atom(s) is actually associated with the electron
     *  difference. This method can be called to manually override the
     *  number of electrons.
     *
     *  @note Setting the charge will also default the multiplicity (1 if the
     *        new number of electrons is even, and 2 if the new number is odd).
     *        This is done to avoid having *this be in a bad state. If you want
     *        a different multiplicity, follow the call to set_charge with a
     *        call to set_multiplicity.
     *
     *  @param[in] n The number of electrons the caller wants *this to have.
     *               The charge must be less than the sum of the currently
     *               stored atoms' atomic numbers.
     *
     *  @note It's best to call this after adding all atoms you want to add, as
     *        adding another atom will update the charge based on the charge of
     *        that atom.
     *
     *  @throw std::bad_alloc if there is no PIMPL and allocating one fails.
     *                        If an error is raised, it is done with a strong
     *                        guarantee. If *this already has a PIMPL this is
     *                        no-throw guarantee.
     */
    void set_charge(charge_type n);

    /** @brief The multiplicity of *this.
     *
     *  The multiplicity of a molecule is @f$2S+1@f$ where @f$S@f$ is the total
     *  spin. Note that an empty molecule has 0 electrons, and thus has a total
     *  spin of 0 and a multiplicity of 1.
     *
     *  @return The multiplicity of *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type multiplicity() const noexcept;

    /** @brief Used to override the multiplicity.
     *
     *  This method will set the multiplicity of *this to @p mult.
     *
     *  @note Calling this method will NOT change the charge/number of
     *        electrons. You must have the charge/number of electrons set
     *        correctly prior to calling this method.
     *
     *  @param[in] mult The multiplicity the user wants *this to have.
     *
     *  @throw std::bad_alloc if *this has no PIMPL, and allocating a new PIMPL
     *                        fails. Strong throw guarantee.
     */
    void set_multiplicity(size_type mult);

    /** @brief Serialize Molecule instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserialize for Molecule instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar);

private:
    /// Lets the base class implement the container API
    friend base_type;

    /// Used by the base to implement read/write element access
    reference at_(size_type i) noexcept;

    /// Used by the base to implement read-only element access
    const_reference at_(size_type i) const noexcept;

    /// Used by the base to implement size
    size_type size_() const noexcept;

    /// Code factorization for creating a new, empty PIMPL
    static pimpl_pointer make_pimpl_();

    /// Code factorization for checking if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /// Code factorization for throwing if *this does not have a PIMPL
    void assert_pimpl_() const;

    /// Code factorization for setting the multiplicity after modifying *this
    void set_multiplicity_();

    /// Code factorization for setting the charge after modifying *this
    void set_charge_();

    /// The object actually implementing the Molecule class
    pimpl_pointer m_pimpl_;
};

/**
 * @relates Molecule
 * @brief Overloads the stream insertion operator so that molecules can be
 *        printed.
 *
 * @param os The ostream instance to write to.
 * @param mol The Molecule instance to write to the stream.
 * @return The ostream instance containing the text representation of the
 *         molecule.
 * @throws std::ios_base::failure if anything goes wrong while writing.  Weak
 *         throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const Molecule& mol);

/**
 * @defgroup Molecule comparison operators
 * @relates Molecule
 * @brief Allows one to compare two molecule instances for exact equality.
 *
 * Two molecule instances are defined as equal if they have the same charge,
 * multiplicity, and atoms (w.r.t. atomic number, atomic mass, atomic
 * coordinates, and index within the atom-holding container). *N.B* that
 * floating-point comparisons are bit-wise with zero tolerance for deviation,
 * *i.e.*, 1.99999999999999 != 2.00000000000000
 *
 * @param[in] lhs The Molecule instance on the left of the equivalence operation
 * @param[in] rhs The Molecule instance on the right of the equivalence
 * operation
 * @return Whether the two molecules obey the requested equivalence relation.
 *
 * @throw none all comparisons are no throw guarantee.
 */
///@{
bool operator==(const Molecule& lhs, const Molecule& rhs) noexcept;
inline bool operator!=(const Molecule& lhs, const Molecule& rhs) noexcept {
    return !(lhs == rhs);
}
///@}

// -- Inline Implementations ---------------------------------------------------

template<typename Archive>
void Molecule::save(Archive& ar) const {
    ar& has_pimpl_();
    if(has_pimpl_()) {
        ar& charge();
        ar& multiplicity();
        ar& nuclei();
    }
}

template<typename Archive>
void Molecule::load(Archive& ar) {
    bool has_pimpl = false;
    ar& has_pimpl;
    if(has_pimpl) {
        charge_type q;
        size_type m;
        ar& q;
        ar& m;
        ar& nuclei();
        set_charge(q);
        set_multiplicity(m);
    }
}

} // end namespace chemist

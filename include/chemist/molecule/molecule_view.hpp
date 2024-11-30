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
#include <chemist/molecule/molecule_class.hpp>
#include <chemist/traits/molecule_traits.hpp>
#include <memory>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename MoleculeType>
class MoleculeViewPIMPL;

}

/** @brief Allows existing molecular state to be used as if it were a Molecule
 *         object.
 *
 *  @tparam MoleculeType The type of molecule that *this will alias.
 *                       @p MoleculeType is expected to be either Molecule or
 *                       const Molecule.
 */
template<typename MoleculeType>
class MoleculeView
  : public utilities::IndexableContainerBase<MoleculeView<MoleculeType>> {
private:
    /// Type of *this
    using my_type = MoleculeView<MoleculeType>;

    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<my_type>;

    /// Type trait to work out if @p T is cv-qualified
    template<typename T>
    static constexpr bool is_cv_v = !std::is_same_v<T, std::remove_cv_t<T>>;

    /// Enables a function if *this is aliasing const Molecule and @p T is
    /// mutable
    template<typename T>
    using enable_read_only_conversion_t =
      std::enable_if_t<is_cv_v<MoleculeType> && !is_cv_v<T>>;

    template<typename T>
    using disable_if_read_only_t =
      std::enable_if_t<!is_cv_v<T> && std::is_same_v<T, MoleculeType>>;

    /**
     * We allow assigning values to the aliased state when:
     * - *this is mutable
     * - the object to assign from is a Molecule object (with arbitrary
     *   cv-qualifiers)
     */
    template<typename T>
    using enable_assign_from_molecule_t = std::enable_if_t<
      !is_cv_v<MoleculeType> &&
      std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<MoleculeType>>>;

public:
    /// Type of the object implementing *this
    using pimpl_type = detail_::MoleculeViewPIMPL<MoleculeType>;

    /// Type of a pointer to the object implementing *this
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of the struct which defines the types for *this
    using traits_type = ChemistClassTraits<MoleculeType>;

    /// Type of the struct which defines the types for Atom objects in *this
    using atom_traits = typename traits_type::atom_traits;

    /// Type of the struct defining types for the Nuclei piece of *this
    using nuclei_traits = typename traits_type::nuclei_traits;

    /// Type of the struct defining types for the electrons
    using many_electrons_traits = typename traits_type::many_electrons_traits;

    /// Non-cv-qualified type of the Molecule object *this aliases
    using molecule_type = typename traits_type::value_type;

    /// Type of a reference to a Molecule object
    using molecule_reference = typename traits_type::reference;

    /// Type of a mutable reference to a Nuclei object
    using nuclei_reference = typename nuclei_traits::view_type;

    /// Type of a read-only reference to a Nuclei object
    using const_nuclei_reference = typename nuclei_traits::const_view_type;

    /// Type of the object holding the electronic part of *this
    using many_electrons_type = typename many_electrons_traits::value_type;

    /// Type of a nucleus object
    using value_type = typename atom_traits::nucleus_traits::value_type;

    /// Type used to hold the multiplicity
    using multiplicity_type = typename atom_traits::multiplicity_type;

    /// Type of a pointer to the multiplicity of a Molecule
    using multiplicity_pointer = typename atom_traits::multiplicity_pointer;

    /// Type of a read-only pointer to a Molecule's multiplicity
    using const_multiplicity_pointer =
      typename atom_traits::const_multiplicity_pointer;

    /// Type of the molecule's charge
    using charge_type = typename traits_type::charge_type;

    /// Type of a pointer to the molecule's charge
    using charge_pointer = typename traits_type::charge_pointer;

    /// Type of a read-only pointer to the aliased molecule's charge
    using const_charge_pointer = typename traits_type::const_charge_pointer;

    /// Type used for indexing and offsets
    using size_type = typename base_type::size_type;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Aliases an empty Molecule object.
     *
     *  This method creates a view which aliases an empty Molecule object.
     *
     *  @throw None No throw guarantee.
     */
    MoleculeView() noexcept;

    /** @brief Creates a view of the Molecule object @p mol.
     *
     *  This ctor dispatches to the pointer ctor. See the documentation for the
     *  pointer ctor for more information.
     *
     *  @param[in] mol A mutable reference to a Molecule object. Note this ctor
     *                 is only used for references to Molecule objects, not
     *                 views.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    MoleculeView(molecule_reference mol);

    /** @brief Creates a view of a Molecule given pointers to its state.
     *
     *  This ctor takes in a reference to a set of Nuclei and pointers to
     *  the charge and multiplicity for the aliased Molecule.
     *
     *  @param[in] nuclei The nuclear framework the electrons are moving around
     *                    in.
     *  @param[in] pcharge A pointer to the Molecule's charge.
     *  @param[in] pmultiplicity A pointer to the Molecule's multiplicity
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    MoleculeView(nuclei_reference nuclei, charge_pointer pcharge,
                 multiplicity_pointer pmultiplicity);

    /** @brief Implicit conversion from a mutable view to a read-only view.
     *
     *  @tparam MoleculeType2 The type of Molecule object aliased by @p other.
     *                        For this ctor to participate in overload
     *                        resolution @p MoleculeType2 must be Molecule.
     *  @tparam <anonymous> Used to disable this ctor via SFINAE if *this is
     *                      not a view of a read-only Molecule object and/or if
     *                      @p other is not a view of a mutable Molecule object.
     *
     *  This ctor is used to convert from a view of a mutable Molecule to a
     *  view of a read-only Molecule. The conversion is akin to the conversion
     *  from `T&` to `const T&` and is allowed to happen implicitly.
     *
     *  @param[in] other The MoleculeView we are converting from.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    template<typename MoleculeType2,
             typename = enable_read_only_conversion_t<MoleculeType2>>
    MoleculeView(const MoleculeView<MoleculeType2>& other) :
      // N.B. this is okay b/c other is const
      MoleculeView(other.nuclei(), other.charge_data(),
                   other.multiplicity_data()) {}

    /** @brief Creates a new view of the object aliased by @p other.
     *
     *  This copy ctor will create new instances of the state needed to alias
     *  a Molecule object and then point that state at the same Molecule
     * aliased by @p other. In other words, the resulting view will alias the
     * same Molecule as @p other, but pointing the resulting view at a
     * different Molecule object will not affect @p other.
     *
     *  @param[in] other The view whose Molecule *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     * Strong throw guarantee.
     */
    MoleculeView(const MoleculeView& other);

    /** @brief Initializes *this with the state in @p other.
     *
     *  This ctor will take the state inside @p other and give it to *this.
     *  Thus, after this call *this will alias the same Molecule as @p other.
     *
     *  @param[in,out] other The object to take the state from. After this call
     *                       @p other will be in a valid, but otherwise
     *                       undefined state.
     *
     *  @throw None No throw guarantee.
     */
    MoleculeView(MoleculeView&& other) noexcept;

    /** @brief Sets the aliased state so it is value equal to @p rhs.
     *
     *  This method modifies the aliased state so that it is value equal to
     *  the state of @p rhs. It does NOT allocate new state.
     *
     *  @param[in] rhs The Molecule we want the aliased Molecule to be value
     *                 equal to.
     *
     *  @throw std::runtime_error if the Molecule aliased by *this does not have
     *                            the same number of atoms as @p rhs. Strong
     *                            throw guarantee.
     */
    template<typename MoleculeType2,
             typename = enable_assign_from_molecule_t<MoleculeType2>>
    MoleculeView& operator=(const MoleculeType2& rhs) {
        // N.B. this assigns the VALUES in the Nuclei object
        nuclei() = rhs.nuclei().as_nuclei();
        set_charge(rhs.charge());
        set_multiplicity(rhs.multiplicity());
        return *this;
    }

    /** @brief Replaces the state in *this with a copy of the state in @p rhs.
     *
     *  This method will release the state currently held by *this and replace
     *  it with a copy (in the copy ctor sense of the word) of the state in
     *  @p rhs.
     *
     *  @param[in] rhs The object whose state will be copied.
     *
     *  @return *this after setting its state to a copy of the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    MoleculeView& operator=(const MoleculeView& rhs);

    /** @brief Replaces the state in *this with the state in @p rhs.
     *
     *  This method will release the state currently held by *this and replace
     *  it with the state currently held by @p rhs.
     *
     *  @param[in,out] rhs The object whose state will be taken. After this call
     *                     @p rhs will be in a valid, but otherwise undefined
     *                     state.
     *
     *  @return *this after taking the state of @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    MoleculeView& operator=(MoleculeView&& rhs) noexcept;

    /// Defaulted no-throw dtor
    ~MoleculeView() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters and setters
    // -------------------------------------------------------------------------

    /** @brief Provides access to the nuclear framework.
     *
     *  @return A mutable reference to the nuclei within *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return
     *                        value. Strong throw guarantee.
     */
    nuclei_reference nuclei();

    /** @brief Provides access to the nuclear framework
     *
     *  This method is the same as the non-const version except that the
     *  resulting reference is read-only.
     *
     *  @return A read-only reference to the nuclei within *this
     *
     *  @throw std::bad_alloc if there is a problem allocating the return
     *                        value. Strong throw guarantee.
     *
     */
    const_nuclei_reference nuclei() const;

    /** @brief Returns the electronic piece of *this.
     *
     *  This method can be used to retrieve the electrons of *this.braket
     *
     *  @return An object which contains the electronic piece of *this.
     *
     *  @throw None No throw guarantee.
     */
    many_electrons_type electrons() const {
        return many_electrons_type(n_electrons());
    }

    /** @brief How many electrons does the aliased Molecule have?
     *
     *  This method will determine the number of electrons in the aliased
     *  Molecule. This will be the sum of the atomic numbers of the nuclei less
     *  the charge (n.b. a negative charge means extra electrons).
     *
     *  @return The number of electrons in the aliased Molecule.
     *
     *  @throw None No throw guarantee.
     */
    size_type n_electrons() const noexcept;

    /** @brief Returns the electronic charge of *this.
     *
     *  This method returns the difference between the sum of the atomic
     *  numbers in this->nuclei() and the number of electrons.
     *
     *  @return The electronic charge of *this.
     *
     *  @throw None No throw guarantee.
     */
    charge_type charge() const noexcept;

    /** @brief Sets the charge of the aliased Molecule.
     *
     *  @tparam MoleculeType2 The type of the Molecule aliased by *this. This
     *                        is a template type parameter of this function so
     *                        we can disable the function. The user should
     *                        ignore the template type parameters for this
     *                        method.
     *  @tparam <anonymous> Used to disable this method via SFINAE when
     *                      @p MoleculeType2 does not equal @p MoleculeType and
     *                      when @p MoleculeType is read-only.
     *
     *
     *  @param[in] charge The new value for the Molecule's total electronic
     *                    charge.
     *
     *  @throw std::runtime_error if the user attempts to set the charge to a
     *                            non-physical value. Strong throw guarantee.
     */
    template<typename MoleculeType2 = MoleculeType,
             typename               = disable_if_read_only_t<MoleculeType2>>
    void set_charge(charge_type charge);

    /** @brief Returns the multiplicity associated with *this.
     *
     *  The multiplicity of a Molecule is @f$2S+1@f$ where @f$S@f$ is the
     *  total spin quantum number. Note that if *this has no electrons,
     *  @f$S@fZ$ is 0 and the multiplicity is 1.
     *
     *  @return The multiplicity associated with *this.
     *
     *  @throw None No throw guarantee.
     */
    multiplicity_type multiplicity() const noexcept;

    /** @brief Sets the multiplicity of the aliased Molecule
     *
     *  @tparam MoleculeType2 The type of the Molecule aliased by *this. This
     *                        is a template type parameter of this function so
     *                        we can disable the function. The user should
     *                        ignore the template type parameters for this
     *                        method.
     *  @tparam <anonymous> Used to disable this method via SFINAE when
     *                      @p MoleculeType2 does not equal @p MoleculeType and
     *                      when @p MoleculeType is read-only.
     *
     *  @note Users should set the number of electrons before setting the
     *        multiplicity as the number of electrons is used to determine the
     *        allowed value of the multiplicity.
     *
     *  @param[in] multiplicity The value to set the multiplicity to.
     *
     *  @throw std::runtime_error if the user attempts to set the multiplicity
     *                            value to a non-physical value. Strong throw
     *                            guarantee.
     *
     */
    template<typename MoleculeType2 = MoleculeType,
             typename               = disable_if_read_only_t<MoleculeType2>>
    void set_multiplicity(multiplicity_type multiplicity);

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Converts *this into a Molecule object.
     *
     *  MoleculeView objects alias their state whereas Molecule objects own
     *  their state. This method will deep copy the state owned by *this into
     *  a newly created Molecule object.
     *
     *  @return A Molecule object which is a deep copy of the state aliased by
     *          *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    molecule_type as_molecule() const;

    /** @brief Returns the address where the charge is stored.
     *
     *  @return A read-only pointer to the charge. If *this is an empty
     *          Molecule then this method returns a nullptr.
     *
     *  @throw None No throw guarantee.
     */
    const_charge_pointer charge_data() const noexcept;

    /** @brief Returns the address where the multiplicity is stored.
     *
     *  @return A read-only pointer to the multiplicity of *this.
     *
     *  @throw None No throw guarantee.
     */
    const_multiplicity_pointer multiplicity_data() const noexcept;

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *
     *  @param[in,out] other The object *this will take its state from. After
     *                       this method @p other will contain the state which
     *                       was previously in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(MoleculeView& other) noexcept;

    /** @brief Determines if *this and @p rhs both alias value equal objects.
     *
     *  This method will compare the Molecule aliased by *this to the Molecule
     *  aliased by @p rhs. In particular this method does NOT determine if
     *  *this aliases the same instance as @p rhs.
     *
     *  @param[in] rhs The view to compare to.
     *
     *  @return True if the Molecule aliased by *this and the one aliased by
     *          @p rhs compare value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const MoleculeView& rhs) const noexcept;

    /** @brief Determines if *this is different than @p rhs.
     *
     *  This method defines different as not value equal. See operator== for
     *  the definition of value equal.
     *
     *  @param[in] rhs The view to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const MoleculeView& rhs) const noexcept {
        return !(*this == rhs);
    }

protected:
    /// Allows base class to access protected members implementing it
    friend base_type;

    /// Returns a mutable reference to the i-th nucleus in *this
    auto at_(size_type i) { return nuclei()[i]; }

    /// Returns a read-only reference to the i-th nucleus in *this
    auto at_(size_type i) const { return nuclei()[i]; }

    /// Returns the number of nuclei in *this
    size_type size_() const noexcept { return nuclei().size(); }

private:
    /// Code factorization for determining if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// The number of electrons *this would have if it is neutral
    size_type neutral_n_electrons_() const noexcept;

    /// Throws if @p charge doesn't make sense
    void check_charge_(charge_type charge) const;

    /// Throws if @p multiplicity doesn't make sense
    void check_multiplicity_(multiplicity_type multiplicity) const;

    /// Type of the object implementing *this
    pimpl_pointer m_pimpl_;
};

/// Compares the object aliased by @p lhs to @p rhs for value equality
template<typename MoleculeType>
bool operator==(const MoleculeView<MoleculeType>& lhs,
                const std::remove_cv_t<MoleculeType>& rhs) {
    return lhs == MoleculeView<const MoleculeType>(rhs);
}

/// Determines if the object aliased by @p lhs is different than @p rhs
template<typename MoleculeType>
bool operator!=(const MoleculeView<MoleculeType>& lhs,
                const std::remove_cv_t<MoleculeType>& rhs) {
    return !(lhs == rhs);
}

/// Compares @p lhs to the value aliased by @p rhs for value equality
template<typename MoleculeType>
bool operator==(const std::remove_cv_t<MoleculeType>& lhs,
                const MoleculeView<MoleculeType>& rhs) {
    return rhs == lhs;
}

/// Determines if @p lhs is different from the object aliased by @p rhs
template<typename MoleculeType>
bool operator!=(const std::remove_cv_t<MoleculeType>& lhs,
                const MoleculeView<MoleculeType>& rhs) {
    return rhs != lhs;
}

/// Equality comparison when MoleculeViews alias Molecules of different cv-ness
template<typename MoleculeType1, typename MoleculeType2>
bool operator==(const MoleculeView<MoleculeType1>& lhs,
                const MoleculeView<MoleculeType2>& rhs) {
    return MoleculeView<const MoleculeType1>(lhs) ==
           MoleculeView<const MoleculeType2>(rhs);
}

/// Difference comparison for MoleculeViews alias Molecules of different cv-ness
template<typename MoleculeType1, typename MoleculeType2>
bool operator!=(const MoleculeView<MoleculeType1>& lhs,
                const MoleculeView<MoleculeType2>& rhs) {
    return MoleculeView<const MoleculeType1>(lhs) !=
           MoleculeView<const MoleculeType2>(rhs);
}

extern template class MoleculeView<Molecule>;
extern template class MoleculeView<const Molecule>;

} // namespace chemist

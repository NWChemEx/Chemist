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
#include <chemist/chemical_system/chemical_system_class.hpp>
namespace chemist {
namespace detail_ {
template<typename ChemicalSystemType>
class ChemicalSystemViewPIMPL;
}

/** @brief Enables using existing state as if it were a ChemicalSystem.
 *
 *  @tparam ChemicalSystemType The type of object *this will alias. Assumed to
 *                             be either `ChemicalSystem` or
 *                             `const ChemicalSystem`.
 *
 *  This class is designed to impart the same API as a ChemicalSystem object
 *  to existing state which may not necessarily be in an actual ChemicalSystem
 *  object.
 */
template<typename ChemicalSystemType>
class ChemicalSystemView {
private:
    /// Type of *this
    using my_type = ChemicalSystemView<ChemicalSystemType>;

    /// Type trait for determining if @p T is cv-qualified
    template<typename T>
    static constexpr bool is_cv_v = !std::is_same_v<std::remove_cv_t<T>, T>;

    /// Is *this aliasing a const ChemicalSystem?
    static constexpr bool const_alias_v = is_cv_v<ChemicalSystemType>;

    /// Enables a function if const_alias_v and !is_cv_v<T>
    template<typename T>
    using is_read_only_conversion_t =
      std::enable_if_t<const_alias_v && !is_cv_v<T>>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::ChemicalSystemViewPIMPL<ChemicalSystemType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of a struct defining the types for the aliased ChemicalSystem
    using type_traits = ChemistClassTraits<ChemicalSystemType>;

    /// Type of a struct defining the types for the Molecule piece of *this
    using molecule_traits = typename type_traits::molecule_traits;

    /// Type *this aliases
    using chemical_system_type = typename type_traits::value_type;

    /// A mutable reference to the type *this aliases
    using chemical_system_reference = typename type_traits::reference;

    /// A read-only reference to the type *this aliases
    using const_chemical_system_reference =
      typename type_traits::const_reference;

    /// Type of the Molecule piece of *this
    using molecule_type = typename molecule_traits::value_type;

    /// Type of a mutable reference to the Molecule piece of *this
    using molecule_reference = typename molecule_traits::view_type;

    /// Type of a read-only reference to the Molecule piece of *this
    using const_molecule_reference = typename molecule_traits::const_view_type;

    /** @brief Initializes *this to a view of the empty ChemicalSystem.
     *
     *  The view resulting from the default ctor acts like a view of a defaulted
     *  ChemicalSystem. In particular, this means the Molecule part of the
     *  aliased ChemicalSystem is empty.
     *
     *  @throw None No throw guarantee
     */
    ChemicalSystemView() noexcept;

    /** @brief Initializes *this to be a view of @p chemical_system.
     *
     *  Given an already existing ChemicalSystem object, @p chemical_system,
     *  this ctor will initialize *this so that it aliases the state in
     *  @p chemical_system.
     *
     *  @param[in] chemical_system The object to alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    ChemicalSystemView(chemical_system_reference chemical_system) :
      ChemicalSystemView(chemical_system.molecule()) {}

    /** @brief Initializes *this with the provided state.
     *
     *  This ctor takes aliases to the individual pieces of state making up
     *  a ChemicalSystem and initializes *this to be an alias of a
     *  ChemicalSystem comprised of that state.
     *
     *  @param[in] molecule A view to use as the Molecule piece of the aliased
     *                      ChemicalSystem.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    explicit ChemicalSystemView(molecule_reference molecule);

    /** @brief Converts a view of a mutable ChemicalSystem to a view of a
     *         read-only ChemicalSystem.
     *
     *  @tparam ChemicalSystemType2 The type of ChemicalSystem that @p other
     *                              aliases. For this ctor to participate in
     *                              overload resolution @p ChemicalSystemType2
     *                              must be non-cv qualified ChemicalSystem.
     *  @tparam <anonymous> Used to disable this ctor via SFINAE when either
     *                      the resulting view would not be read-only or when
     *                      @p ChemicalSystemType2 is not non-cv-qualified
     *                      ChemicalSystem.
     *
     *  Views of mutable ChemicalSystem objects should be usable wherever views
     *  of read-only ChemicalSystem objects are needed. This ctor enables
     *  implicit conversions from views of mutable ChemicalSystem objects to
     *  views of read-only ChemicalSystem objects.
     *
     *  @param[in] other The view to convert from.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    template<typename ChemicalSystemType2,
             typename = is_read_only_conversion_t<ChemicalSystemType2>>
    ChemicalSystemView(const ChemicalSystemView<ChemicalSystemType2>& other) :
      ChemicalSystemView(other.molecule()) {}

    /** @brief Initializes *this to a copy of @p other.
     *
     *  The object resulting from the copy ctor will alias the same state as
     *  @p other; however, it will alias that state through different pointers
     *  and/or references. Meaning, causing *this to alias a different
     *  ChemicalSystem will leave @p other unchanged, but modifying the state
     *  of the aliased ChemicalSystem through either @p other or *this will
     *  be reflected in both @p other and *this.
     *
     *  @param[in] other The view to copy.
     *
     *  @throw std::bad_alloc if there is a problem copying the state of
     *                        @p other. Strong throw guarantee.
     *
     */
    ChemicalSystemView(const ChemicalSystemView& other);

    /** @brief Initializes *this with the state in @p other.
     *
     *  The move ctor will initialize *this so that it now owns the state which
     *  was in @p other. This means that *this will alias the same
     *  ChemicalSystem object as @p other previously did.
     *
     *  @param[in,out] other The object to take the state from. After this call
     *                       @p other will be in a state consistent with being
     *                       default initialized.
     *
     *  @throw None No throw guarantee.
     *
     */
    ChemicalSystemView(ChemicalSystemView&& other) noexcept;

    /** @brief Replaces the state in *this with a copy of that in @p rhs.
     *
     *  This method will release the state currently in *this and replace it
     *  with a copy of the state in @p rhs.
     *
     *  @param[in] rhs The object whose state is being copied.
     *
     *  @return *this after copying the state of @p rhs.
     *
     *  @throw std::bad_alloc if there is problem allocating memory for the new
     *                        state. Strong throw guarantee.
     */
    ChemicalSystemView& operator=(const ChemicalSystemView& rhs);

    /** @brief Replaces the stat in *this with the state in @p rhs.
     *
     *  This method will release the state currently in *this and replace it
     *  with the state in @p rhs.
     *
     *  @param[in,out] rhs The object whose state is being taken. After this
     *                     operation @p rhs will be in a state consistent with
     *                     default initialization.
     *
     *  @return *this after taking the state from @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    ChemicalSystemView& operator=(ChemicalSystemView&& rhs) noexcept;

    /// Defaulted no-throw dtor
    ~ChemicalSystemView() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters and Setters
    // -------------------------------------------------------------------------

    /** @brief Provides access to Molecule piece of *this.
     *
     *  @return A mutable reference to the Molecule part of *this.
     *
     *  @throw None No throw guarantee.
     */
    molecule_reference molecule() noexcept;

    /** @brief Provides access to the Molecule piece of *this.
     *
     *  This method is the same as the non-const overload except that resulting
     *  Molecule reference is read-only. See the documentation for the non-const
     *  overload for more details.
     *
     *  @return A read-only reference to the Molecule part of *this.
     *
     *  @throw None No throw guarantee.
     */
    const_molecule_reference molecule() const noexcept;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // ------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with the state in @p other.
     *
     *  @param[in,out] other The object whose state is being transferred to
     *                       *this. After this method is called, @p other will
     *                       contain the state which was previously in *this.
     *
     *  @throw None no throw guarantee.
     */
    void swap(ChemicalSystemView& other) noexcept;

    /** @brief Are the ChemicalSystem objects aliased by *this and @p rhs equal?
     *
     *  This method does determine if *this aliases the same ChemicalSystem
     *  instance as @p rhs, but rather whether the aliased ChemicalSystem
     *  objects are value equal. The aliased ChemicalSystem objects are value
     *  equal if:
     *
     *  - The Molecule portions compare value equal
     *
     *  @param[in] rhs The view to compare against.
     *
     *  @return True if *this and @p rhs alias ChemicalSystem objects which
     *          compare value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ChemicalSystemView& rhs) const noexcept;

    /** @brief Is the ChemicalSystem aliased by *this value equal to @p rhs?
     *
     *  This method ultimately dispatches to same logic as the operator==
     *  overload comparing two ChemicalSystemView objects. See the documentation
     *  for that overload for more details.
     *
     *  @param[in] rhs The ChemicalSystem object to compare to.
     *
     *  @return True if *this aliases a ChemicalSystem object which is value
     *          equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const_chemical_system_reference rhs) const noexcept;

    /** @brief Are the ChemicalSystem objects aliased by *this and @p rhs
     *         different?
     *
     *  This method defines "different" as not value equal. Thus this method
     *  simply negates the corresponding overload of operator==. See the
     *  documentation for operator== for more detail.
     *
     *  @parm[in] rhs The view to compare against.
     *
     *  @return False if the ChemicalSystem aliased by *this is value equal to
     *          the ChemicalSystem aliased by @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const ChemicalSystemView& rhs) const noexcept {
        return !((*this) == rhs);
    }

    /** @brief Is the ChemicalSystem aliased by *this different from @p rhs.
     *
     *  This method simply negates the corresponding operator== overload.
     *  See the documentation for operator== for more details.
     *
     *  @param[in] rhs The ChemicalSystem to compare against.
     *
     *  @return False if the ChemicalSystem aliased by *this compares value
     *          equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const_chemical_system_reference rhs) const noexcept {
        return !((*this) == rhs);
    }

private:
    /// Code factorization for determining if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /// The object actually implementing *this
    pimpl_pointer m_pimpl_;
};

/** @brief Compares a ChemicalSystem to a a view of a ChemicalSystem.
 *
 *  @relates ChemicalSystem
 *  @relates ChemicalSystemView
 *
 *  @tparam ChemicalSystemType The type of the aliased ChemicalSystem. Expected
 *                             to be ChemicalSystem or a cv-qualified variant.
 *
 *  This method dispatches to operator== of @p rhs. See the documentation for
 *  ChemicalSystemView::operator==(ChemicalSystem) for more details.
 *
 *  @param[in] lhs The ChemicalSystem to the left of the operator.
 *  @param[in] rhs The ChemicalSystemView on the right of the operator.
 *
 *  @return True if @p lhs compares value equal to the ChemicalSystem aliased
 *          by @p rhs and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename ChemicalSystemType>
bool operator==(const std::decay_t<ChemicalSystemType>& lhs,
                const ChemicalSystemView<ChemicalSystemType>& rhs) noexcept {
    return rhs == lhs;
}

/** @brief Compares a ChemicalSystem to a a view of a ChemicalSystem.
 *
 *  @relates ChemicalSystem
 *  @relates ChemicalSystemView
 *
 *  @tparam ChemicalSystemType The type of the aliased ChemicalSystem. Expected
 *                             to be ChemicalSystem or a cv-qualified variant.
 *
 *  This method dispatches to operator!= of @p rhs. See the documentation for
 *  ChemicalSystemView::operator!=(ChemicalSystem) for more details.
 *
 *  @param[in] lhs The ChemicalSystem to the left of the operator.
 *  @param[in] rhs The ChemicalSystemView on the right of the operator.
 *
 *  @return False if @p lhs compares value equal to the ChemicalSystem aliased
 *          by @p rhs and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename ChemicalSystemType>
bool operator!=(const std::decay_t<ChemicalSystemType>& lhs,
                const ChemicalSystemView<ChemicalSystemType>& rhs) noexcept {
    return rhs != lhs;
}

extern template class ChemicalSystemView<ChemicalSystem>;
extern template class ChemicalSystemView<ChemicalSystem>;

} // namespace chemist

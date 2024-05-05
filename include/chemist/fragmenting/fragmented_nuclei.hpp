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
#include <chemist/chemical_system/nucleus/nucleus.hpp>
#include <chemist/fragmenting/capping/capping.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>

namespace chemist::fragmenting {
namespace detail_ {
template<typename NucleiType>
class FragmentedNucleiPIMPL;
}

template<typename NucleiType>
class FragmentedNuclei : public FragmentedBase<FragmentedNuclei<NucleiType>> {
private:
    /// Type of *this
    using my_type = FragmentedNuclei<NucleiType>;

    /// Type *this derives from
    using base_type = FragmentedBase<my_type>;

public:
    /// Type of the object defining types associated with *this
    using typename base_type::traits_type;

    /// Type of the object implementing *this
    using pimpl_type = detail_::FragmentedNucleiPIMPL<NucleiType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of object *this is fragmenting
    using typename base_type::supersystem_type;

    /// Type of the fragments
    using value_type = typename traits_type::fragment_type;

    /// Type of a mutable reference to a fragment
    using reference = typename traits_type::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename traits_type::const_fragment_reference;

    /// Type of the object holding the caps for the fragments
    using cap_set_type = typename traits_type::cap_set_type;

    /// Type supersystem uses for indexing and offsets
    using size_type = typename supersystem_type::size_type;

    /// Type used to specify a set of nucleus indices
    using nucleus_index_set = std::vector<size_type>;

    /// Type of a map from fragment index to nuclei offset
    using nucleus_map_type = std::vector<nucleus_index_set>;

    // -- CTors, assignment, and dtor

    /** @brief Creates a null object.
     *
     *  Null FragmentedNuclei objects have no supersystem, not even an empty
     *  one. Since they do not contain a supersystem they can not contain
     *  fragments either.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei() noexcept;

    /** @brief Creates an empty object.
     *
     *  Empty FragmentedNuclei objects have a supersystem, but no fragments.
     *  Fragments can be added by calling emplace_back. This ctor is most
     *  useful when *this will be filled in algorithmically.
     *
     *  @param[in] supersystem The object *this will hold fragments of.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     */
    explicit FragmentedNuclei(supersystem_type supersystem);

    /** @brief Initializes *this with the provided set of fragments.
     *
     *  @param[in] supersystem The object that is being fragmented.
     *  @param[in] frags A container-of-containers object such that
     *                   `frags[i][j]` is the offset (with respect to
     *                   @p supersystem) of the `j`-th nucleus in the `i`-th
     *                   fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    FragmentedNuclei(supersystem_type supersystem, nucleus_map_type frags);

    /** @brief Initializes *this from fragments and a set of caps.
     *
     *  @param[in] supersystem The object being fragmented.
     *  @param[in] frags A container-of-containers object such that
     *                   `frags[i][j]` is the offset (with respect to
     *                   @p supersystem) of the `j`-th nucleus in the `i`-th
     *                   fragment.
     *  @param[in] caps The caps for fixing severed bonds.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    FragmentedNuclei(supersystem_type supersystem, nucleus_map_type frags,
                     cap_set_type caps);

    /** @brief Initializes *this to a copy of @p other.
     *
     *  This ctor will deep copy the supersystem of @p other, as well as the
     *  mapping of nuclei to fragments and the caps.
     *
     *  @param[in] other The object whose state is being copied.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    FragmentedNuclei(const FragmentedNuclei& other);

    /** @brief Takes ownership of the state in @p other.
     *
     *  This ctor will transfer the state in @p other to the newly constructed
     *  object.
     *
     *  @param[in,out] other The object whose state is being taken. After this
     *                       call @p other will be in a valid, but otherwise
     *                       unspecified state.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei(FragmentedNuclei&& other) noexcept;

    /** @brief Replaces the state in *this with a copy of that in @p rhs.
     *
     *  This method will release the state in *this and replace it with a
     *  copy of the state in @p rhs.
     *
     *  @param[in] rhs The object whose state is being taken.
     *
     *  @return *this with its state set to a deep copy of the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    FragmentedNuclei& operator=(const FragmentedNuclei& rhs);

    /** @brief Replaces the state in *this with that in @p rhs.
     *
     *  This method will replace the state which currently resides in *this with
     *  the state which is in @p rhs.
     *
     *  @param[in,out] rhs The object to take the state from. After this method
     *                     is called @p rhs will be in a valid, but otherwise
     *                     undefined state.
     *
     *  @return *this after taking the state of @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei& operator=(FragmentedNuclei&& rhs) noexcept;

    /// Defaulted no throw dtor
    ~FragmentedNuclei() noexcept;

    /** @brief Exchanges the contents of *this with @p other.
     *
     *  @param[in,out] other The object to swap state with. After this call
     *                       @p other will contain the state which was
     *                       previously in *this
     *  @throw None No throw guarantee.
     */
    void swap(FragmentedNuclei& other) noexcept;

    /** @brief Is *this value equal to @p rhs?
     *
     *  This method defines value equal as both being null or both being
     *  non-null. If both are non-null then they must also satisfy:
     *  - Supersystems compare value equal
     *  - Same number of fragments
     *  - Same caps
     *  - (*this)[i] == rhs[i] for all i in range [0, size())
     *
     *  @param[in] rhs Teh object to compare against.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FragmentedNuclei& rhs) const noexcept;

    /** @brief Is *this different than @p rhs?
     *
     *  This method defines "different" as not value equal. See operator==
     *  for the definition of value equal.
     *
     *  @param[in] rhs The object to compare against.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const FragmentedNuclei& rhs) const noexcept;

protected:
    friend utilities::IndexableContainerBase<my_type>;

    /// Implements at() and operator[]
    reference at_(size_type i);

    /// Implements at() const and operator[] const
    const_reference at_(size_type i) const;

    /// Implements size_
    size_type size_() const noexcept;

private:
    /// Factors out the code for checking if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /// The object actually implementing *this
    pimpl_pointer m_pimpl_;
};

extern template class FragmentedNuclei<Nuclei>;
extern template class FragmentedNuclei<const Nuclei>;

} // namespace chemist::fragmenting

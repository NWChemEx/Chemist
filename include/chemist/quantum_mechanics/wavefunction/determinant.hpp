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
#include <boost/container/flat_set.hpp>
#include <chemist/quantum_mechanics/wavefunction/detail_/wavefunction_impl.hpp>
#include <chemist/quantum_mechanics/wavefunction/wavefunction_fwd.hpp>

namespace chemist::wavefunction {

/** @brief A fermionic multi-particle wavefunction.
 *
 *  @tparam OneParticleBasis Type of the vector space spanned by the one-
 *                           particle basis functions.
 *
 *  For @f$N@f$ indistinguishable fermions, the multi-particle wavefunction is
 *  given by the Slater determinant of the product of the @f$N@f$ occupied
 *  orbitals. This class represents that wavefunction.
 *
 */
template<typename OneParticleBasis>
class Determinant
  : public detail_::WavefunctionImpl<Determinant<OneParticleBasis>> {
private:
    /// Type of *this
    using my_type = Determinant<OneParticleBasis>;

public:
    /// Fundamental base type of *this
    using base_type = Wavefunction::base_type;

    /// Type of the orbitals in which the determinant is expanded
    using orbital_space_type = OneParticleBasis;

    /// Type of a mutable reference to an object of type orbital_space_type
    using orbital_space_reference = orbital_space_type&;

    /// Type of a read-only reference to an object of type orbital_space_type
    using const_orbital_space_reference = const orbital_space_type&;

    /// Type used for indexing and offsets
    using size_type = Wavefunction::size_type;

    /// Type of a set containing orbital indices
    using orbital_index_set_type = boost::container::flat_set<size_type>;

    /// A mutable reference to an object of type orbital_index_set_type
    using orbital_index_set_reference = orbital_index_set_type&;

    /// A read-only reference to an object of type orbital_index_set_type
    using const_orbital_index_set_reference = const orbital_index_set_type&;

    /// Type of a vector holding orbital occupations
    using occupation_vector_type = std::vector<unsigned short>;

    /** @brief Creates a determinant for a zero particle system.
     *
     *  The default ctor will create a Determinant which is associated with an
     *  empty orbital space and has no particles in assigned to any orbital
     *  (which is good because there are no orbitals to assign to).
     *
     *  @throw None No throw guarantee.
     */
    Determinant() noexcept = default;

    /** @brief Creates a Determinant from the provided orbitals and occupations.
     *
     *  A determinant for an @f$N@f$-particle system is obtained by considering
     *  all permutations of a set of @f$N@f$ orbitals taken from the full set
     *  of orbitals. This ctor treats @p orbitals as the full set of orbitals
     *  to choose from and will build the determinant from @p occupied such
     *  that the `orbitals[occupied[i]]` is the i-th orbital in each orbital
     *  product.
     *
     *  @param[in] occupied The set of orbital offsets which are occupied in
     *                      THIS determinant.
     *  @param[in] orbitals The full set of orbitals (including orbitals not
     *                      occupied in *this).
     *
     *  @throw None No throw guarantee.
     */
    Determinant(orbital_index_set_type occupied, orbital_space_type orbitals) :
      m_occupied_(std::move(occupied)), m_orbitals_(std::move(orbitals)) {}

    /** @brief The indices for the orbitals comprising *this.
     *
     *  An @f$N@f$-particle wavefunction contains particles in @f$N@f$ orbitals.
     *  Those orbitals are "occupied" in the sense that a particle is in them
     *  in this determinant; however, the indices may be those of virtual
     *  orbitals if *this is a determinant representing an excited state.
     *
     *  @return A set such that the i-th element is the offset of orbital i in
     *          the determinant @f$|\phi_0 \phi_1 \cdots \phi_i \cdots \rangle$.
     *
     *  @throw None No throw guarantee
     */
    orbital_index_set_reference orbital_indices() { return m_occupied_; }

    /** @brief The indices for the orbitals comprising *this.
     *
     *  This method is conceptually the same as the non-const version except
     *  that the resulting reference is immutable.
     *
     *  @return A set such that the i-th element is the offset of orbital i in
     *          the determinant @f$|\phi_0 \phi_1 \cdots \phi_i \cdots \rangle$.
     *
     *  @throw None No throw guarantee.
     */
    const_orbital_index_set_reference orbital_indices() const {
        return m_occupied_;
    }

    /** @brief Returns the occupation number vector for *this.
     *
     *  `orbital_indices` returns an array containing the offsets of the
     *  occupied orbitals. This function returns an array which is "number of
     *  orbitals" long such that the  `i`-th element of return is the occupation
     *  of orbital `i`.
     *
     *  @return An array providing the occupations of each orbital.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    occupation_vector_type occupations() const {
        occupation_vector_type rv(m_orbitals_.size(), 0);
        // TODO: We need to generalize this using TMP, for now crash if not CMOs
        static_assert(std::is_same_v<OneParticleBasis, CMOs> ||
                      std::is_same_v<OneParticleBasis, MOs>);
        for(auto i : orbital_indices()) rv[i] = 2;
        return rv;
    }

    /** @brief Returns the set of orbitals from which the occupied orbitals are
     *         taken.
     *
     *  Each determinant describing @f$N@f$ particles is obtained by taking
     *  permutations of electrons in @f$N@f$ orbitals. This method returns the
     *  full set of orbitals from which the occupied orbitals are taken. Note
     *  that in this context "occupied" refers to occupied in THIS determinant,
     *  and the orbitals are not necessarily occupied in the reference
     *  determinant.
     *
     *  @return The orbital space from which the occupied orbitals are taken.
     *
     *  @throw None No throw guarantee.
     */
    orbital_space_reference orbitals() { return m_orbitals_; }

    /** @brief Returns the set of orbitals from which the occupied orbitals are
     *         taken.
     *
     *  Conceptually this method is the same as the non-const version except
     *  that the resulting orbital space is immutable.
     *
     *  @return The orbital space from which the occupied orbitals are taken.
     *
     *  @throw None No throw guarantee.
     */
    const_orbital_space_reference orbitals() const { return m_orbitals_; }

    /** @brief Determins if *this is value equal to @p rhs.
     *
     *  Two Determinant objects are value equal if:
     *  - the orbitals are taken from the same set of orbitals, and if
     *  - the same @f$N@f$ orbitals are occupied in both determinants.
     *
     *  @param[in] rhs The determinant to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Determinant& rhs) const noexcept {
        return std::tie(m_occupied_, m_orbitals_) ==
               std::tie(rhs.m_occupied_, rhs.m_orbitals_);
    }

    /** @brief Determines if *this is different than @p rhs.
     *
     *  This class defines "different" as "not value equal." See the description
     *  for operator== for the definition of value equal.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Determinant& rhs) const noexcept {
        return !((*this) == rhs);
    }

private:
    /// The indices of the orbitals which are occupied
    orbital_index_set_type m_occupied_;

    /// The full set of orbitals (occupied and virtual)
    orbital_space_type m_orbitals_;
};

} // namespace chemist::wavefunction

/*
 * Copyright 2026 NWChemEx-Project
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
#include <chemist/experimental/basis_set/ao_shell_enums.hpp>
#include <chemist/experimental/traits/atomic_basis_set_traits.hpp>
#include <chemist/experimental/traits/point_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <span>
#include <string>

namespace chemist::experimental {

class MolecularBasisSet;

template<typename MolecularBasisSetType>
class MolecularBasisSetView;

namespace detail_ {

/** @brief Types shared by both ChemistClassTraits<MolecularBasisSet>
 *         specializations.
 *
 *  A molecular basis set stores one atomic basis set's worth of state per
 *  atom, back to back, so the parameter, shell, and per-atom types are pulled
 *  out of ChemistClassTraits<AtomicBasisSet> rather than being redeclared
 *  here.
 */
struct MolecularBasisSetTraitsCommon {
    /// Traits of a mutable atomic basis set
    using atomic_basis_set_traits =
      ChemistClassTraits<experimental::AtomicBasisSet>;

    /// Traits of the set of centers
    using point_set_traits = ChemistClassTraits<experimental::PointSet>;

    /// Type of the molecular basis set *this describes
    using value_type = experimental::MolecularBasisSet;

    /// Type of the atomic basis sets *this is a container of
    using atomic_basis_set_type = experimental::AtomicBasisSet;

    /// Type of a read-only view of one atomic basis set
    using const_atomic_basis_set_reference =
      typename atomic_basis_set_traits::const_view_type;

    /// Type of a pointer to a read-only, polymorphic view of one shell
    using shell_pointer = typename atomic_basis_set_traits::shell_pointer;

    /// Type used to model the name of a basis set, e.g. "cc-pVDZ"
    using name_type = typename atomic_basis_set_traits::name_type;

    /// Type used to model an atomic number
    using atomic_number_type =
      typename atomic_basis_set_traits::atomic_number_type;

    /// Type used to model the total angular momentum of a shell
    using angular_momentum_type =
      typename atomic_basis_set_traits::angular_momentum_type;

    /// Type used to own the centers
    using center_set_type = experimental::PointSet;

    /// Type used to own one parameter's worth of values (coefficients or
    /// exponents), across every primitive of every shell of every atom
    using buffer_type = typename atomic_basis_set_traits::buffer_type;

    /// Type of a read-only, aliasing view of a coefficient/exponent array
    using const_buffer_reference =
      typename atomic_basis_set_traits::const_buffer_reference;

    /// Type of a read-only, aliasing view of a primitive
    using const_primitive_reference =
      typename atomic_basis_set_traits::const_primitive_reference;

    /// Type of a read-only, aliasing view of the centers
    using const_center_set_reference =
      typename point_set_traits::const_view_type;

    /// Type of a read-only view of the angular momenta
    using const_l_span = std::span<const angular_momentum_type>;

    /// Type of a read-only view of the names
    using const_name_span = std::span<const name_type>;

    /// Type of a read-only view of the atomic numbers
    using const_atomic_number_span = std::span<const atomic_number_type>;

    /// Type used for indexing and offsets
    using size_type = typename atomic_basis_set_traits::size_type;

    /// Type of a read-only view of a set of offsets
    using offset_span = std::span<const size_type>;

    /// Type of a read-only view of the purity of each atom's shells
    using purity_span = std::span<const ShellPurity>;

    /// Type of a read-only view of the AO ordering of each atom's shells
    using ordering_span = std::span<const AOOrdering>;

    /// Type of a half-open range of offsets, [first, second)
    using range_type = typename atomic_basis_set_traits::range_type;
};

} // namespace detail_

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for a mutable experimental::MolecularBasisSet. */
template<>
struct ChemistClassTraits<experimental::MolecularBasisSet>
  : experimental::detail_::MolecularBasisSetTraitsCommon {
    /// Type of a mutable reference to a molecular basis set
    using reference = value_type&;

    /// Type of a read-only reference to a molecular basis set
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to a molecular basis set
    using view_type = experimental::MolecularBasisSetView<value_type>;

    /// Type acting like a read-only reference to a molecular basis set
    using const_view_type =
      experimental::MolecularBasisSetView<const value_type>;

    /// Type of a mutable view of one atomic basis set
    using atomic_basis_set_reference =
      typename atomic_basis_set_traits::view_type;

    /// Type of a mutable, aliasing view of a coefficient/exponent array
    using buffer_reference = typename atomic_basis_set_traits::buffer_reference;

    /// Type of a mutable, aliasing view of a primitive
    using primitive_reference =
      typename atomic_basis_set_traits::primitive_reference;

    /// Type of a mutable, aliasing view of the centers
    using center_set_reference = typename point_set_traits::view_type;

    /// Type of a mutable view of the angular momenta
    using l_span = std::span<angular_momentum_type>;

    /// Type of a mutable view of the names
    using name_span = std::span<name_type>;

    /// Type of a mutable view of the atomic numbers
    using atomic_number_span = std::span<atomic_number_type>;
};

/** @brief Traits for a read-only experimental::MolecularBasisSet.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through
 *  MolecularBasisSetView, all the way down to the AtomicBasisSetView objects
 *  it hands out.
 */
template<>
struct ChemistClassTraits<const experimental::MolecularBasisSet>
  : experimental::detail_::MolecularBasisSetTraitsCommon {
    /// Type of a reference to a molecular basis set (read-only because *this
    /// is const)
    using reference = const value_type&;

    /// Type of a read-only reference to a molecular basis set
    using const_reference = const value_type&;

    /// Type acting like a reference to a molecular basis set (read-only
    /// because *this is const)
    using view_type = experimental::MolecularBasisSetView<const value_type>;

    /// Type acting like a read-only reference to a molecular basis set
    using const_view_type = view_type;

    /// Type of a view of one atomic basis set (read-only because *this is
    /// const)
    using atomic_basis_set_reference = const_atomic_basis_set_reference;

    /// Type of an aliasing view of a coefficient/exponent array (read-only
    /// because *this is const)
    using buffer_reference = const_buffer_reference;

    /// Type of an aliasing view of a primitive (read-only because *this is
    /// const)
    using primitive_reference = const_primitive_reference;

    /// Type of an aliasing view of the centers (read-only because *this is
    /// const)
    using center_set_reference = const_center_set_reference;

    /// Type of a view of the angular momenta (read-only because *this is
    /// const)
    using l_span = const_l_span;

    /// Type of a view of the names (read-only because *this is const)
    using name_span = const_name_span;

    /// Type of a view of the atomic numbers (read-only because *this is
    /// const)
    using atomic_number_span = const_atomic_number_span;
};

} // namespace chemist

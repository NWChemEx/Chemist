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
#include <chemist/experimental/traits/ao_shell_traits.hpp>
#include <chemist/experimental/traits/contracted_gaussian_traits.hpp>
#include <chemist/experimental/traits/primitive_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

namespace chemist::experimental {

class AtomicBasisSet;

template<typename AtomicBasisSetType>
class AtomicBasisSetView;

namespace detail_ {

class AtomicBasisSetPIMPLBase;

/** @brief Types shared by both ChemistClassTraits<AtomicBasisSet>
 *         specializations.
 *
 *  An atomic basis set stores the parameters of its shells as one contracted
 *  Gaussian's worth of state per shell, so the parameter types are pulled out
 *  of ChemistClassTraits<ContractedGaussian> and the shell types out of
 *  ChemistClassTraits<AOShellBase>, rather than being redeclared here.
 */
struct AtomicBasisSetTraitsCommon {
    /// Traits of the contracted Gaussians the shells are built on
    using contracted_gaussian_traits =
      ChemistClassTraits<experimental::ContractedGaussian>;

    /// Traits of the shells the set hands out
    using ao_shell_traits = ChemistClassTraits<experimental::AOShellBase>;

    /// Traits of the primitives the set hands out
    using primitive_traits = ChemistClassTraits<experimental::Primitive>;

    /// Type of the atomic basis set *this describes
    using value_type = experimental::AtomicBasisSet;

    /// Type of the object implementing an atomic basis set
    using pimpl_type = experimental::detail_::AtomicBasisSetPIMPLBase;

    /// Type of a pointer to the object implementing an atomic basis set
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of the polymorphic, owning shell a set accepts
    using shell_type = typename ao_shell_traits::value_type;

    /// Type of the polymorphic view of a shell a set accepts and hands out
    using shell_view_type = typename ao_shell_traits::const_view_type;

    /// Type of a read-only reference to a polymorphic, owning shell
    using const_shell_reference = const shell_type&;

    /// Type of a read-only reference to a polymorphic view of a shell
    using const_shell_view_reference = const shell_view_type&;

    /// Type of a pointer to a read-only, polymorphic view of one shell
    using shell_pointer = std::unique_ptr<shell_view_type>;

    /// Type used to model the name of the basis set, e.g. "cc-pVDZ"
    using name_type = std::string;

    /// Type used to model the atomic number
    using atomic_number_type = std::size_t;

    /// Type used to model the total angular momentum of a shell
    using angular_momentum_type =
      typename contracted_gaussian_traits::angular_momentum_type;

    /// Type used to model the point the set is centered on
    using center_type = typename contracted_gaussian_traits::center_type;

    /// Type used to own one parameter's worth of values (coefficients or
    /// exponents), across every primitive of every shell
    using buffer_type = typename contracted_gaussian_traits::buffer_type;

    /// Type of a read-only, aliasing view of a coefficient/exponent array
    using const_buffer_reference =
      typename contracted_gaussian_traits::const_buffer_reference;

    /// Type of a read-only, aliasing view of a primitive
    using const_primitive_reference =
      typename primitive_traits::const_view_type;

    /// Type of a read-only reference to the name
    using const_name_reference = const name_type&;

    /// Type of a read-only reference to the atomic number
    using const_atomic_number_reference = const atomic_number_type&;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename contracted_gaussian_traits::const_center_reference;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// Type of a half-open range of offsets, [first, second)
    using range_type = std::pair<size_type, size_type>;
};

} // namespace detail_

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for a mutable experimental::AtomicBasisSet. */
template<>
struct ChemistClassTraits<experimental::AtomicBasisSet>
  : experimental::detail_::AtomicBasisSetTraitsCommon {
    /// Type of a mutable reference to an atomic basis set
    using reference = value_type&;

    /// Type of a read-only reference to an atomic basis set
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to an atomic basis set
    using view_type = experimental::AtomicBasisSetView<value_type>;

    /// Type acting like a read-only reference to an atomic basis set
    using const_view_type = experimental::AtomicBasisSetView<const value_type>;

    /// Type of a mutable, aliasing view of a coefficient/exponent array
    using buffer_reference =
      typename contracted_gaussian_traits::buffer_reference;

    /// Type of a mutable, aliasing view of a primitive
    using primitive_reference = typename primitive_traits::view_type;

    /// Type of a mutable reference to the name
    using name_reference = name_type&;

    /// Type of a mutable reference to the atomic number
    using atomic_number_reference = atomic_number_type&;

    /// Type of a mutable, aliasing view of the center
    using center_reference =
      typename contracted_gaussian_traits::center_reference;
};

/** @brief Traits for a read-only experimental::AtomicBasisSet.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through
 *  AtomicBasisSetView.
 */
template<>
struct ChemistClassTraits<const experimental::AtomicBasisSet>
  : experimental::detail_::AtomicBasisSetTraitsCommon {
    /// Type of a reference to an atomic basis set (read-only because *this is
    /// const)
    using reference = const value_type&;

    /// Type of a read-only reference to an atomic basis set
    using const_reference = const value_type&;

    /// Type acting like a reference to an atomic basis set (read-only because
    /// *this is const)
    using view_type = experimental::AtomicBasisSetView<const value_type>;

    /// Type acting like a read-only reference to an atomic basis set
    using const_view_type = view_type;

    /// Type of an aliasing view of a coefficient/exponent array (read-only
    /// because *this is const)
    using buffer_reference = const_buffer_reference;

    /// Type of an aliasing view of a primitive (read-only because *this is
    /// const)
    using primitive_reference = const_primitive_reference;

    /// Type of a reference to the name (read-only because *this is const)
    using name_reference = const_name_reference;

    /// Type of a reference to the atomic number (read-only because *this is
    /// const)
    using atomic_number_reference = const_atomic_number_reference;

    /// Type of an aliasing view of the center (read-only because *this is
    /// const)
    using center_reference = const_center_reference;
};

} // namespace chemist

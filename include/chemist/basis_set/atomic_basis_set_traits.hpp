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
#include <chemist/basis_set/shell_traits.hpp>

namespace chemist::basis_set {

/** @brief Propagates types associated with an AtomicBaseSet of type
 *         @p AtomicBSType.
 *
 *  This struct uses inheritance because the total set of types needed for
 *  interacting with a AtomicBasisSet is the union of the types needed for
 *  the primitives making up the AtomicBasisSet plus the types exclusive
 *  to the AtomicBasisSet.
 *
 *  @tparam AtomicBSType expected to be either an instantiation of
 *                       AtomicBasisSet or AtomicBasisSetView.
 */
template<typename AtomicBSType>
struct AtomicBasisSetTraits
  : public ShellTraits<typename AtomicBSType::reference> {
    /// String-like type used to store the basis set name
    using name_type = typename AtomicBSType::name_type;

    /// Mutable reference to the basis set's name
    using name_reference = typename AtomicBSType::name_reference;

    /// Read-only reference to the basis set's name
    using const_name_reference = typename AtomicBSType::const_name_reference;

    /// Unsigned integral type used to store the atomic number
    using atomic_number_type = typename AtomicBSType::atomic_number_type;

    /// Mutable reference to the basis set's atomic number
    using atomic_number_reference =
      typename AtomicBSType::atomic_number_reference;

    /// Read-only reference to the basis set's atomic number
    using const_atomic_number_reference =
      typename AtomicBSType::const_atomic_number_reference;

    /// Type used to return index ranges
    using range_type = typename AtomicBSType::range_type;

    /// Type of the Shells on this AtomicBasisSet
    using shell_type = typename AtomicBSType::value_type;

    /// Type of a read-/write-able reference to a AtomicBasisSet
    using shell_reference = typename AtomicBSType::reference;

    /// Type of a read-only reference to a AtomicBasisSet
    using const_shell_reference = typename AtomicBSType::const_reference;
};

} // namespace chemist::basis_set
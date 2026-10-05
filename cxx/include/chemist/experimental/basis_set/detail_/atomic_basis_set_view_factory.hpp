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
#include <chemist/experimental/basis_set/detail_/atomic_basis_set_pimpl_base.hpp>
#include <chemist/experimental/traits/atomic_basis_set_traits.hpp>
#include <span>

namespace chemist::experimental::detail_ {

/** @brief Makes an implementation of an atomic basis set which aliases the
 *         provided state.
 *
 *  AtomicBasisSetViewPIMPL is templated on the type of the shells, which an
 *  atomic basis set only knows at runtime, as a ShellPurity and an
 *  AOOrdering. This function maps the runtime description to the
 *  compile-time one, so that code which needs to build views of atomic basis
 *  sets over its own storage, e.g. MolecularBasisSet, need not include the
 *  implementation templates.
 *
 *  The state is described exactly as for AtomicBasisSetViewPIMPL's ctor; in
 *  particular, @p offsets are taken relative to their first element.
 *
 *  @param[in] purity The purity of the shells.
 *  @param[in] ordering The AO ordering of the shells.
 *  @param[in] coefficients Every coefficient, shell-major.
 *  @param[in] exponents Every exponent, shell-major.
 *  @param[in] ls The angular momentum of each shell.
 *  @param[in] offsets The offsets of the shells into the parameter arrays,
 *                     plus one past the end.
 *  @param[in] center The center shared by every shell.
 *  @param[in] name The name of the basis set.
 *  @param[in] atomic_number The atomic number.
 *
 *  @return An implementation aliasing the provided state. It is read-only
 *          if the state is.
 *
 *  @throw std::invalid_argument if @p ordering is not a known AO ordering, or
 *                               under the conditions AtomicBasisSetViewPIMPL's
 *                               ctor throws. Strong throw guarantee.
 *  @throw std::bad_alloc if there is a problem allocating the result. Strong
 *                        throw guarantee.
 */
///@{
AtomicBasisSetPIMPLBase::pimpl_pointer make_atomic_basis_set_view_pimpl(
  ShellPurity purity, AOOrdering ordering,
  ChemistClassTraits<AtomicBasisSet>::buffer_reference coefficients,
  ChemistClassTraits<AtomicBasisSet>::buffer_reference exponents,
  std::span<ChemistClassTraits<AtomicBasisSet>::angular_momentum_type> ls,
  std::span<const ChemistClassTraits<AtomicBasisSet>::size_type> offsets,
  ChemistClassTraits<AtomicBasisSet>::center_reference center,
  ChemistClassTraits<AtomicBasisSet>::name_reference name,
  ChemistClassTraits<AtomicBasisSet>::atomic_number_reference atomic_number);

AtomicBasisSetPIMPLBase::pimpl_pointer make_atomic_basis_set_view_pimpl(
  ShellPurity purity, AOOrdering ordering,
  ChemistClassTraits<const AtomicBasisSet>::buffer_reference coefficients,
  ChemistClassTraits<const AtomicBasisSet>::buffer_reference exponents,
  std::span<const ChemistClassTraits<AtomicBasisSet>::angular_momentum_type> ls,
  std::span<const ChemistClassTraits<AtomicBasisSet>::size_type> offsets,
  ChemistClassTraits<const AtomicBasisSet>::center_reference center,
  ChemistClassTraits<const AtomicBasisSet>::name_reference name,
  ChemistClassTraits<const AtomicBasisSet>::atomic_number_reference
    atomic_number);
///@}

} // namespace chemist::experimental::detail_

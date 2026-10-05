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

#include <chemist/experimental/basis_set/detail_/atomic_basis_set_pimpl.hpp>
#include <chemist/experimental/basis_set/detail_/atomic_basis_set_view_factory.hpp>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace chemist::experimental::detail_ {
namespace {

using pimpl_pointer = AtomicBasisSetPIMPLBase::pimpl_pointer;

/** @brief Makes the view implementation for the requested shell type.
 *
 *  As with make_pimpl in atomic_basis_set_class.cpp, this is where the
 *  runtime description of a shell type becomes the compile-time one; a new
 *  ordering is supported by adding a case here.
 *
 *  @tparam IsConst Whether the aliased state is read-only.
 */
template<bool IsConst, typename... Args>
pimpl_pointer make_view_pimpl(ShellPurity purity, AOOrdering ordering,
                              Args&&... args) {
    // Makes the implementation for the shell type tag holds, applying const
    // to it if the state is read-only
    auto make = [&](auto tag) -> pimpl_pointer {
        using shell_type = typename decltype(tag)::type;
        using view_shell_type =
          std::conditional_t<IsConst, const shell_type, shell_type>;
        return std::make_unique<AtomicBasisSetViewPIMPL<view_shell_type>>(
          std::forward<Args>(args)...);
    };

    switch(ordering) {
        case AOOrdering::cca:
            if(purity == ShellPurity::pure)
                return make(std::type_identity<CCAShell<SphericalAO>>{});
            return make(std::type_identity<CCAShell<CartesianAO>>{});
    }
    throw std::invalid_argument(
      "chemist::experimental::AtomicBasisSet: unknown AO ordering.");
}

} // namespace

pimpl_pointer make_atomic_basis_set_view_pimpl(
  ShellPurity purity, AOOrdering ordering,
  ChemistClassTraits<AtomicBasisSet>::buffer_reference coefficients,
  ChemistClassTraits<AtomicBasisSet>::buffer_reference exponents,
  std::span<ChemistClassTraits<AtomicBasisSet>::angular_momentum_type> ls,
  std::span<const ChemistClassTraits<AtomicBasisSet>::size_type> offsets,
  ChemistClassTraits<AtomicBasisSet>::center_reference center,
  ChemistClassTraits<AtomicBasisSet>::name_reference name,
  ChemistClassTraits<AtomicBasisSet>::atomic_number_reference atomic_number) {
    return make_view_pimpl<false>(purity, ordering, std::move(coefficients),
                                  std::move(exponents), ls, offsets,
                                  std::move(center), name, atomic_number);
}

pimpl_pointer make_atomic_basis_set_view_pimpl(
  ShellPurity purity, AOOrdering ordering,
  ChemistClassTraits<const AtomicBasisSet>::buffer_reference coefficients,
  ChemistClassTraits<const AtomicBasisSet>::buffer_reference exponents,
  std::span<const ChemistClassTraits<AtomicBasisSet>::angular_momentum_type> ls,
  std::span<const ChemistClassTraits<AtomicBasisSet>::size_type> offsets,
  ChemistClassTraits<const AtomicBasisSet>::center_reference center,
  ChemistClassTraits<const AtomicBasisSet>::name_reference name,
  ChemistClassTraits<const AtomicBasisSet>::atomic_number_reference
    atomic_number) {
    return make_view_pimpl<true>(purity, ordering, std::move(coefficients),
                                 std::move(exponents), ls, offsets,
                                 std::move(center), name, atomic_number);
}

} // namespace chemist::experimental::detail_

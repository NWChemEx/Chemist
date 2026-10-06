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

#include <chemist/experimental/basis_set/ao_shell.hpp>
#include <chemist/experimental/basis_set/ao_shell_view.hpp>
#include <chemist/experimental/basis_set/cartesian_ao_view.hpp>
#include <chemist/experimental/basis_set/spherical_ao_view.hpp>

namespace chemist::experimental {

// The members below are defined here, rather than inline, because they need
// CartesianAOView, SphericalAOView, or AOShellView to be complete, and those
// headers (directly or indirectly) include the shell headers.

template<typename BaseType, typename AOType>
auto AOShellCommon<BaseType, AOType>::at(size_type i) const
  -> const_ao_pointer {
    this->check_offset_(i);
    if constexpr(traits_type::is_pure) {
        // Hands the new shell view to the AO, rather than letting the AO
        // clone it, so that indexing allocates one shell view, not two.
        return std::make_unique<const_ao_reference>(get_cartesian_shell(),
                                                    ao_index_(i));
    } else {
        const auto [x, y, z] = ao_index_(i);
        return std::make_unique<const_ao_reference>(
          this->get_contracted_gaussian(), x, y, z);
    }
}

template<typename BaseType, typename AOType>
auto AOShellCommon<BaseType, AOType>::at_(size_type i) const
  -> ao_view_pointer {
    return at(i);
}

template<typename AOType>
auto AOShell<AOType>::as_view() const -> view_pointer {
    // Every class deriving from *this views as its own view type, which is an
    // AOShellView<AOType>, so the downcast can not fail.
    return view_pointer(
      static_cast<AOShellView<AOType>*>(this->as_view_().release()));
}

template class AOShellCommon<AOShellBase, CartesianAO>;
template class AOShellCommon<AOShellBase, SphericalAO>;
template class AOShellCommon<AOShellBaseView, CartesianAO>;
template class AOShellCommon<AOShellBaseView, SphericalAO>;
template class AOShell<CartesianAO>;
template class AOShell<SphericalAO>;
template class AOShellView<CartesianAO>;
template class AOShellView<SphericalAO>;

} // namespace chemist::experimental

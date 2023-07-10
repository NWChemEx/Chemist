/*
 * Copyright 2022 NWChemEx-Project
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
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian_traits.hpp>
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian_view.hpp>
#include <vector>

namespace chemist::basis_set::detail_ {

// Design note: we went with PIMPL because depending on the guarantees on the
//              data we can get better performance
template<typename CGType>
class ContractedGaussianViewPIMPL {
public:
    /// Type of *this
    using my_type = ContractedGaussianViewPIMPL<CGType>;

    /// Type this is implementing
    using parent_type = ContractedGaussianView<CGType>;

    /// Traits class containing the types for parent_type
    using cg_traits = ContractedGaussianTraits<parent_type>;

    /// Type parent uses for indexing and offsets
    using size_type = typename parent_type::size_type;

    ContractedGaussianViewPIMPL(size_type n_prims,
                                typename cg_traits::coefficient_reference c0,
                                typename cg_traits::exponent_reference e0,
                                typename cg_traits::center_reference center) :
      m_n_prims(n_prims),
      m_pcoefs(&c0),
      m_pexps(&e0),
      m_center(std::move(center)) {}

    auto size() const noexcept { return m_n_prims; }

    decltype(auto) coefficient(size_type i) { return m_pcoefs[i]; }

    decltype(auto) coefficient(size_type i) const { return m_pcoefs[i]; }

    decltype(auto) exponent(size_type i) { return m_pexps[i]; }

    decltype(auto) exponent(size_type i) const { return m_pexps[i]; }

    auto operator[](size_type i) {
        return typename cg_traits::primitive_reference(coefficient(i),
                                                       exponent(i), m_center);
    }

    auto operator[](size_type i) const {
        return typename cg_traits::const_primitive_reference(
          coefficient(i), exponent(i), m_center);
    }

    bool operator==(const ContractedGaussianViewPIMPL& rhs) const noexcept {
        if(size() != rhs.size()) return false;
        if(m_center != rhs.m_center) return false;
        for(auto i = 0; i < size(); ++i) {
            if(coefficient(i) != rhs.coefficient(i)) return false;
            if(exponent(i) != rhs.exponent(i)) return false;
        }
        return true;
    }

    /// Number of primitives
    size_type m_n_prims;

    /// Pointer to the first coefficient
    typename cg_traits::coefficient_pointer m_pcoefs;

    /// Pointer to the first exponent
    typename cg_traits::exponent_pointer m_pexps;

    /// The center common to all primitives
    typename cg_traits::center_reference m_center;
};

} // namespace chemist::basis_set::detail_

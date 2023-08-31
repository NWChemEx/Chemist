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
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian_traits.hpp>
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp>
#include <optional>
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

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /// Null state
    ContractedGaussianViewPIMPL() = default;

    /// Assumes contiguous state for c0 and e0
    ContractedGaussianViewPIMPL(size_type n_prims,
                                typename cg_traits::coefficient_reference c0,
                                typename cg_traits::exponent_reference e0,
                                typename cg_traits::center_reference center) :
      m_n_prims_(n_prims),
      m_pcoefs_(&c0),
      m_pexps_(&e0),
      m_center_(std::move(center)) {}

    // -------------------------------------------------------------------------
    // -- Getters and setters
    // -------------------------------------------------------------------------

    // Number of primitives
    auto size() const noexcept { return m_n_prims_; }

    // reference to mutable i-th primitive's coefficient
    decltype(auto) coefficient(size_type i) { return m_pcoefs_[i]; }

    // reference to read-only i-th primitive coefficient
    decltype(auto) coefficient(size_type i) const { return m_pcoefs_[i]; }

    // reference to mutable exponent for the i-th primitive
    decltype(auto) exponent(size_type i) { return m_pexps_[i]; }

    // reference to read-only exponent for the i-th primitive
    decltype(auto) exponent(size_type i) const { return m_pexps_[i]; }

    // mutable reference to where these primitives are centered
    typename cg_traits::center_reference center() { return m_center_.value(); }

    // read-only reference to where these primitives are centered
    typename cg_traits::const_center_reference center() const {
        return m_center_.value();
    }

    // mutable reference to i-th primitive
    auto operator[](size_type i) {
        return typename cg_traits::primitive_reference(coefficient(i),
                                                       exponent(i), center());
    }

    // read-only reference to the i-th primitive
    auto operator[](size_type i) const {
        return typename cg_traits::const_primitive_reference(
          coefficient(i), exponent(i), center());
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    bool operator==(const ContractedGaussianViewPIMPL& rhs) const noexcept {
        if(size() != rhs.size()) return false;
        if(m_center_ != rhs.m_center_) return false;
        for(auto i = 0; i < size(); ++i) {
            if(coefficient(i) != rhs.coefficient(i)) return false;
            if(exponent(i) != rhs.exponent(i)) return false;
        }
        return true;
    }

private:
    /// Number of primitives
    size_type m_n_prims_ = 0;

    /// Pointer to the first coefficient
    typename cg_traits::coefficient_pointer m_pcoefs_ = nullptr;

    /// Pointer to the first exponent
    typename cg_traits::exponent_pointer m_pexps_ = nullptr;

    /// The center common to all primitives
    std::optional<typename cg_traits::center_reference> m_center_;
};

} // namespace chemist::basis_set::detail_

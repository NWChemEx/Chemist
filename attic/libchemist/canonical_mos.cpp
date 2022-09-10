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

#include "canonical_mos.hpp"
#include "chemist/detail_/canonical_mo_pimpl.hpp"

namespace chemist {

#define OrthSpace OrthogonalSpace<e_type, t_type>
#define CanMO CanonicalMO<e_type, t_type>

template<typename e_type, typename t_type>
const typename CanMO::pimpl_type& CanMO::dpimpl_() const {
    const auto* ptr = dynamic_cast<const pimpl_type*>(&this->pimpl_());
    assert(ptr != nullptr);
    return *ptr;
}

template<typename e_type, typename t_type>
CanMO::CanonicalMO() : OrthSpace(std::make_unique<pimpl_type>()) {}

template<typename e_type, typename t_type>
CanMO::CanonicalMO(basis_type bs, t_type S, t_type mo_energies, t_type C,
                   t_type Cdagger, t_type D) :
  OrthSpace(std::make_unique<pimpl_type>(bs, S, mo_energies, C, Cdagger, D)) {}

template<typename e_type, typename t_type>
CanMO::CanonicalMO(basis_type bs, t_type S, t_type mo_energies, t_type C,
                   t_type Cdagger) :
  OrthSpace(std::make_unique<pimpl_type>(bs, S, mo_energies, C, Cdagger)) {}

template<typename e_type, typename t_type>
CanMO::CanonicalMO(basis_type bs, t_type S, t_type mo_energies, t_type C) :
  OrthSpace(std::make_unique<pimpl_type>(bs, S, mo_energies, C)) {}

template<typename e_type, typename t_type>
CanMO::CanonicalMO(const CanonicalMO<e_type, t_type>& rhs) :
  OrthSpace(std::move(rhs.pimpl_().clone())) {}

template<typename e_type, typename t_type>
CanMO::CanonicalMO(CanonicalMO<e_type, t_type>&& rhs) noexcept = default;

template<typename e_type, typename t_type>
CanMO& CanMO::operator=(const CanonicalMO<e_type, t_type>& rhs) {
    return *this = std::move(CanMO(rhs));
}

template<typename e_type, typename t_type>
CanMO& CanMO::operator=(CanonicalMO<e_type, t_type>&& rhs) noexcept = default;

template<typename e_type, typename t_type>
CanMO::~CanonicalMO() noexcept = default;

template<typename e_type, typename t_type>
const t_type& CanMO::mo_energies() const {
    return dpimpl_().mo_energies();
}

template class CanonicalMO<float, type::tensor<float>>;
template class CanonicalMO<float, type::tensor<double>>;
template class CanonicalMO<double, type::tensor<float>>;
template class CanonicalMO<double, type::tensor<double>>;

} // namespace chemist
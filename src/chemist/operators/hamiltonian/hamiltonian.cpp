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

#include "chemist/operators/hamiltonian/hamiltonian.hpp"
#include "chemist/operators/kinetic.hpp"

namespace chemist::operators {

using size_type = typename Hamiltonian::size_type;

Hamiltonian::Hamiltonian() noexcept  = default;
Hamiltonian::~Hamiltonian() noexcept = default;

Hamiltonian::Hamiltonian(const Hamiltonian& other)     = default;
Hamiltonian::Hamiltonian(Hamiltonian&& other) noexcept = default;
Hamiltonian& Hamiltonian::operator=(const Hamiltonian& other) = default;

Hamiltonian& Hamiltonian::operator=(Hamiltonian&& other) noexcept = default;

size_type Hamiltonian::n_electrons() const noexcept {
    using T_type = NElectronKinetic;
    if(!has_term<T_type>()) return 0;
    auto T = get_terms<T_type>();
    return T[0]->at<0>().size;
}

bool Hamiltonian::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Hamiltonian*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Hamiltonian::clone_impl() const {
    return std::make_unique<Hamiltonian>(*this);
}

} // namespace chemist::operators

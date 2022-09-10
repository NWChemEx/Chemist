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

#include "chemist/operators/hamiltonian/hamiltonian_nuclear.hpp"
#include "chemist/operators/kinetic_nuclear.hpp"

namespace chemist::operators {

using size_type = typename Hamiltonian_Nuclear::size_type;

Hamiltonian_Nuclear::Hamiltonian_Nuclear() noexcept  = default;
Hamiltonian_Nuclear::~Hamiltonian_Nuclear() noexcept = default;

Hamiltonian_Nuclear::Hamiltonian_Nuclear(const Hamiltonian_Nuclear& other) =
  default;
Hamiltonian_Nuclear::Hamiltonian_Nuclear(Hamiltonian_Nuclear&& other) noexcept =
  default;
Hamiltonian_Nuclear& Hamiltonian_Nuclear::operator=(
  const Hamiltonian_Nuclear& other) = default;

Hamiltonian_Nuclear& Hamiltonian_Nuclear::operator=(
  Hamiltonian_Nuclear&& other) noexcept = default;

/*
size_type Hamiltonian_Nuclear::nelectrons() const noexcept {
    using T_type = NElectronKinetic_Nuclear;
    if(!has_term<T_type>()) return 0;
    auto T = get_terms<T_type>();
    return T[0]->at<0>().size;
}
*/

bool Hamiltonian_Nuclear::is_equal_impl(
  const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Hamiltonian_Nuclear*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Hamiltonian_Nuclear::clone_impl() const {
    return std::make_unique<Hamiltonian_Nuclear>(*this);
}

} // namespace chemist::operators

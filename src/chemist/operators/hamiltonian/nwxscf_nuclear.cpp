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

#include "chemist/operators/hamiltonian/nwxscf_nuclear.hpp"
#include "chemist/operators/kinetic_nuclear.hpp"

namespace chemist::operators {

using size_type = typename NWXSCF_Nuclear::size_type;

NWXSCF_Nuclear::NWXSCF_Nuclear() noexcept  = default;
NWXSCF_Nuclear::~NWXSCF_Nuclear() noexcept = default;

NWXSCF_Nuclear::NWXSCF_Nuclear(const NWXSCF_Nuclear& other)     = default;
NWXSCF_Nuclear::NWXSCF_Nuclear(NWXSCF_Nuclear&& other) noexcept = default;
NWXSCF_Nuclear& NWXSCF_Nuclear::operator=(const NWXSCF_Nuclear& other) =
  default;

NWXSCF_Nuclear& NWXSCF_Nuclear::operator=(NWXSCF_Nuclear&& other) noexcept =
  default;

bool NWXSCF_Nuclear::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const NWXSCF_Nuclear*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> NWXSCF_Nuclear::clone_impl() const {
    return std::make_unique<NWXSCF_Nuclear>(*this);
}

} // namespace chemist::operators

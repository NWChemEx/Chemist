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

#include "chemist/operators/fock.hpp"

namespace chemist::operators {

Fock::Fock() noexcept  = default;
Fock::~Fock() noexcept = default;

Fock::Fock(const Fock& other)     = default;
Fock::Fock(Fock&& other) noexcept = default;
Fock& Fock::operator=(const Fock& other) = default;
Fock& Fock::operator=(Fock&& other) noexcept = default;

bool Fock::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Fock*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Fock::clone_impl() const {
    return std::make_unique<Fock>(*this);
}

} // namespace chemist::operators

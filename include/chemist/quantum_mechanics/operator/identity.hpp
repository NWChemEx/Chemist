/*
 * Copyright 2024 NWChemEx-Project
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
#include <chemist/quantum_mechanics/operator/detail_/operator_impl.hpp>

namespace chemist::qm_operator {

/** @brief Represents the identity operator.
 *
 *  When the identity operator acts on a wavefunction the result is the
 *  original wavefunction unchanged. While this may not sound useful, the
 *  identity operator is needed to express overlap integrals in bra-ket form
 *  when one is required to specify an operator.
 */
class Identity : public detail_::OperatorImpl<Identity> {
private:
    /// Type of *this
    using my_type = Identity;

    /// Type *this directly derives from
    using base_type = detail_::OperatorImpl<my_type>;

public:
    /// All are implemented by OperatorImpl. This exposes them to the user.
    ///@{
    Identity() noexcept               = default;
    Identity(const Identity&)         = default;
    Identity(Identity&& rhs) noexcept = default;
    Identity& operator=(const Identity&) = default;
    Identity& operator=(Identity&& rhs) noexcept = default;
    ///@}

    /// Defaulted, nothrow dtor
    ~Identity() noexcept = default;
};

} // namespace chemist::qm_operator
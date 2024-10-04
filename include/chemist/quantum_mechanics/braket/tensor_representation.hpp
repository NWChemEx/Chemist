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
#include <chemist/quantum_mechanics/braket/detail_/braket_base.hpp>
#include <tensorwrapper/tensorwrapper.hpp>

namespace chemist::braket {

/** @brief Denotes that a BraKet is a tensor representation of an operator.
 *
 *  This class is a stub for the moment and will be fleshed out later.
 */
class TensorRepresentation : public detail_::BraKetBase<TensorRepresentation> {
private:
    /// Type of *this
    using my_type = TensorRepresentation;

    /// Type *this actually derives from
    using bra_ket_base_type = BraKetBase<my_type>;

public:
    /// Type *this evaluates to
    using result_type = tensorwrapper::Tensor;
};

} // namespace chemist::braket
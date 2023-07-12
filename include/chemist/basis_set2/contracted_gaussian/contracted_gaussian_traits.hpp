/*
 * Copyright 2023 NWChemEx-Project
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
#include <chemist/basis_set2/primitive/primitive_traits.hpp>

namespace chemist::basis_set {

/** @brief Propagates types associated with a ContractedGaussian of type
 *         @p CGType.
 *
 *  This struct uses inheritance because the total set of types needed for
 *  interacting with a ContractedGaussian is the union of the types needed for
 *  the primitives making up the ContractedGaussian plus the types exclusive
 *  to the ContractedGaussian. *
 *
 *  @tparam CGType expected to be either an instantiation of ContractedGaussian
 *                 or ContractedGaussianView.
 */
template<typename CGType>
struct ContractedGaussianTraits
  : public PrimitiveTraits<typename CGType::reference> {
    /// Type of a Primitive in a ContractedGaussian of type @p CGType
    using primitive_type = typename CGType::value_type;

    /// Type of a reference to a mutable Primitive in a ContractedGaussian
    using primitive_reference = typename CGType::reference;

    /// Type of a reference to a read-only Primitive in a ContractedGaussian
    using const_primitive_reference = typename CGType::const_reference;
};

} // namespace chemist::basis_set

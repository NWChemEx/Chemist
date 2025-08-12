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

namespace chemist::braket {

/** @brief Base class for all bra-kets that are scalars.
 *
 *  This class is a stub for the moment.
 *
 *  @tparam ElementType The type of object that the BraKet evaluates to. The
 *                      exact type will depend on the operator, but ElementType
 *                      is usually a floating-point type.
 */
template<typename ElementType>
class TensorElement : public detail_::BraKetBase<TensorElement<ElementType>> {
public:
    /// Type of the BraKet's value
    using result_type = ElementType;
};

} // namespace chemist::braket

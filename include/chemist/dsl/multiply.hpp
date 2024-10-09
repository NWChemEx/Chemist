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
#include <chemist/dsl/binary_op.hpp>
#include <type_traits>

namespace chemist::dsl {

/** @brief Represents the multiplication of two terms.
 *
 *  @tparam LHSType The type of the object on the left of the times sign.
 *  @tparam RHSType The type of the object on the right of the times sign.
 *
 *  This class is essentially a strong type over top of BinaryOp to signal
 *  that the binary operation is multiplication (or at the least represented
 *  with a times sign).
 */
template<typename LHSType, typename RHSType>
class Multiply : public BinaryOp<Multiply<LHSType, RHSType>, LHSType, RHSType> {
private:
    /// Type of *this
    using my_type = Multiply<LHSType, RHSType>;

    /// Type *this inherits from
    using op_type = BinaryOp<my_type, LHSType, RHSType>;

public:
    /// Reuse the base class's ctor
    using op_type::op_type;
};

} // namespace chemist::dsl
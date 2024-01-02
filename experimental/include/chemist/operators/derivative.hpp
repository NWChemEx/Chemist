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

#pragma once
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

/** @brief Models a derivative
 *
 *  @tparam Numerator The type of the numerator of the derivative.
 *  @tparam Denominators The type(s) of the denominator(s) of the derivative.
 */
template<typename Numerator, typename... Denominators>
class Derivative
  : public detail_::OperatorImpl<Derivative, Numerator, Denominators...> {
private:
    using base_type =
      detail_::OperatorImpl<Derivative, Numerator, Denominators...>;

public:
    Derivative()                  = default;
    Derivative(const Derivative&) = default;
    Derivative(Derivative&&)      = default;
    Derivative(Numerator p1, Denominators... ps) :
      base_type(std::move(p1), std::move(ps)...) {}
    Derivative& operator=(const Derivative&) = default;
    Derivative& operator=(Derivative&&) = default;
};

} // namespace chemist::operators

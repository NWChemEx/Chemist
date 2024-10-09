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
#include <chemist/dsl/dsl_fwd.hpp>

namespace chemist::dsl {

template<typename DerivedType>
class Term {
public:
    template<typename RHSType>
    auto operator+(RHSType&& rhs) {
        auto& lhs      = static_cast<DerivedType&>(*this);
        using no_ref_t = std::remove_reference_t<RHSType>;
        return Add<DerivedType, no_ref_t>(lhs, std::forward<RHSType>(rhs));
    }

    template<typename RHSType>
    auto operator*(RHSType&& rhs) {
        auto& lhs      = static_cast<DerivedType&>(*this);
        using no_ref_t = std::remove_reference_t<RHSType>;
        return Multiply<DerivedType, no_ref_t>(lhs, std::forward<RHSType>(rhs));
    }
};

} // namespace chemist::dsl
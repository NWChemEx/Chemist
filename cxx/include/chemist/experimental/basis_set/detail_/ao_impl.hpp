/*
 * Copyright 2026 NWChemEx-Project
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
#include <chemist/experimental/basis_set/ao.hpp>
#include <memory>

namespace chemist::experimental::detail_ {

/** @brief Code factorization for implementing AO's virtual methods.
 *
 *  Two of AO's virtual methods, clone_ and are_equal_, can be implemented
 *  knowing nothing about the derived class except its type, given that it:
 *  - defines a copy ctor, and
 *  - is value comparable.
 *  *this is that implementation, and is the AO-layer counterpart of
 *  chemist::wavefunction::detail_::WavefunctionImpl.
 *
 *  Note *this deliberately does not implement the remaining virtual methods
 *  (get_l_, get_center_, and friends). Those are the ones which genuinely
 *  depend on what the derived class stores, so there is nothing to factor.
 *
 *  @tparam DerivedType The type of the derived class *this is implementing.
 */
template<typename DerivedType>
class AOImpl : public AO {
protected:
    /// Implements clone() by calling DerivedType's copy ctor
    base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Implements are_equal by calling DerivedType's operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        const auto& lhs = downcast_();
        auto prhs       = dynamic_cast<const DerivedType*>(&rhs);
        if(prhs == nullptr) return false; // Different kinds of AO
        return lhs == (*prhs);
    }

private:
    /// Wraps downcasting *this to a read-only reference to DerivedType
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::experimental::detail_

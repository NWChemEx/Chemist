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
#include <chemist/quantum_mechanics/wavefunction/wavefunction_class.hpp>

namespace chemist::wavefunction::detail_ {

/** @brief Code factorization for implementing Wavefunction's virtual methods.
 *
 *  @tparam DerivedType The type of the derived class *this is implementing.
 *
 *  A number of the virtual functions of the Wavefunction class can be
 *  implemented by making some basic assumptions about the derived class
 *  including:
 *   - Defines a copy ctor
 *   - is value comparable
 *  and by knowing the type of the derived class. The Wavefunction Impl class
 *  is code factorization for implementing the virtual functions which fall
 *  into this category.
 */
template<typename DerivedType>
class WavefunctionImpl : public Wavefunction {
protected:
    /// Implements clone() by calling DerivedType's copy ctor
    base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Implements are_equal by calling DerivedType's operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        const auto& lhs = downcast_();
        auto prhs       = dynamic_cast<const DerivedType*>(&rhs);
        if(prhs == nullptr) return false;
        return lhs == (*prhs);
    }

private:
    /// Wraps downcasting *this to DerivedType&
    DerivedType& downcast_() { return static_cast<DerivedType&>(*this); }

    /// Wraps downcast *this to const DerivedType&
    const DerivedType& downcast_() const {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::wavefunction::detail_
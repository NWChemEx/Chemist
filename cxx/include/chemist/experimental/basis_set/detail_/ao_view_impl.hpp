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
#include <chemist/experimental/basis_set/ao_view.hpp>
#include <memory>

namespace chemist::experimental::detail_ {

/** @brief Code factorization for implementing AOView's virtual methods.
 *
 *  *this is to AOView what AOImpl is to AO, and implements the same two
 *  methods the same way, plus as_ao_. The derived class must:
 *  - define a copy ctor, which for a view is shallow,
 *  - be value comparable, and
 *  - define an `ao_type` alias naming the owning AO it materializes into,
 *    along with a nullary method named by that type which returns one by
 *    value (e.g. CartesianAOView::as_cartesian_ao).
 *
 *  Note that clone_ needing no special handling for a view is a consequence of
 *  the derived class's copy ctor already having the right semantics: copying a
 *  CartesianAO is a deep copy and copying a CartesianAOView is a shallow one,
 *  so delegating to it gives AO::clone and AOView::clone their respectively
 *  documented behaviors without either mixin knowing which it is doing.
 *
 *  @tparam DerivedType The type of the derived class *this is implementing.
 */
template<typename DerivedType>
class AOViewImpl : public AOView {
protected:
    /// Implements clone() by calling DerivedType's (shallow) copy ctor
    base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Implements as_ao() by materializing DerivedType into its owning AO
    ao_pointer as_ao_() const override {
        using ao_type = typename DerivedType::ao_type;
        return std::make_unique<ao_type>(downcast_().as_ao_value_());
    }

    /// Implements are_equal by calling DerivedType's operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        const auto& lhs = downcast_();
        auto prhs       = dynamic_cast<const DerivedType*>(&rhs);
        if(prhs == nullptr) return false; // Views of different kinds of AO
        return lhs == (*prhs);
    }

private:
    /// Wraps downcasting *this to a read-only reference to DerivedType
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::experimental::detail_

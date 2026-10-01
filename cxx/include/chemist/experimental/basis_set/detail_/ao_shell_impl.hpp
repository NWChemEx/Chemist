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
#include <chemist/experimental/basis_set/ao_shell.hpp>
#include <memory>

namespace chemist::experimental::detail_ {

/** @brief Code factorization for implementing AOShell's virtual methods.
 *
 *  *this is to AOShell what AOImpl is to AO: it implements the virtual methods
 *  which can be written knowing nothing about the derived class except its
 *  type. The derived class must:
 *  - define a copy ctor, and
 *  - be value comparable.
 *
 *  as_view_ is deliberately NOT implemented here, even though it too only
 *  needs the derived type (it would wrap *this in DerivedType's view). The
 *  view's header necessarily includes the derived class's header, so the
 *  view is incomplete wherever the derived class is defined. The derived
 *  class instead declares as_view_ itself and arranges for its view to be
 *  complete by the time it is instantiated (see cca_shell_class.hpp).
 *
 *  @tparam DerivedType The type of the derived class *this is implementing.
 */
template<typename DerivedType>
class AOShellImpl : public AOShell {
protected:
    /// Implements clone() by calling DerivedType's copy ctor
    base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Implements are_equal by calling DerivedType's operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        const auto& lhs = downcast_();
        auto prhs       = dynamic_cast<const DerivedType*>(&rhs);
        if(prhs == nullptr) return false; // Different orderings
        return lhs == (*prhs);
    }

private:
    /// Wraps downcasting *this to a read-only reference to DerivedType
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::experimental::detail_

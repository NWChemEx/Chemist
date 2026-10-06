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
#include <chemist/experimental/basis_set/ao_shell_view.hpp>
#include <memory>

namespace chemist::experimental::detail_ {

/** @brief Code factorization for implementing AOShellBaseView's virtual
 *         methods.
 *
 *  *this is to AOShellBaseView what AOViewImpl is to AOView. It derives from
 *  AOShellView<AOType>, so the derived class is a view of a shell of known
 *  purity. The derived class must:
 *  - define a copy ctor, which for a view is shallow,
 *  - be value comparable, and
 *  - define a `shell_type` alias naming the owning shell it materializes into,
 *    along with a private `as_shell_value_()` method returning one by value.
 *
 *  @tparam DerivedType The type of the derived class *this is implementing.
 *  @tparam AOType The kind of AO the derived class holds: CartesianAO or
 *                 SphericalAO.
 */
template<typename DerivedType, typename AOType>
class AOShellViewImpl : public AOShellView<AOType> {
private:
    /// Type of a pointer to the polymorphic, purity-agnostic base
    using base_pointer = typename AOShellBaseView::base_pointer;

    /// Type of a read-only reference to the polymorphic, purity-agnostic base
    using const_base_reference = typename AOShellBaseView::const_base_reference;

    /// Type of a pointer to the polymorphic, purity-agnostic owning shell
    using base_shell_pointer = typename AOShellBaseView::shell_pointer;

protected:
    /// Implements clone() by calling DerivedType's (shallow) copy ctor
    base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Implements as_shell() by materializing DerivedType into its owning
    /// shell
    base_shell_pointer as_shell_() const override {
        using shell_type = typename DerivedType::shell_type;
        return std::make_unique<shell_type>(downcast_().as_shell_value_());
    }

    /// Implements are_equal by calling DerivedType's operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        const auto& lhs = downcast_();
        auto prhs       = dynamic_cast<const DerivedType*>(&rhs);
        if(prhs == nullptr) return false; // Views of different orderings
        return lhs == (*prhs);
    }

private:
    /// Wraps downcasting *this to a read-only reference to DerivedType
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::experimental::detail_

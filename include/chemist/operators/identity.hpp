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
#include "chemist/electrons/electrons.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

/** @brief Models the identity function
 *
 *  @tparam Particles The type(s) of the entities identified by the function.
 */
template<typename... Particles>
class Identity : public detail_::OperatorImpl<Identity, Particles...> {
private:
    using base_type = detail_::OperatorImpl<Identity, Particles...>;

public:
    Identity()                = default;
    Identity(const Identity&) = default;
    Identity(Identity&&)      = default;
    Identity(Particles... ps) : base_type(std::move(ps)...) {}
    Identity& operator=(const Identity&) = default;
    Identity& operator=(Identity&&) = default;

protected:
    /// Gives a more traditional representation than the default implementation
    std::string as_string_impl() const { return "I\u0302"; }
};

/// The type of the electronic identity function
using ElectronIdentity = Identity<chemist::Electron>;

} // namespace chemist::operators

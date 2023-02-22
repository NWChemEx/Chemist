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
#include <chemist/integral_factory/integral_factory.hpp>

namespace chemist::detail_ {

class IntegralFactoryPIMPL {
private:
    using parent_t = IntegralFactory;

public:
    /// Type of this instance
    using pimpl_t = typename parent_t::pimpl_t;

    /// Type of the pimpl
    using pimpl_ptr_t = typename parent_t::pimpl_ptr_t;

    /// Type of the shell indices
    using indices_t = typename parent_t::indices_t;

    ///
    using buffer_t = typename parent_t::buffer_t;

    virtual ~IntegralFactoryPIMPL() noexcept = default;

    pimpl_ptr_t clone() const { return clone_(); }

    bool are_equal(const IntegralFactoryPIMPL& rhs) const noexcept {
        return are_equal_(rhs);
    };

    const buffer_t& compute(const indices_t& indices) const {
        return compute_(indices);
    }

protected:
    IntegralFactoryPIMPL()                             = default;
    IntegralFactoryPIMPL(const IntegralFactoryPIMPL&)  = default;
    IntegralFactoryPIMPL(IntegralFactoryPIMPL&& other) = default;
    IntegralFactoryPIMPL& operator=(const IntegralFactoryPIMPL& rhs) = default;
    IntegralFactoryPIMPL& operator=(IntegralFactoryPIMPL&& rhs) = default;

private:
    /// To be overridden by the derived class to implement clone()
    virtual pimpl_ptr_t clone_() const = 0;

    /// To be overridden by the derived class to implement compute()
    virtual const buffer_t& compute_(const indices_t& indices) const = 0;

    /// To be overridden by the derived class to implement compute()
    virtual bool are_equal_(const IntegralFactoryPIMPL& rhs) const = 0;
};

} // namespace chemist::detail_
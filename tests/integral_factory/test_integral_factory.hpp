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

#include "chemist/integral_factory/integral_factory.hpp"
#include "chemist/integral_factory/integral_factory_pimpl.hpp"
#include <catch2/catch.hpp>

namespace testing {

/// Minimum derived class for testing IntegralFactoryPIMPL
class TestPIMPL : public chemist::IntegralFactoryPIMPL {
public:
    using base_t      = chemist::IntegralFactoryPIMPL;
    using pimpl_ptr_t = typename base_t::pimpl_ptr_t;
    using indices_t   = typename base_t::indices_t;
    using buffer_t    = typename base_t::buffer_t;

    TestPIMPL() = default;
    TestPIMPL(buffer_t b) : m_buffer(b) {}

    buffer_t m_buffer;

private:
    pimpl_ptr_t clone_() const { return std::make_unique<TestPIMPL>(m_buffer); }

    const buffer_t& compute_(const indices_t& indices) const {
        return m_buffer;
    }

    bool are_equal_(const IntegralFactoryPIMPL& rhs) const {
        auto ptr = dynamic_cast<const TestPIMPL*>(&rhs);
        if(ptr == nullptr) return false;
        return m_buffer == ptr->m_buffer;
    }
};

} // namespace testing
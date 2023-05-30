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

#include "test_integral_factory.hpp"

TEST_CASE("IntegralFactoryPIMPL") {
    using test_t = chemist::IntegralFactoryPIMPL;
    using fake_t = testing::TestPIMPL;

    using pimpl_t     = typename test_t::pimpl_t;
    using pimpl_ptr_t = typename test_t::pimpl_ptr_t;
    using indices_t   = typename test_t::indices_t;
    using buffer_t    = typename test_t::buffer_t;

    double z = 0.0;
    double y = 0.0;
    double* a = &z;
    double* b = &y;
    buffer_t buffer1{a};
    buffer_t buffer2{b};

    pimpl_ptr_t pimpl1 = std::make_unique<fake_t>(buffer1);
    pimpl_ptr_t pimpl2 = std::make_unique<fake_t>(buffer1);
    pimpl_ptr_t pimpl3 = std::make_unique<fake_t>(buffer2);

    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<pimpl_t, test_t>);
        REQUIRE(std::is_same_v<pimpl_ptr_t, std::unique_ptr<test_t>>);
        REQUIRE(std::is_same_v<indices_t, std::vector<std::size_t>>);
        REQUIRE(std::is_same_v<buffer_t, std::vector<const double*>>);
    }

    SECTION("clone") {
        auto clone_ptr      = pimpl1->clone();
        auto clone_fake_ptr = dynamic_cast<const fake_t*>(clone_ptr.get());
        REQUIRE(clone_fake_ptr->m_buffer == buffer1);
    }

    SECTION("are_equal") {
        REQUIRE(pimpl1->are_equal(*pimpl2));
        REQUIRE_FALSE(pimpl1->are_equal(*pimpl3));
    }

    SECTION("compute") {
        auto compute_return = pimpl3->compute(indices_t{});
        REQUIRE(compute_return == buffer2);
    }
}

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
#include <iostream>

TEST_CASE("IntegralFactory") {
    using test_t = chemist::IntegralFactory;
    using real_t = chemist::IntegralFactoryPIMPL;
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
    pimpl_ptr_t pimpl2 = std::make_unique<fake_t>(buffer2);

    test_t defaulted;
    test_t factory1(std::move(pimpl1));

    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<pimpl_t, real_t>);
        REQUIRE(std::is_same_v<pimpl_ptr_t, std::unique_ptr<real_t>>);
        REQUIRE(std::is_same_v<indices_t, std::vector<std::size_t>>);
        REQUIRE(std::is_same_v<buffer_t, std::vector<const double*>>);
    }

    SECTION("Default Ctor") { REQUIRE(defaulted == test_t{}); }

    SECTION("PIMPL Ctor") {
        REQUIRE(factory1 == test_t{std::make_unique<fake_t>(buffer1)});
    }

    SECTION("Copy Ctor") {
        test_t copied_to(factory1);
        REQUIRE(copied_to == factory1);
    }

    SECTION("Move Ctor") {
        test_t moved_to(std::move(factory1));
        REQUIRE(moved_to == test_t{std::make_unique<fake_t>(buffer1)});
        REQUIRE(factory1 == defaulted);
    }

    SECTION("Copy Assignment") {
        test_t copy_assigned;
        copy_assigned = factory1;
        REQUIRE(copy_assigned == factory1);
    }

    SECTION("Move Assignment") {
        test_t move_assigned;
        move_assigned = std::move(factory1);
        REQUIRE(move_assigned == test_t{std::make_unique<fake_t>(buffer1)});
        REQUIRE(factory1 == defaulted);
    }

    SECTION("compute") {
        auto check1 = factory1.compute(indices_t{});
        REQUIRE(check1 == buffer1);
    }

    SECTION("comparisons") {
        REQUIRE(defaulted == test_t{});
        REQUIRE(factory1 == test_t{std::make_unique<fake_t>(buffer1)});
        REQUIRE_FALSE(defaulted == factory1);
        REQUIRE_FALSE(factory1 == test_t{std::make_unique<fake_t>(buffer2)});

        REQUIRE_FALSE(defaulted != test_t{});
        REQUIRE_FALSE(factory1 != test_t{std::make_unique<fake_t>(buffer1)});
        REQUIRE(defaulted != factory1);
        REQUIRE(factory1 != test_t{std::make_unique<fake_t>(buffer2)});
    }
}

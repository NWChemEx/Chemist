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

#include "../catch.hpp"
#include <chemist/dsl/add.hpp>
#include <chemist/dsl/term_traits.hpp>

using namespace chemist;

/* Testing strategy.
 *
 * Our goal here is to verify that we implemented the TMP in the TermTraits
 * class correctly. That really depends on whether or not the template type
 * parameter T:
 *
 * - is const qualified,
 * - is derived from dsl::Term,
 * - is a floating point type
 */

TEST_CASE("TermTraits<char>") {
    using traits = dsl::TermTraits<char>;
    STATIC_REQUIRE_FALSE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, char>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const char&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, char&>);
    STATIC_REQUIRE_FALSE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, char&>);
}

TEST_CASE("TermTraits<const char>") {
    using traits = dsl::TermTraits<const char>;
    STATIC_REQUIRE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, char>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const char&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, const char&>);
    STATIC_REQUIRE_FALSE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, const char&>);
}

TEST_CASE("TermTraits<double>") {
    using traits = dsl::TermTraits<double>;
    STATIC_REQUIRE_FALSE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, double>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const double&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, double&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, double>);
}

TEST_CASE("TermTraits<const double>") {
    using traits = dsl::TermTraits<const double>;
    STATIC_REQUIRE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, double>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const double&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, const double&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, double>);
}

TEST_CASE("TermTraits<Add<int, double>>") {
    using op_t   = dsl::Add<int, double>;
    using traits = dsl::TermTraits<op_t>;
    STATIC_REQUIRE_FALSE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, op_t>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const op_t&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, op_t&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, op_t>);
}

TEST_CASE("TermTraits<const Add<int, double>>") {
    using op_t   = dsl::Add<int, double>;
    using traits = dsl::TermTraits<const op_t>;
    STATIC_REQUIRE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, op_t>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const op_t&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, const op_t&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, op_t>);
}
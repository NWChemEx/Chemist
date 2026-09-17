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

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <chemist/experimental/detail_/float_serialization.hpp>
#include <chemist/types/floating_point.hpp>
#include <sstream>

using namespace chemist::experimental::detail_;
using test_chemist::as_double;

namespace {

/// Wraps writing one type-erased value to a fresh binary archive
std::stringstream save_to_binary(const_float_reference value) {
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive ar(ss);
        save_float(ar, value);
    }
    return ss;
}

/// Wraps reading one type-erased value back out of a binary archive
float_type load_from_binary(std::stringstream& ss) {
    cereal::BinaryInputArchive ar(ss);
    return load_float(ar);
}

} // namespace

TEST_CASE("experimental::detail_ float serialization") {
    float_type a_double(3.14);
    float_type a_float(2.5f);

    // long double is a floating-point type WTF knows about but chemist's
    // floating_point_types list deliberately does not include.
    float_type unknown(1.0L);

    SECTION("Round trips a double") {
        auto ss = save_to_binary(a_double);
        auto rv = load_from_binary(ss);
        REQUIRE(as_double(rv) == 3.14);
    }

    SECTION("Round trips a float") {
        auto ss = save_to_binary(a_float);
        auto rv = load_from_binary(ss);
        REQUIRE(rv.as_view().value<float>() == 2.5f);
    }

    SECTION("Restores the concrete type, not just the value") {
        // A float goes in; a float must come back out. If the archive only
        // recorded the number, this would come back as a double and the
        // cast below would succeed.
        auto ss = save_to_binary(a_float);
        auto rv = load_from_binary(ss);
        REQUIRE_THROWS_AS(rv.as_view().value<double>(), std::runtime_error);

        // ... and the two are therefore not value equal
        REQUIRE(rv != a_double);
        REQUIRE(rv != float_type(2.5));
    }

    SECTION("Round trips several values in order") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            save_float(ar, a_double);
            save_float(ar, a_float);
            save_float(ar, float_type(-1.0));
        }
        cereal::BinaryInputArchive ar(ss);
        REQUIRE(as_double(load_float(ar)) == 3.14);
        REQUIRE(load_float(ar).as_view().value<float>() == 2.5f);
        REQUIRE(as_double(load_float(ar)) == -1.0);
    }

    SECTION("Works with a text archive too") {
        std::stringstream ss;
        {
            cereal::JSONOutputArchive ar(ss);
            save_float(ar, a_double);
        }
        cereal::JSONInputArchive ar(ss);
        REQUIRE(as_double(load_float(ar)) == 3.14);
    }

    SECTION("The archive names the concrete type") {
        // The name is WTF's, which is standardized across compilers, rather
        // than typeid(T).name(), which is not.
        std::stringstream ss;
        {
            cereal::JSONOutputArchive ar(ss);
            save_float(ar, a_double);
        }
        REQUIRE(ss.str().find("double") != std::string::npos);
    }

    SECTION("save_float rejects a type chemist does not know about") {
        std::stringstream ss;
        cereal::BinaryOutputArchive ar(ss);
        REQUIRE_THROWS_AS(save_float(ar, unknown), std::runtime_error);
    }

    SECTION("load_float rejects an unknown type name") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(std::string("not a floating-point type"));
            ar(3.14);
        }
        cereal::BinaryInputArchive ar(ss);
        REQUIRE_THROWS_AS(load_float(ar), std::runtime_error);
    }

#ifdef ENABLE_SIGMA
    SECTION("Uncertainty-quantification types are refused, not truncated") {
        using udouble = tensorwrapper::types::udouble;
        float_type u(udouble(2.0, 0.1));

        // Sigma's types have no cereal support, and round-tripping them
        // through a center/radius pair would silently discard correlation
        // information. Refusing is the honest behavior.
        std::stringstream ss;
        cereal::BinaryOutputArchive ar(ss);
        REQUIRE_THROWS_AS(save_float(ar, u), std::runtime_error);
    }

    SECTION("load_float refuses an archive naming a UQ type") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(std::string(
              wtf::type_traits::type_name_v<tensorwrapper::types::udouble>));
        }
        cereal::BinaryInputArchive ar(ss);
        REQUIRE_THROWS_AS(load_float(ar), std::runtime_error);
    }
#endif
}

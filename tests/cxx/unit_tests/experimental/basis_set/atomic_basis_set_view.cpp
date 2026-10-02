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

// Only what AtomicBasisSetView itself implements is tested here, plus enough
// of the shared API to show that a view reaches the state it aliases.
// Everything else it inherits from AtomicBasisSetCommon is tested, through an
// AtomicBasisSet, in atomic_basis_set_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/atomic_basis_set.hpp>
#include <chemist/experimental/basis_set/cca_shell.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// Detects whether T has the setters
template<typename T, typename = void>
struct has_setters : std::false_type {};

template<typename T>
struct has_setters<T, std::void_t<decltype(std::declval<T&>().set_name(""))>>
  : std::true_type {};

} // namespace

TEMPLATE_TEST_CASE("experimental::AtomicBasisSetView", "", CartesianAO,
                   SphericalAO) {
    constexpr bool pure = std::is_same_v<TestType, SphericalAO>;
    const auto purity   = pure ? ShellPurity::pure : ShellPurity::cartesian;

    std::vector<double> s_cs{0.5, 0.25}, s_es{3.0, 0.5};
    std::vector<double> p_cs{1.0}, p_es{0.8};
    Point r0(1.0, 2.0, 3.0);

    AtomicBasisSet abs("cc-pVDZ", 8, r0, purity);
    abs.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
    abs.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end());

    // Same shape as abs, different values everywhere
    std::vector<double> s_cs2{1.5, 1.25}, s_es2{4.0, 1.5};
    std::vector<double> p_cs2{2.0}, p_es2{1.8};
    AtomicBasisSet other("STO-3G", 1, Point(4.0, 5.0, 6.0), purity);
    other.add_shell(1, s_cs2.begin(), s_cs2.end(), s_es2.begin(), s_es2.end());
    other.add_shell(2, p_cs2.begin(), p_cs2.end(), p_es2.begin(), p_es2.end());

    atomic_basis_set_view v(abs);
    const_atomic_basis_set_view cv(abs);

    SECTION("Only the mutable view has setters") {
        STATIC_REQUIRE(has_setters<atomic_basis_set_view>::value);
        STATIC_REQUIRE_FALSE(has_setters<const_atomic_basis_set_view>::value);
        STATIC_REQUIRE(std::is_same_v<decltype(cv.primitive(0)),
                                      PrimitiveView<const Primitive>>);
    }

    SECTION("Ctors") {
        SECTION("From an AtomicBasisSet") {
            REQUIRE(v == abs);
            REQUIRE(cv == abs);
            REQUIRE(const_atomic_basis_set_view(std::as_const(abs)) == abs);
        }

        SECTION("Mutable to read-only") {
            const_atomic_basis_set_view converted(v);
            REQUIRE(converted == abs);
            abs.set_name("STO-3G");
            REQUIRE(converted.get_name() == "STO-3G");
        }

        SECTION("Copy is shallow") {
            atomic_basis_set_view copy(v);
            REQUIRE(copy == abs);
            copy.set_atomic_number(1);
            REQUIRE(abs.get_atomic_number() == 1);
            REQUIRE(v.get_atomic_number() == 1);
        }

        SECTION("Move") {
            atomic_basis_set_view copy(v);
            atomic_basis_set_view moved(std::move(copy));
            REQUIRE(moved == abs);
        }

        SECTION("From a null implementation") {
            using pimpl_pointer = typename atomic_basis_set_view::pimpl_pointer;
            REQUIRE_THROWS_AS(atomic_basis_set_view(pimpl_pointer{}),
                              std::invalid_argument);
        }
    }

    SECTION("The view aliases the set") {
        // Writes through the set are visible through the views ...
        abs.set_center(Point(4.0, 5.0, 6.0));
        abs.primitive(2).set_exponent(9.0);
        REQUIRE(cv.get_center() == Point(4.0, 5.0, 6.0));
        REQUIRE(as_double(cv.primitive(2).get_exponent()) == 9.0);
        REQUIRE(cv.at(1)->get_center() == Point(4.0, 5.0, 6.0));

        // ... and writes through the mutable view land in the set
        v.set_name("STO-3G");
        v.set_l(1, 3);
        v.primitive(0).set_coefficient(42.0);
        REQUIRE(abs.get_name() == "STO-3G");
        REQUIRE(abs.get_l(1) == 3);
        REQUIRE(as_double(abs.primitive(0).get_coefficient()) == 42.0);
    }

    SECTION("Assignment writes through") {
        SECTION("From an AtomicBasisSet") {
            auto pv = &(v = other);
            REQUIRE(pv == &v);
        }
        SECTION("From a view") {
            atomic_basis_set_view ov(other);
            v = ov;
        }
        SECTION("From a read-only view") {
            v = const_atomic_basis_set_view(other);
        }
        REQUIRE(abs == other);
        // The views were not rebound: other is untouched by writes to abs
        abs.set_name("cc-pVDZ");
        REQUIRE(other.get_name() == "STO-3G");
    }

    SECTION("Assignment can not change the shape") {
        AtomicBasisSet copy(abs);
        SECTION("Number of shells") {
            other.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(),
                            s_es.end());
        }
        SECTION("Number of primitives in a shell") {
            AtomicBasisSet rhs("", 0, r0, purity);
            rhs.add_shell(0, p_cs.begin(), p_cs.end(), p_es.begin(),
                          p_es.end());
            rhs.add_shell(1, s_cs.begin(), s_cs.end(), s_es.begin(),
                          s_es.end());
            other = rhs;
        }
        SECTION("Purity") {
            AtomicBasisSet rhs(
              "", 0, r0, pure ? ShellPurity::cartesian : ShellPurity::pure);
            rhs.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(),
                          s_es.end());
            rhs.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(),
                          p_es.end());
            other = rhs;
        }
        REQUIRE_THROWS_AS(v = other, std::runtime_error);
        // Strong throw guarantee: the shape is checked before writing
        REQUIRE(abs == copy);
    }

    SECTION("as_atomic_basis_set") {
        auto copy = cv.as_atomic_basis_set();
        STATIC_REQUIRE(std::is_same_v<decltype(copy), AtomicBasisSet>);
        REQUIRE(copy == abs);
        copy.set_name("STO-3G");
        copy.primitive(0).set_coefficient(42.0);
        REQUIRE(abs.get_name() == "cc-pVDZ");
        REQUIRE(as_double(abs.primitive(0).get_coefficient()) == 0.5);

        REQUIRE(v.as_atomic_basis_set() == abs);
    }

    SECTION("swap rebinds") {
        atomic_basis_set_view ov(other);
        v.swap(ov);
        REQUIRE(v == other);
        REQUIRE(ov == abs);
        v.set_name("changed");
        REQUIRE(other.get_name() == "changed");
        REQUIRE(abs.get_name() == "cc-pVDZ");
    }
}

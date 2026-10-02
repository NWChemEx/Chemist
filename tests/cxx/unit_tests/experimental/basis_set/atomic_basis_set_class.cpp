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

// Only what AtomicBasisSet itself implements is tested here: its ctors,
// assignment, adding shells, swap, and serialization. Everything it inherits
// from AtomicBasisSetCommon is tested in atomic_basis_set_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/atomic_basis_set_class.hpp>
#include <chemist/experimental/basis_set/cca_shell.hpp>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEMPLATE_TEST_CASE("experimental::AtomicBasisSet", "", CartesianAO,
                   SphericalAO) {
    using shell_type    = CCAShell<TestType>;
    using size_type     = std::size_t;
    constexpr bool pure = std::is_same_v<TestType, SphericalAO>;
    const auto purity   = pure ? ShellPurity::pure : ShellPurity::cartesian;
    using other_shell_type =
      CCAShell<std::conditional_t<pure, CartesianAO, SphericalAO>>;

    std::vector<double> s_cs{0.5, 0.25}, s_es{3.0, 0.5};
    std::vector<double> p_cs{1.0}, p_es{0.8};
    Point r0(1.0, 2.0, 3.0);

    shell_type s(s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end(),
                 size_type(0), r0);
    shell_type p(p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end(),
                 size_type(1), r0);

    AtomicBasisSet abs("cc-pVDZ", 8, r0, purity);
    abs.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
    abs.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end());

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            AtomicBasisSet defaulted;
            REQUIRE(defaulted.empty());
            REQUIRE(defaulted.get_name().empty());
            REQUIRE(defaulted.get_atomic_number() == 0);
            REQUIRE(defaulted.get_center() == Point(0.0, 0.0, 0.0));
            REQUIRE(defaulted.purity() == ShellPurity::cartesian);
            REQUIRE(defaulted.ordering() == AOOrdering::cca);
        }

        SECTION("Value") {
            AtomicBasisSet empty("cc-pVDZ", 8, r0, purity);
            REQUIRE(empty.empty());
            REQUIRE(empty.get_name() == "cc-pVDZ");
            REQUIRE(empty.get_atomic_number() == 8);
            REQUIRE(empty.get_center() == r0);
            REQUIRE(empty.purity() == purity);
            REQUIRE(empty.ordering() == AOOrdering::cca);

            // The purity and ordering default to Cartesian and CCA
            AtomicBasisSet defaults("cc-pVDZ", 8, r0);
            REQUIRE(defaults.purity() == ShellPurity::cartesian);
            REQUIRE(defaults.ordering() == AOOrdering::cca);
        }

        SECTION("Copy is deep") {
            AtomicBasisSet copy(abs);
            REQUIRE(copy == abs);
            copy.set_center(Point(9.0, 9.0, 9.0));
            copy.primitive(0).set_coefficient(42.0);
            REQUIRE(abs.get_center() == r0);
            REQUIRE(as_double(abs.primitive(0).get_coefficient()) == 0.5);
        }

        SECTION("Move") {
            AtomicBasisSet copy(abs);
            AtomicBasisSet moved(std::move(copy));
            REQUIRE(moved == abs);
        }

        SECTION("Copy assignment") {
            AtomicBasisSet copy;
            auto pcopy = &(copy = abs);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == abs);
            copy.set_name("STO-3G");
            REQUIRE(abs.get_name() == "cc-pVDZ");
        }

        SECTION("Move assignment") {
            AtomicBasisSet copy(abs);
            AtomicBasisSet moved;
            auto pmoved = &(moved = std::move(copy));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved == abs);
        }
    }

    SECTION("add_shell") {
        REQUIRE(abs.size() == 2);
        REQUIRE(abs.get_l(1) == 1);
        REQUIRE(abs.n_primitives() == 3);

        SECTION("Mismatched lengths") {
            REQUIRE_THROWS_AS(abs.add_shell(2, s_cs.begin(), s_cs.end(),
                                            p_es.begin(), p_es.end()),
                              std::invalid_argument);
            REQUIRE(abs.size() == 2);
            REQUIRE(abs.n_primitives() == 3);
        }

        SECTION("Mismatched floating-point types are rejected atomically") {
            // The coefficients go in fine; the exponents do not. Nothing
            // must change.
            std::vector<double> cs{1.0};
            std::vector<float> es{1.0f};
            REQUIRE_THROWS_AS(
              abs.add_shell(2, cs.begin(), cs.end(), es.begin(), es.end()),
              std::runtime_error);
            REQUIRE(abs.size() == 2);
            REQUIRE(abs.n_primitives() == 3);
            REQUIRE(abs.get_coefficient_buffer().size() == 3);
        }
    }

    SECTION("push_back") {
        AtomicBasisSet built("cc-pVDZ", 8, r0, purity);

        SECTION("Shells") {
            built.push_back(s);
            built.push_back(p);
            REQUIRE(built == abs);
        }

        SECTION("Shell views") {
            built.push_back(CCAShellView<const shell_type>(s));
            built.push_back(CCAShellView<shell_type>(p));
            REQUIRE(built == abs);
        }

        SECTION("The shells are copied, not aliased") {
            built.push_back(s);
            s.get_contracted_gaussian()[0].set_coefficient(42.0);
            REQUIRE(as_double(built.primitive(0).get_coefficient()) == 0.5);
        }

        SECTION("A shell of the set itself") {
            // The new shell's parameters are read out of the state being
            // reallocated.
            abs.push_back(*abs.at(0));
            REQUIRE(abs.size() == 3);
            REQUIRE(abs.at(2)->are_equal(*abs.at(0)));
        }

        SECTION("A shell of the wrong purity") {
            other_shell_type other(s_cs.begin(), s_cs.end(), s_es.begin(),
                                   s_es.end(), size_type(0), r0);
            REQUIRE_THROWS_AS(built.push_back(other), std::invalid_argument);
            REQUIRE(built.empty());
        }

        SECTION("A shell on a different center") {
            shell_type moved(s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end(),
                             size_type(0), Point(0.0, 0.0, 0.0));
            REQUIRE_THROWS_AS(built.push_back(moved), std::invalid_argument);
            REQUIRE(built.empty());
        }
    }

    SECTION("swap") {
        AtomicBasisSet copy(abs);
        AtomicBasisSet other;
        abs.swap(other);
        REQUIRE(other == copy);
        REQUIRE(abs == AtomicBasisSet());
    }

    SECTION("save/load") {
        auto round_trip = [](const AtomicBasisSet& input) {
            std::stringstream ss;
            {
                cereal::BinaryOutputArchive ar(ss);
                ar(input);
            }
            AtomicBasisSet output;
            {
                cereal::BinaryInputArchive ar(ss);
                ar(output);
            }
            return output;
        };
        REQUIRE(round_trip(abs) == abs);
        REQUIRE(round_trip(AtomicBasisSet()) == AtomicBasisSet());
        REQUIRE(round_trip(AtomicBasisSet("", 1, r0, purity)).purity() ==
                purity);
    }
}

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

// Only what MolecularBasisSetView itself implements is tested here, plus
// enough of the shared API to show that a view reaches the state it aliases.
// Everything else it inherits from MolecularBasisSetCommon is tested, through
// a MolecularBasisSet, in molecular_basis_set_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/molecular_basis_set.hpp>
#include <span>
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
struct has_setters<T, std::void_t<decltype(std::declval<T&>().set_l(0, 0))>>
  : std::true_type {};

} // namespace

TEST_CASE("experimental::MolecularBasisSetView") {
    std::vector<double> s_cs{0.5, 0.25}, s_es{3.0, 0.5};
    std::vector<double> p_cs{1.0}, p_es{0.8};
    Point r_o(0.0, 0.0, 0.0), r_h(0.0, 0.0, 1.8);

    AtomicBasisSet o("cc-pVDZ", 8, r_o, ShellPurity::cartesian);
    o.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
    o.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end());
    AtomicBasisSet h("STO-3G", 1, r_h, ShellPurity::pure);
    h.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
    MolecularBasisSet mbs{o, h};

    // Same shape as mbs, different values everywhere
    std::vector<double> s_cs2{1.5, 1.25}, s_es2{4.0, 1.5};
    std::vector<double> p_cs2{2.0}, p_es2{1.8};
    AtomicBasisSet o2("6-31G", 9, Point(1.0, 1.0, 1.0), ShellPurity::cartesian);
    o2.add_shell(1, s_cs2.begin(), s_cs2.end(), s_es2.begin(), s_es2.end());
    o2.add_shell(2, p_cs2.begin(), p_cs2.end(), p_es2.begin(), p_es2.end());
    AtomicBasisSet h2("6-31G", 2, Point(2.0, 2.0, 2.0), ShellPurity::pure);
    h2.add_shell(1, s_cs2.begin(), s_cs2.end(), s_es2.begin(), s_es2.end());
    MolecularBasisSet other{o2, h2};

    molecular_basis_set_view v(mbs);
    const_molecular_basis_set_view cv(mbs);

    SECTION("Only the mutable view has setters") {
        STATIC_REQUIRE(has_setters<molecular_basis_set_view>::value);
        STATIC_REQUIRE_FALSE(
          has_setters<const_molecular_basis_set_view>::value);
        STATIC_REQUIRE(std::is_same_v<decltype(v[0]), atomic_basis_set_view>);
        STATIC_REQUIRE(
          std::is_same_v<decltype(cv[0]), const_atomic_basis_set_view>);
        STATIC_REQUIRE(std::is_same_v<decltype(cv.primitive(0)),
                                      PrimitiveView<const Primitive>>);
    }

    SECTION("Ctors") {
        SECTION("From a MolecularBasisSet") {
            REQUIRE(v == mbs);
            REQUIRE(cv == mbs);
            REQUIRE(const_molecular_basis_set_view(std::as_const(mbs)) == mbs);
            REQUIRE(molecular_basis_set_view(other) == other);
        }

        SECTION("From an empty MolecularBasisSet") {
            MolecularBasisSet empty;
            const_molecular_basis_set_view ev(empty);
            REQUIRE(ev.empty());
            REQUIRE(ev.n_shells() == 0);
            REQUIRE(ev == empty);
        }

        SECTION("Mutable to read-only") {
            const_molecular_basis_set_view converted(v);
            REQUIRE(converted == mbs);
            mbs[0].set_name("STO-3G");
            REQUIRE(converted[0].get_name() == "STO-3G");
        }

        SECTION("Copy is shallow") {
            molecular_basis_set_view copy(v);
            REQUIRE(copy == mbs);
            copy[1].set_atomic_number(2);
            REQUIRE(mbs[1].get_atomic_number() == 2);
            REQUIRE(v[1].get_atomic_number() == 2);
        }

        SECTION("Move") {
            molecular_basis_set_view copy(v);
            molecular_basis_set_view moved(std::move(copy));
            REQUIRE(moved == mbs);
        }

        SECTION("From the state") {
            // Hand-built state for one atom with one shell of two primitives.
            // The offsets need not start at zero.
            PointSet centers{r_o};
            std::vector<std::size_t> ls{0};
            std::vector<std::size_t> prim_offsets{5, 7};
            std::vector<std::size_t> shell_offsets{3, 4};
            std::vector<std::string> names{"cc-pVDZ"};
            std::vector<std::size_t> zs{8};
            std::vector<ShellPurity> purities{ShellPurity::cartesian};
            std::vector<AOOrdering> orderings{AOOrdering::cca};
            auto cs  = o.get_coefficient_buffer();
            auto es  = o.get_exponent_buffer();
            auto cs2 = detail_::slice_buffer(cs, 0, 2);
            auto es2 = detail_::slice_buffer(es, 0, 2);

            molecular_basis_set_view built(cs2, es2, ls, prim_offsets,
                                           shell_offsets, centers, names, zs,
                                           purities, orderings);
            REQUIRE(built.size() == 1);
            REQUIRE(built.n_shells() == 1);
            REQUIRE(built.n_primitives() == 2);
            REQUIRE(built.shell_range(0) ==
                    typename MolecularBasisSet::range_type{0, 1});
            REQUIRE(built.primitive_range(0) ==
                    typename MolecularBasisSet::range_type{0, 2});
            REQUIRE(built[0].get_name() == "cc-pVDZ");
            REQUIRE(built[0].size() == 1);
            REQUIRE(built.shell(0)->are_equal(*o.at(0)));

            SECTION("Inconsistent state is rejected") {
                std::vector<std::size_t> too_many{3, 4, 4};
                REQUIRE_THROWS_AS(molecular_basis_set_view(
                                    cs2, es2, ls, prim_offsets, too_many,
                                    centers, names, zs, purities, orderings),
                                  std::invalid_argument);
                std::vector<std::size_t> wrong_prims{5, 8};
                REQUIRE_THROWS_AS(molecular_basis_set_view(
                                    cs2, es2, ls, wrong_prims, shell_offsets,
                                    centers, names, zs, purities, orderings),
                                  std::invalid_argument);
                std::vector<std::size_t> no_zs;
                REQUIRE_THROWS_AS(molecular_basis_set_view(
                                    cs2, es2, ls, prim_offsets, shell_offsets,
                                    centers, names, no_zs, purities, orderings),
                                  std::invalid_argument);
            }
        }
    }

    SECTION("The view aliases the set") {
        // Writes through the set are visible through the views ...
        mbs.get_centers()[1] = Point(4.0, 5.0, 6.0);
        mbs.primitive(3).set_exponent(9.0);
        REQUIRE(cv[1].get_center() == Point(4.0, 5.0, 6.0));
        REQUIRE(as_double(cv.primitive(3).get_exponent()) == 9.0);
        REQUIRE(cv.shell(2)->get_center() == Point(4.0, 5.0, 6.0));

        // ... and writes through the mutable view land in the set
        v[0].set_name("STO-3G");
        v.set_l(1, 3);
        v.primitive(0).set_coefficient(42.0);
        REQUIRE(mbs[0].get_name() == "STO-3G");
        REQUIRE(mbs.get_l(1) == 3);
        REQUIRE(as_double(mbs.primitive(0).get_coefficient()) == 42.0);
    }

    SECTION("Assignment writes through") {
        SECTION("From a MolecularBasisSet") {
            auto pv = &(v = other);
            REQUIRE(pv == &v);
        }
        SECTION("From a view") {
            molecular_basis_set_view ov(other);
            v = ov;
        }
        SECTION("From a read-only view") {
            v = const_molecular_basis_set_view(other);
        }
        REQUIRE(mbs == other);
        // The views were not rebound: other is untouched by writes to mbs
        mbs[0].set_name("cc-pVDZ");
        REQUIRE(other[0].get_name() == "6-31G");
    }

    SECTION("Assignment can not change the shape") {
        MolecularBasisSet copy(mbs);
        SECTION("Number of atoms") { other.push_back(h2); }
        SECTION("Number of shells on an atom") {
            other = MolecularBasisSet{o2, o2};
        }
        SECTION("Number of shells on each atom, but not in total") {
            // 1 + 2 shells rather than 2 + 1
            AtomicBasisSet h3("", 1, r_h, ShellPurity::cartesian);
            h3.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
            AtomicBasisSet o3("", 8, r_o, ShellPurity::pure);
            o3.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
            o3.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end());
            other = MolecularBasisSet{h3, o3};
        }
        SECTION("Number of primitives in a shell") {
            AtomicBasisSet o3("", 8, r_o, ShellPurity::cartesian);
            o3.add_shell(0, p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end());
            o3.add_shell(1, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
            other = MolecularBasisSet{o3, h2};
        }
        SECTION("Purity of an atom") {
            AtomicBasisSet h3("", 1, r_h, ShellPurity::cartesian);
            h3.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
            other = MolecularBasisSet{o2, h3};
        }
        REQUIRE_THROWS_AS(v = other, std::runtime_error);
        // Strong throw guarantee: the shape is checked before writing
        REQUIRE(mbs == copy);
    }

    SECTION("as_molecular_basis_set") {
        auto copy = cv.as_molecular_basis_set();
        STATIC_REQUIRE(std::is_same_v<decltype(copy), MolecularBasisSet>);
        REQUIRE(copy == mbs);
        copy[0].set_name("STO-3G");
        copy.primitive(0).set_coefficient(42.0);
        REQUIRE(mbs[0].get_name() == "cc-pVDZ");
        REQUIRE(as_double(mbs.primitive(0).get_coefficient()) == 0.5);

        REQUIRE(v.as_molecular_basis_set() == mbs);
    }

    SECTION("swap rebinds") {
        molecular_basis_set_view ov(other);
        v.swap(ov);
        REQUIRE(v == other);
        REQUIRE(ov == mbs);
        v[0].set_name("changed");
        REQUIRE(other[0].get_name() == "changed");
        REQUIRE(mbs[0].get_name() == "cc-pVDZ");
    }
}

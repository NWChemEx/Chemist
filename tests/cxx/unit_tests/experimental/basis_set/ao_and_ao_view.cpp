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

// AO and AOView are abstract, so these tests reach them through CartesianAO
// and CartesianAOView --- the only concrete kinds which exist so far. What is
// being tested is specifically the polymorphic path: that asking a question
// through a base-class reference gets the same answer as asking the derived
// class directly, and that clone/as_ao/are_equal behave as those bases
// document. AOCommon, which both bases share, is what makes the first of those
// hold, and is tested here rather than in a file of its own for the same
// reason: it can only be reached through one of the two bases.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cartesian_ao_view.hpp>
#include <chemist/experimental/basis_set/cca_shell_class.hpp>
#include <chemist/experimental/basis_set/spherical_ao_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/experimental/point/point_set_class.hpp>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEST_CASE("experimental::AO") {
    Point origin(0.0, 0.0, 0.0);
    Point r(0.3, -0.4, 0.5);

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    CartesianAO dxy(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(1),
                    std::size_t(1), std::size_t(0), 1.0, 2.0, 3.0);

    AO& base             = dxy;
    const AO& const_base = dxy;

    SECTION("The shared accessors agree with the derived class") {
        REQUIRE(base.get_l() == dxy.get_l());
        REQUIRE(base.get_center() == dxy.get_center());
        REQUIRE(base.get_contracted_gaussian() ==
                dxy.get_contracted_gaussian());
        REQUIRE(as_double(base.normalization_constant()) ==
                as_double(dxy.normalization_constant()));
    }

    SECTION("The shared evaluation methods agree with the derived class") {
        REQUIRE(as_double(base.evaluate(r)) == as_double(dxy.evaluate(r)));
        REQUIRE(as_double(base.normalized_evaluate(r)) ==
                as_double(dxy.normalized_evaluate(r)));
    }

    SECTION("evaluate accepts any kind of point") {
        // The virtual hooks take one concrete point type, so AOCommon narrows
        // whatever it is given; this is what proves the narrowing works for a
        // view as well as for an owning Point.
        const_point_view rv(r);
        REQUIRE(as_double(base.evaluate(rv)) == as_double(base.evaluate(r)));
    }

    SECTION("evaluate over a set of points") {
        PointSet pts{origin, r};
        auto raw  = base.evaluate(pts);
        auto norm = base.normalized_evaluate(pts);
        REQUIRE(raw.size() == 2);
        REQUIRE(norm.size() == 2);
        REQUIRE(as_double(raw[1]) == as_double(base.evaluate(r)));
        REQUIRE(as_double(norm[1]) == as_double(base.normalized_evaluate(r)));
    }

    SECTION("clone is a deep copy") {
        auto cloned = const_base.clone();
        REQUIRE(cloned->are_equal(const_base));

        // The clone owns its state, so it does not observe later writes.
        dxy.get_contracted_gaussian().set_center(origin);
        REQUIRE(cloned->get_center() == Point(1.0, 2.0, 3.0));
        REQUIRE(cloned->are_different(const_base));
    }

    SECTION("are_equal/are_different") {
        CartesianAO same(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(1), std::size_t(1), std::size_t(0), 1.0,
                         2.0, 3.0);
        const AO& same_base = same;
        REQUIRE(const_base.are_equal(same_base));
        REQUIRE_FALSE(const_base.are_different(same_base));

        CartesianAO diff(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(2), std::size_t(0), std::size_t(0), 1.0,
                         2.0, 3.0);
        const AO& diff_base = diff;
        REQUIRE(const_base.are_different(diff_base));
        REQUIRE_FALSE(const_base.are_equal(diff_base));
    }

    SECTION("A Cartesian and a spherical AO are never equal") {
        // Even for s functions, which are the same function: they are
        // different kinds of AO, and are_equal compares kinds first.
        CartesianAO s_cart(cs.begin(), cs.end(), es.begin(), es.end(),
                           std::size_t(0), std::size_t(0), std::size_t(0), 1.0,
                           2.0, 3.0);
        SphericalAO s_sph(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(0), 0, 1.0, 2.0, 3.0);
        const AO& cart_base = s_cart;
        const AO& sph_base  = s_sph;
        REQUIRE(as_double(cart_base.normalized_evaluate(r)) ==
                Catch::Approx(as_double(sph_base.normalized_evaluate(r))));
        REQUIRE(cart_base.are_different(sph_base));
        REQUIRE(sph_base.are_different(cart_base));
    }
}

TEST_CASE("experimental::AO (spherical)") {
    Point r(0.3, -0.4, 0.5);

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    SphericalAO d1(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(2),
                   1, 1.0, 2.0, 3.0);
    const AO& base = d1;

    SECTION("The shared API agrees with the derived class") {
        REQUIRE(base.get_l() == d1.get_l());
        REQUIRE(base.get_center() == d1.get_center());
        REQUIRE(base.get_contracted_gaussian() == d1.get_contracted_gaussian());
        REQUIRE(as_double(base.normalization_constant()) ==
                as_double(d1.normalization_constant()));
        REQUIRE(as_double(base.evaluate(r)) == as_double(d1.evaluate(r)));
        REQUIRE(as_double(base.normalized_evaluate(r)) ==
                as_double(d1.normalized_evaluate(r)));
    }

    SECTION("clone is a deep copy of the same kind") {
        auto cloned = base.clone();
        REQUIRE(dynamic_cast<SphericalAO*>(cloned.get()) != nullptr);
        REQUIRE(cloned->are_equal(base));
    }
}

TEST_CASE("experimental::AOView (spherical)") {
    Point r(0.3, -0.4, 0.5);

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    CCAShell<CartesianAO> shell(cs.begin(), cs.end(), es.begin(), es.end(),
                                std::size_t(2), 1.0, 2.0, 3.0);
    SphericalAO d1(shell, 1);
    const_spherical_ao_view v(shell, 1);
    const AOView& base = v;

    SECTION("The shared API agrees with the AO it aliases") {
        REQUIRE(base.get_l() == d1.get_l());
        REQUIRE(as_double(base.normalized_evaluate(r)) ==
                as_double(d1.normalized_evaluate(r)));
    }

    SECTION("clone is a shallow copy") {
        auto cloned = base.clone();
        REQUIRE(cloned->are_equal(base));
        shell.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(cloned->get_center() == Point(0.0, 0.0, 0.0));
    }

    SECTION("as_ao materializes a SphericalAO") {
        auto materialized = base.as_ao();
        REQUIRE(dynamic_cast<SphericalAO*>(materialized.get()) != nullptr);
        REQUIRE(materialized->are_equal(d1));
    }

    SECTION("Views of different kinds of AO are never equal") {
        CartesianAO dxz(cs.begin(), cs.end(), es.begin(), es.end(),
                        std::size_t(1), std::size_t(0), std::size_t(1), 1.0,
                        2.0, 3.0);
        cartesian_ao_view cart_view(dxz);
        REQUIRE(base.are_different(cart_view));
    }
}

TEST_CASE("experimental::AOView") {
    Point r(0.3, -0.4, 0.5);

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    CartesianAO dxy(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(1),
                    std::size_t(1), std::size_t(0), 1.0, 2.0, 3.0);

    cartesian_ao_view v(dxy);
    AOView& base             = v;
    const AOView& const_base = v;

    SECTION("The shared accessors agree with the derived class") {
        REQUIRE(base.get_l() == v.get_l());
        REQUIRE(base.get_center() == v.get_center());
        REQUIRE(base.get_contracted_gaussian() == v.get_contracted_gaussian());
        REQUIRE(as_double(base.normalization_constant()) ==
                as_double(v.normalization_constant()));
    }

    SECTION("The shared accessors agree with the AO they alias") {
        const AO& owner = dxy;
        REQUIRE(base.get_l() == owner.get_l());
        REQUIRE(as_double(base.normalization_constant()) ==
                as_double(owner.normalization_constant()));
        REQUIRE(as_double(base.normalized_evaluate(r)) ==
                as_double(owner.normalized_evaluate(r)));
    }

    SECTION("clone is a shallow copy") {
        auto cloned = const_base.clone();
        REQUIRE(cloned->are_equal(const_base));

        // Unlike AO::clone, the result still aliases what *this aliases, so a
        // write made through the original AO is visible through the clone.
        dxy.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(cloned->get_center() == Point(0.0, 0.0, 0.0));
        REQUIRE(cloned->are_equal(const_base));
    }

    SECTION("as_ao materializes an owning AO") {
        auto materialized = const_base.as_ao();
        REQUIRE(materialized->are_equal(dxy));

        // The result owns its state, so it does not observe later writes.
        dxy.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(materialized->get_center() == Point(1.0, 2.0, 3.0));
        REQUIRE(materialized->are_different(dxy));
    }

    SECTION("as_ao returns the same kind of AO") {
        auto materialized = const_base.as_ao();
        REQUIRE(dynamic_cast<CartesianAO*>(materialized.get()) != nullptr);
    }

    SECTION("are_equal/are_different") {
        CartesianAO other(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(2), std::size_t(0), std::size_t(0), 1.0,
                          2.0, 3.0);
        cartesian_ao_view other_view(other);
        const AOView& other_base = other_view;

        REQUIRE(const_base.are_different(other_base));

        // Value equality does not care whether the two views alias the same
        // state, only whether the values agree.
        CartesianAO same(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(1), std::size_t(1), std::size_t(0), 1.0,
                         2.0, 3.0);
        cartesian_ao_view same_view(same);
        const AOView& same_base = same_view;
        REQUIRE(const_base.are_equal(same_base));
    }
}

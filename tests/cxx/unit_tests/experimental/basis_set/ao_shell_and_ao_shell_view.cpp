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

// AOShell<AOType> and AOShellView<AOType> are abstract, so these tests reach
// them through CCAShell and CCAShellView, of both purities, the same way
// ao_shell_base_and_ao_shell_base_view.cpp reaches the layer below. What is
// tested is what knowing the purity statically adds: the purity is a
// constant, the angular index of the purity (and only that one) is available
// and follows the shell's ordering, indexing hands out the concrete AO view,
// clone/as_view/as_shell keep the purity in their result's type, and
// as_cartesian_shell/as_spherical_shell recover the purity from the layer
// below. AOShellCommon is tested here too, since it can only be reached
// through one of the two bases.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cca_shell_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;

namespace {

/// Detects whether a shell type has cartesian_powers / magnetic_index
///@{
template<typename T, typename = void>
struct has_cartesian_powers : std::false_type {};

template<typename T>
struct has_cartesian_powers<
  T, std::void_t<decltype(std::declval<const T&>().cartesian_powers(0))>>
  : std::true_type {};

template<typename T, typename = void>
struct has_magnetic_index : std::false_type {};

template<typename T>
struct has_magnetic_index<
  T, std::void_t<decltype(std::declval<const T&>().magnetic_index(0))>>
  : std::true_type {};
///@}

} // namespace

TEST_CASE("experimental::AOShell") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    cartesian_cca_shell d(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(2), 1.0, 2.0, 3.0);
    spherical_cca_shell pd(cs.begin(), cs.end(), es.begin(), es.end(),
                           std::size_t(2), 1.0, 2.0, 3.0);
    const AOShell<CartesianAO>& cart = d;
    const AOShell<SphericalAO>& pure = pd;

    SECTION("is_pure/is_cartesian are known statically") {
        // CCAShell does not redeclare these, so this is AOShell's constexpr
        STATIC_REQUIRE_FALSE(d.is_pure());
        STATIC_REQUIRE(d.is_cartesian());
        STATIC_REQUIRE(pd.is_pure());
        REQUIRE_FALSE(cart.is_pure());
        REQUIRE(cart.is_cartesian());
        REQUIRE(pure.is_pure());
        REQUIRE_FALSE(pure.is_cartesian());

        // The base, which dispatches virtually, agrees
        REQUIRE_FALSE(static_cast<const AOShellBase&>(cart).is_pure());
        REQUIRE(static_cast<const AOShellBase&>(pure).is_pure());
    }

    SECTION("Only the angular index of the purity exists") {
        STATIC_REQUIRE(has_cartesian_powers<cartesian_ao_shell>::value);
        STATIC_REQUIRE_FALSE(has_magnetic_index<cartesian_ao_shell>::value);
        STATIC_REQUIRE(has_magnetic_index<spherical_ao_shell>::value);
        STATIC_REQUIRE_FALSE(has_cartesian_powers<spherical_ao_shell>::value);
    }

    SECTION("The angular index follows the shell's ordering") {
        for(std::size_t a = 0; a < d.size(); ++a)
            REQUIRE(cart.cartesian_powers(a) == d.cartesian_powers(a));
        for(std::size_t a = 0; a < pd.size(); ++a)
            REQUIRE(pure.magnetic_index(a) == pd.magnetic_index(a));
        REQUIRE_THROWS_AS(cart.cartesian_powers(6), std::out_of_range);
        REQUIRE_THROWS_AS(pure.magnetic_index(5), std::out_of_range);
    }

    SECTION("at hands out the concrete AO view") {
        using cart_ptr = decltype(cart.at(0));
        using pure_ptr = decltype(pure.at(0));
        STATIC_REQUIRE(
          std::is_same_v<cart_ptr, std::unique_ptr<const_cartesian_ao_view>>);
        STATIC_REQUIRE(
          std::is_same_v<pure_ptr, std::unique_ptr<const_spherical_ao_view>>);

        for(std::size_t a = 0; a < d.size(); ++a) {
            auto pao = cart.at(a);
            REQUIRE(*pao == *d.at(a));
            REQUIRE(*cart[a] == *pao);
        }
        for(std::size_t a = 0; a < pd.size(); ++a) {
            auto pao = pure.at(a);
            REQUIRE(*pao == *pd.at(a));
            REQUIRE(*pure[a] == *pao);
        }
        REQUIRE_THROWS_AS(cart.at(6), std::out_of_range);
        REQUIRE_THROWS_AS(pure.at(5), std::out_of_range);
    }

    SECTION("at through the base agrees with the typed at") {
        const AOShellBase& base = pure;
        for(std::size_t a = 0; a < pd.size(); ++a)
            REQUIRE(base.at(a)->are_equal(*pure.at(a)));
    }

    SECTION("The AOs from at alias the shell") {
        auto ao      = cart.at(1);
        auto pure_ao = pure.at(1);
        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        pd.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(ao->get_center() == Point(0.0, 0.0, 0.0));
        REQUIRE(pure_ao->get_center() == Point(0.0, 0.0, 0.0));
    }

    SECTION("get_cartesian_shell") {
        for(const AOShellBase* s : {static_cast<const AOShellBase*>(&cart),
                                    static_cast<const AOShellBase*>(&pure)}) {
            std::unique_ptr<cartesian_ao_shell_view> pc;
            if(s->is_pure())
                pc = as_spherical_shell(*s).get_cartesian_shell();
            else
                pc = as_cartesian_shell(*s).get_cartesian_shell();
            REQUIRE(pc->is_cartesian());
            REQUIRE(pc->get_contracted_gaussian() ==
                    d.get_contracted_gaussian());
            // It keeps the ordering of the shell it came from
            REQUIRE(dynamic_cast<const_cartesian_cca_shell_view*>(pc.get()) !=
                    nullptr);
        }

        // It aliases the shell
        auto pc = pure.get_cartesian_shell();
        pd.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(pc->get_center() == Point(0.0, 0.0, 0.0));
    }

    SECTION("clone keeps the purity in the type") {
        STATIC_REQUIRE(std::is_same_v<decltype(cart.clone()),
                                      std::unique_ptr<cartesian_ao_shell>>);
        auto cloned = cart.clone();
        REQUIRE(cloned->are_equal(cart));
        REQUIRE(dynamic_cast<cartesian_cca_shell*>(cloned.get()) != nullptr);

        auto pure_cloned = pure.clone();
        REQUIRE(pure_cloned->are_equal(pure));
        REQUIRE(dynamic_cast<spherical_cca_shell*>(pure_cloned.get()) !=
                nullptr);

        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(cloned->get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("as_view keeps the purity in the type") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(pure.as_view()),
                         std::unique_ptr<spherical_ao_shell_view>>);
        auto v = cart.as_view();
        REQUIRE(dynamic_cast<const_cartesian_cca_shell_view*>(v.get()) !=
                nullptr);
        auto pv = pure.as_view();
        REQUIRE(dynamic_cast<const_spherical_cca_shell_view*>(pv.get()) !=
                nullptr);

        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(v->get_center() == Point(0.0, 0.0, 0.0));
    }
}

TEST_CASE("experimental::AOShellView") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    cartesian_cca_shell d(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(2), 1.0, 2.0, 3.0);
    spherical_cca_shell pd(cs.begin(), cs.end(), es.begin(), es.end(),
                           std::size_t(2), 1.0, 2.0, 3.0);
    const_cartesian_cca_shell_view v(d);
    const_spherical_cca_shell_view pv(pd);
    const AOShellView<CartesianAO>& cart = v;
    const AOShellView<SphericalAO>& pure = pv;

    SECTION("is_pure/is_cartesian are known statically") {
        STATIC_REQUIRE(has_cartesian_powers<cartesian_ao_shell_view>::value);
        STATIC_REQUIRE_FALSE(
          has_magnetic_index<cartesian_ao_shell_view>::value);
        STATIC_REQUIRE(has_magnetic_index<spherical_ao_shell_view>::value);
        STATIC_REQUIRE_FALSE(
          has_cartesian_powers<spherical_ao_shell_view>::value);
        REQUIRE(cart.is_cartesian());
        REQUIRE(pure.is_pure());
    }

    SECTION("The angular index and at agree with the derived class") {
        for(std::size_t a = 0; a < v.size(); ++a) {
            REQUIRE(cart.cartesian_powers(a) == v.cartesian_powers(a));
            REQUIRE(*cart.at(a) == *v.at(a));
        }
        for(std::size_t a = 0; a < pv.size(); ++a) {
            REQUIRE(pure.magnetic_index(a) == pv.magnetic_index(a));
            REQUIRE(*pure.at(a) == *pv.at(a));
        }
    }

    SECTION("clone is a shallow copy of the same purity") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(cart.clone()),
                         std::unique_ptr<cartesian_ao_shell_view>>);
        auto cloned = cart.clone();
        REQUIRE(cloned->are_equal(cart));
        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(cloned->get_center() == Point(0.0, 0.0, 0.0));
    }

    SECTION("as_shell keeps the purity in the type") {
        STATIC_REQUIRE(std::is_same_v<decltype(pure.as_shell()),
                                      std::unique_ptr<spherical_ao_shell>>);
        auto materialized = pure.as_shell();
        REQUIRE(dynamic_cast<spherical_cca_shell*>(materialized.get()) !=
                nullptr);
        REQUIRE(materialized->are_equal(pd));
    }
}

TEST_CASE("experimental::as_cartesian_shell/as_spherical_shell") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    cartesian_cca_shell d(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(2), 1.0, 2.0, 3.0);
    spherical_cca_shell pd(cs.begin(), cs.end(), es.begin(), es.end(),
                           std::size_t(2), 1.0, 2.0, 3.0);
    const AOShellBase& base      = d;
    const AOShellBase& pure_base = pd;
    const_cartesian_cca_shell_view v(d);
    const_spherical_cca_shell_view pv(pd);
    const AOShellBaseView& view_base      = v;
    const AOShellBaseView& pure_view_base = pv;

    SECTION("A match is the same object, typed") {
        const AOShell<CartesianAO>& c = as_cartesian_shell(base);
        REQUIRE(&c == &d);
        const AOShell<SphericalAO>& p = as_spherical_shell(pure_base);
        REQUIRE(&p == &pd);

        const AOShellView<CartesianAO>& cv = as_cartesian_shell(view_base);
        REQUIRE(&cv == &v);
        const AOShellView<SphericalAO>& pvv =
          as_spherical_shell(pure_view_base);
        REQUIRE(&pvv == &pv);
    }

    SECTION("A mismatch throws") {
        REQUIRE_THROWS_AS(as_cartesian_shell(pure_base), std::invalid_argument);
        REQUIRE_THROWS_AS(as_spherical_shell(base), std::invalid_argument);
        REQUIRE_THROWS_AS(as_cartesian_shell(pure_view_base),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(as_spherical_shell(view_base), std::invalid_argument);
    }
}

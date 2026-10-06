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

// CCAShellCommon implements the API shared by CCAShell and CCAShellView, for
// both purities. It has no state of its own to construct, so these tests
// drive it through CCAShell<CartesianAO> and CCAShell<SphericalAO>, pulling in
// the views only where a property spans two derived types. CCAShell's ctors,
// swap, and serialization are tested in cca_shell_class.cpp; CCAShellView's
// own members in cca_shell_view.cpp. is_pure, size, and at are implemented by
// AOShellCommon rather than CCAShellCommon, but are still checked here, since
// what at hands out depends on the CCA ordering.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <array>
#include <chemist/experimental/basis_set/cca_shell_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// Shorthand for the powers of one Cartesian AO
using powers = std::array<std::size_t, 3>;

/// Detects whether a method can be called on a T. The setters and the
/// purity-specific accessors are removed by SFINAE rather than throwing, and
/// "does not compile" can not be a runtime assertion.
///@{
template<typename T, typename = void>
struct has_l_setter : std::false_type {};

template<typename T>
struct has_l_setter<T, std::void_t<decltype(std::declval<T&>().set_l(0))>>
  : std::true_type {};

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

TEST_CASE("experimental::CCAShellCommon") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    auto make_cart = [&](std::size_t l) {
        return cartesian_cca_shell(cs.begin(), cs.end(), es.begin(), es.end(),
                                   l, 1.0, 2.0, 3.0);
    };
    auto make_pure = [&](std::size_t l) {
        return spherical_cca_shell(cs.begin(), cs.end(), es.begin(), es.end(),
                                   l, 1.0, 2.0, 3.0);
    };

    auto d  = make_cart(2);
    auto pd = make_pure(2);

    SECTION("get_l") {
        REQUIRE(d.get_l() == 2);
        REQUIRE(pd.get_l() == 2);
        REQUIRE(pd.get_contracted_gaussian().get_l() == 2);
    }

    SECTION("is_pure/is_cartesian") {
        REQUIRE_FALSE(d.is_pure());
        REQUIRE(d.is_cartesian());
        REQUIRE(pd.is_pure());
        REQUIRE_FALSE(pd.is_cartesian());
    }

    SECTION("size of a Cartesian shell is (l+1)(l+2)/2") {
        REQUIRE(make_cart(0).size() == 1);
        REQUIRE(make_cart(1).size() == 3);
        REQUIRE(make_cart(2).size() == 6);
        REQUIRE(make_cart(3).size() == 10);
    }

    SECTION("size of a pure shell is 2l+1") {
        REQUIRE(make_pure(0).size() == 1);
        REQUIRE(make_pure(1).size() == 3);
        REQUIRE(make_pure(2).size() == 5);
        REQUIRE(make_pure(3).size() == 7);
    }

    SECTION("get_center") {
        REQUIRE(d.get_center() == Point(1.0, 2.0, 3.0));
        REQUIRE(pd.get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("cartesian_powers follows the CCA order") {
        // These are the tables in ao_hierarchy.rst.
        auto order_of = [&](std::size_t l) {
            auto shell = make_cart(l);
            std::vector<powers> rv;
            for(std::size_t a = 0; a < shell.size(); ++a)
                rv.push_back(shell.cartesian_powers(a));
            return rv;
        };

        REQUIRE(order_of(0) == std::vector<powers>{{0, 0, 0}});
        REQUIRE(order_of(1) ==
                std::vector<powers>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
        REQUIRE(
          order_of(2) ==
          std::vector<powers>{
            {2, 0, 0}, {1, 1, 0}, {1, 0, 1}, {0, 2, 0}, {0, 1, 1}, {0, 0, 2}});
        REQUIRE(order_of(3) == std::vector<powers>{{3, 0, 0},
                                                   {2, 1, 0},
                                                   {2, 0, 1},
                                                   {1, 2, 0},
                                                   {1, 1, 1},
                                                   {1, 0, 2},
                                                   {0, 3, 0},
                                                   {0, 2, 1},
                                                   {0, 1, 2},
                                                   {0, 0, 3}});
    }

    SECTION("magnetic_index runs from -l to +l") {
        auto f = make_pure(3);
        for(std::size_t a = 0; a < f.size(); ++a)
            REQUIRE(f.magnetic_index(a) == static_cast<long>(a) - 3);
    }

    SECTION("The angular indices throw for an out-of-range offset") {
        REQUIRE_THROWS_AS(d.cartesian_powers(6), std::out_of_range);
        REQUIRE_THROWS_AS(pd.magnetic_index(5), std::out_of_range);
    }

    SECTION("Each purity only has its own angular index") {
        STATIC_REQUIRE(has_cartesian_powers<cartesian_cca_shell>::value);
        STATIC_REQUIRE_FALSE(has_magnetic_index<cartesian_cca_shell>::value);
        STATIC_REQUIRE(has_magnetic_index<spherical_cca_shell>::value);
        STATIC_REQUIRE_FALSE(has_cartesian_powers<spherical_cca_shell>::value);
    }

    SECTION("at/operator[] of a Cartesian shell build the Cartesian AO") {
        CartesianAO dxz(cs.begin(), cs.end(), es.begin(), es.end(),
                        std::size_t(1), std::size_t(0), std::size_t(1), 1.0,
                        2.0, 3.0);
        STATIC_REQUIRE(
          std::is_same_v<decltype(d.at(2)),
                         std::unique_ptr<const_cartesian_ao_view>>);
        REQUIRE(*d.at(2) == dxz);
        REQUIRE(*d[2] == dxz);
        REQUIRE_THROWS_AS(d.at(6), std::out_of_range);
    }

    SECTION("at/operator[] of a pure shell build the spherical AO") {
        SphericalAO d_m1(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(2), -1, 1.0, 2.0, 3.0);
        STATIC_REQUIRE(
          std::is_same_v<decltype(pd.at(1)),
                         std::unique_ptr<const_spherical_ao_view>>);
        REQUIRE(*pd.at(1) == d_m1);
        REQUIRE(*pd[1] == d_m1);
        REQUIRE(pd.at(1)->get_m() == -1);
        REQUIRE_THROWS_AS(pd.at(5), std::out_of_range);
    }

    SECTION("The AOs alias the shell's contracted Gaussian") {
        auto cart_ao = d.at(1);
        auto pure_ao = pd.at(1);
        d.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0));
        pd.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0));
        REQUIRE(cart_ao->get_center() == Point(4.0, 5.0, 6.0));
        REQUIRE(pure_ao->get_center() == Point(4.0, 5.0, 6.0));
    }

    SECTION("Each call builds a new view") {
        // Indexing twice gives two different, but equal, objects, for either
        // purity ...
        auto a0 = d.at(1);
        auto a1 = d.at(1);
        REQUIRE(a0.get() != a1.get());
        REQUIRE(*a0 == *a1);
        auto p0 = pd.at(1);
        auto p1 = pd.at(1);
        REQUIRE(p0.get() != p1.get());
        REQUIRE(*p0 == *p1);

        // ... and indexing through a read-only view of the shell gives an
        // equal view, aliasing the same state.
        const_cartesian_cca_shell_view v(d);
        REQUIRE(*v.at(1) == *d.at(1));
    }

    SECTION("Indexing follows set_l") {
        d.set_l(3);
        REQUIRE(*d.at(9) == CartesianAO(cs.begin(), cs.end(), es.begin(),
                                        es.end(), std::size_t(0),
                                        std::size_t(0), std::size_t(3), 1.0,
                                        2.0, 3.0));

        pd.set_l(3);
        REQUIRE(pd.at(6)->get_m() == 3);
        REQUIRE(pd.at(6)->get_l() == 3);
    }

    SECTION("Indexing follows l changed through the contracted Gaussian") {
        d.get_contracted_gaussian().set_l(1);
        REQUIRE(d.size() == 3);
        REQUIRE(d.at(2)->get_k() == 1);
        REQUIRE(d.at(2)->get_l() == 1);
    }

    SECTION("An AO obtained before set_l still aliases the shell") {
        auto ao = d.at(2);
        d.set_l(3);
        REQUIRE(ao->get_contracted_gaussian().get_l() == 3);
    }

    SECTION("A copy's AOs alias the copy, not the original") {
        auto copy = d;
        copy.get_contracted_gaussian().set_center(Point(9.0, 9.0, 9.0));
        REQUIRE(copy.at(0)->get_center() == Point(9.0, 9.0, 9.0));
        REQUIRE(d.at(0)->get_center() == Point(1.0, 2.0, 3.0));

        auto pure_copy = pd;
        pure_copy.get_contracted_gaussian().set_center(Point(9.0, 9.0, 9.0));
        REQUIRE(pure_copy.at(0)->get_center() == Point(9.0, 9.0, 9.0));
        REQUIRE(pd.at(0)->get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("A moved-to shell's AOs alias the moved-to shell") {
        auto moved = std::move(d);
        moved.get_contracted_gaussian().set_center(Point(9.0, 9.0, 9.0));
        REQUIRE(moved.at(0)->get_center() == Point(9.0, 9.0, 9.0));
    }

    SECTION("After assignment and swap the AOs alias the right shell") {
        auto p = make_cart(1);

        d.swap(p);
        REQUIRE(d.size() == 3);
        d.get_contracted_gaussian().set_center(Point(9.0, 9.0, 9.0));
        REQUIRE(d.at(2)->get_center() == Point(9.0, 9.0, 9.0));
        REQUIRE(p.at(5)->get_center() == Point(1.0, 2.0, 3.0));

        auto q = make_cart(1);
        q      = p;
        q.get_contracted_gaussian().set_center(Point(8.0, 8.0, 8.0));
        REQUIRE(q.at(5)->get_center() == Point(8.0, 8.0, 8.0));
        REQUIRE(p.at(5)->get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("get_cartesian_shell") {
        // For a pure shell it is the Cartesian shell underneath ...
        auto cart = pd.get_cartesian_shell();
        REQUIRE(cart.is_cartesian());
        REQUIRE(cart.size() == 6);
        REQUIRE(cart == d);

        // ... which aliases the pure shell's contracted Gaussian.
        pd.get_contracted_gaussian().set_center(Point(7.0, 7.0, 7.0));
        REQUIRE(cart.get_center() == Point(7.0, 7.0, 7.0));

        // For a Cartesian shell it is the shell itself.
        REQUIRE(d.get_cartesian_shell() == d);
    }

    SECTION("normalization_constant is N^G") {
        REQUIRE(
          as_double(d.normalization_constant()) ==
          as_double(d.get_contracted_gaussian().normalization_constant()));
        REQUIRE(as_double(pd.normalization_constant()) ==
                as_double(d.normalization_constant()));
    }

    SECTION("set_l") {
        d.set_l(3);
        REQUIRE(d.get_l() == 3);
        REQUIRE(d.size() == 10);

        pd.set_l(3);
        REQUIRE(pd.get_l() == 3);
        REQUIRE(pd.size() == 7);
        REQUIRE(pd.get_contracted_gaussian().get_l() == 3);
    }

    SECTION("set_l exists only when the shell is mutable") {
        STATIC_REQUIRE(has_l_setter<cartesian_cca_shell>::value);
        STATIC_REQUIRE(has_l_setter<cartesian_cca_shell_view>::value);
        STATIC_REQUIRE_FALSE(
          has_l_setter<const_cartesian_cca_shell_view>::value);
        STATIC_REQUIRE(has_l_setter<spherical_cca_shell>::value);
        STATIC_REQUIRE_FALSE(
          has_l_setter<const_spherical_cca_shell_view>::value);
    }

    SECTION("Comparisons") {
        REQUIRE(d == make_cart(2));
        REQUIRE_FALSE(d != make_cart(2));
        REQUIRE(d != make_cart(1));
        REQUIRE(pd == make_pure(2));
        REQUIRE(pd != make_pure(1));

        // Same contracted Gaussian, different purity: not the same shell
        REQUIRE(d != pd);
        REQUIRE(pd != d);

        // Owning versus aliasing is not considered
        auto other = make_cart(2);
        const_cartesian_cca_shell_view v(other);
        REQUIRE(d == v);
        REQUIRE(v == d);
    }
}

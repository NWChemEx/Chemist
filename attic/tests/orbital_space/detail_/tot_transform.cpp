#include "../test_orbital_space.hpp"
#include "../transform_data.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include "chemist/orbital_space/derived_space.hpp"
#include "chemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

using namespace chemist::orbital_space;

namespace {
// Notation is outer_rank "underscore" inner_rank

template<typename ScalarType>
auto c_1_1(TA::World& world) {
    TA::Range r({7});
    TA::Tensor<ScalarType> t0(r, {-9.94435e-01, -2.40970e-02, -3.16155e-03,
                                  7.88705e-20, -2.17229e-18, 4.59374e-03,
                                  4.59374e-03});
    TA::Tensor<ScalarType> t1(r, {-2.39159e-01, 8.85736e-01, 8.58964e-02,
                                  -1.70837e-17, 2.25883e-16, 1.44039e-01,
                                  1.44039e-01});
    return type::tensor_of_tensors<ScalarType>(world, {t0, t1});
}

template<typename ScalarType>
auto c_1_2(TA::World& world) {
    TA::Range r({7, 2});
    TA::Tensor<ScalarType> t0(
      r, {-9.94435e-01, -2.39159e-01, -2.40970e-02, 8.85736e-01, -3.16155e-03,
          8.58964e-02, 7.88705e-20, -1.70837e-17, -2.17229e-18, 2.25883e-16,
          4.59374e-03, 1.44039e-01, 4.59374e-03, 1.44039e-01});
    return type::tensor_of_tensors<ScalarType>(world, {t0, t0});
}

template<typename ScalarType>
auto c_2_2(TA::World& world) {
    TA::Range r({7, 2});
    TA::Tensor<ScalarType> t0(
      r, {-9.94435e-01, -2.39159e-01, -2.40970e-02, 8.85736e-01, -3.16155e-03,
          8.58964e-02, 7.88705e-20, -1.70837e-17, -2.17229e-18, 2.25883e-16,
          4.59374e-03, 1.44039e-01, 4.59374e-03, 1.44039e-01});
    return type::tensor_of_tensors<ScalarType>(world, {{t0, t0}, {t0, t0}});
}

template<typename ScalarType>
auto t_1_2(TA::World& world) {
    TA::Range r({7, 7});
    TA::Tensor<ScalarType> t0(
      r, {-20.2546,     -5.16378,     -0.0254795,   -1.71349e-18, -1.16579e-17,
          -0.853342,    -0.853342,    -5.16378,     -2.41068,     -0.0743883,
          -7.0713e-19,  -3.67949e-16, -0.801537,    -0.801537,    -0.0254795,
          -0.0743883,   -0.275535,    -3.21467e-17, -7.76323e-16, -0.324682,
          -0.324682,    -1.71349e-18, -7.0713e-19,  -3.21467e-17, -0.387587,
          -4.16098e-17, -6.04149e-17, 3.06325e-17,  -1.28123e-17, -3.81826e-16,
          -7.86731e-16, -4.16098e-17, -0.23364,     -0.357785,    0.357785,
          -0.853342,    -0.801537,    -0.324682,    -6.04149e-17, -0.357785,
          -0.418208,    -0.252292,    -0.853342,    -0.801537,    -0.324682,
          3.06325e-17,  0.357785,     -0.252292,    -0.418208});
    return type::tensor_of_tensors<ScalarType>(world, {t0, t0});
}

template<typename ScalarType>
auto t_2_2(TA::World& world) {
    TA::Range r({7, 7});
    TA::Tensor<ScalarType> t0(
      r, {-20.2546,     -5.16378,     -0.0254795,   -1.71349e-18, -1.16579e-17,
          -0.853342,    -0.853342,    -5.16378,     -2.41068,     -0.0743883,
          -7.0713e-19,  -3.67949e-16, -0.801537,    -0.801537,    -0.0254795,
          -0.0743883,   -0.275535,    -3.21467e-17, -7.76323e-16, -0.324682,
          -0.324682,    -1.71349e-18, -7.0713e-19,  -3.21467e-17, -0.387587,
          -4.16098e-17, -6.04149e-17, 3.06325e-17,  -1.28123e-17, -3.81826e-16,
          -7.86731e-16, -4.16098e-17, -0.23364,     -0.357785,    0.357785,
          -0.853342,    -0.801537,    -0.324682,    -6.04149e-17, -0.357785,
          -0.418208,    -0.252292,    -0.853342,    -0.801537,    -0.324682,
          3.06325e-17,  0.357785,     -0.252292,    -0.418208});
    return type::tensor_of_tensors<ScalarType>(world, {{t0, t0}, {t0, t0}});
}

template<typename ScalarType>
auto ct_1_2(TA::World& world) {
    TA::Range r({2, 7});
    TA::Tensor<ScalarType> t0(
      r, {2.02585552e+01, 5.18600480e+00, 2.50183497e-02, 1.65524576e-18,
          2.34187669e-17, 8.65854185e-01, 8.65854185e-01, 2.23063853e-02,
          -1.13755646e+00, -1.76995949e-01, -6.46226826e-19, -4.44089210e-16,
          -6.30332921e-01, -6.30332921e-01});
    return type::tensor_of_tensors<ScalarType>(world, {t0, t0});
}

template<typename ScalarType>
auto ct_1_1(TA::World& world) {
    TA::Range r({7});
    TA::Tensor<ScalarType> t0(
      r, {2.02585552e+01, 5.18600480e+00, 2.50183497e-02, 1.65524576e-18,
          2.34187669e-17, 8.65854185e-01, 8.65854185e-01});
    TA::Tensor<ScalarType> t1(
      r, {2.23063853e-02, -1.13755646e+00, -1.76995949e-01, -6.46226826e-19,
          -4.44089210e-16, -6.30332921e-01, -6.30332921e-01});
    return type::tensor_of_tensors<ScalarType>(world, {t0, t1});
}

template<typename ScalarType>
auto ct_2_2(TA::World& world) {
    TA::Range r({2, 7});
    TA::Tensor<ScalarType> t0(
      r, {2.02585552e+01, 5.18600480e+00, 2.50183497e-02, 1.65524576e-18,
          2.34187669e-17, 8.65854185e-01, 8.65854185e-01, 2.23063853e-02,
          -1.13755646e+00, -1.76995949e-01, -6.46226826e-19, -4.44089210e-16,
          -6.30332921e-01, -6.30332921e-01});
    return type::tensor_of_tensors<ScalarType>(world, {{t0, t0}, {t0, t0}});
}

template<typename ScalarType>
auto tc_1_2(TA::World& world) {
    TA::Range r({7, 2});
    TA::Tensor<ScalarType> t0(
      r, {2.02585552e+01, 2.23063853e-02, 5.18600480e+00, -1.13755646e+00,
          2.50183497e-02, -1.76995949e-01, 1.65524576e-18, -6.46226826e-19,
          2.49366500e-17, -4.57966998e-16, 8.65854185e-01, -6.30332921e-01,
          8.65854185e-01, -6.30332921e-01});
    return type::tensor_of_tensors<ScalarType>(world, {t0, t0});
}

template<typename ScalarType>
auto tc_2_2(TA::World& world) {
    TA::Range r({7, 2});
    TA::Tensor<ScalarType> t0(
      r, {2.02585552e+01, 2.23063853e-02, 5.18600480e+00, -1.13755646e+00,
          2.50183497e-02, -1.76995949e-01, 1.65524576e-18, -6.46226826e-19,
          2.49366500e-17, -4.57966998e-16, 8.65854185e-01, -6.30332921e-01,
          8.65854185e-01, -6.30332921e-01});
    return type::tensor_of_tensors<ScalarType>(world, {{t0, t0}, {t0, t0}});
}

template<typename ScalarType>
auto ctc_1_2(TA::World& world) {
    TA::Range r({2, 2});
    TA::Tensor<ScalarType> t0(
      r, {-2.02629076e+01, -2.14177280e-06, -2.14177280e-06, -1.20969785e+00});
    return type::tensor_of_tensors<ScalarType>(world, {t0, t0});
}

template<typename ScalarType>
auto ctc_2_2(TA::World& world) {
    TA::Range r({2, 2});
    TA::Tensor<ScalarType> t0(
      r, {-2.02629076e+01, -2.14177280e-06, -2.14177280e-06, -1.20969785e+00});
    return type::tensor_of_tensors<ScalarType>(world, {{t0, t0}, {t0, t0}});
}

} // namespace

using namespace chemist::orbital_space::detail_;
using namespace chemist::ta_helpers;

TEMPLATE_TEST_CASE("tot_transform", "", double, float) {
    using tensor_type = type::tensor_of_tensors<TestType>;
    using from_space  = BaseSpace_<tensor_type>;
    using base_space  = BaseSpace_<tensor_type>;
    using space_type  = DerivedSpace_<tensor_type, from_space, base_space>;
    using mode_vec    = std::vector<std::size_t>;

    auto& world = TA::get_default_world();

    SECTION("Raises error if C's ind rank != T's ind rank") {
        space_type st(c_1_2<TestType>(world), base_space{});
        auto t = t_2_2<TestType>(world);
        mode_vec modes{1};
        REQUIRE_THROWS_AS(tot_transform(st, t, modes), std::runtime_error);
    }

    SECTION("Raises error if mode to transform is > dep_rank") {
        space_type st(c_1_2<TestType>(world), base_space{});
        auto t = t_1_2<TestType>(world);
        mode_vec modes{5};
        REQUIRE_THROWS_AS(tot_transform(st, t, modes), std::runtime_error);
    }

    SECTION("One independent, two dependent") {
        space_type st(c_1_2<TestType>(world), base_space{});
        auto t = t_1_2<TestType>(world);

        SECTION("No transform") {
            auto x = tot_transform(st, t, mode_vec{});
            REQUIRE(allclose_tot(x, t, 2));
        }

        SECTION("Transform mode 0") {
            auto x    = tot_transform(st, t, mode_vec{0});
            auto corr = ct_1_2<TestType>(world);
            REQUIRE(allclose_tot(x, corr, 2));
        }

        SECTION("Transform mode 1") {
            auto x    = tot_transform(st, t, mode_vec{1});
            auto corr = tc_1_2<TestType>(world);
            REQUIRE(allclose_tot(x, corr, 2));
        }

        SECTION("Transform modes 0 and 1") {
            auto x    = tot_transform(st, t, mode_vec{0, 1});
            auto corr = ctc_1_2<TestType>(world);
            REQUIRE(allclose_tot(x, corr, 2));
        }
    }

    SECTION("Two independent, two dependent") {
        space_type st(c_2_2<TestType>(world), base_space{});
        auto t = t_2_2<TestType>(world);

        SECTION("No transform") {
            auto x = tot_transform(st, t, mode_vec{});
            REQUIRE(allclose_tot(x, t, 2));
        }

        SECTION("Transform mode 0") {
            auto x    = tot_transform(st, t, mode_vec{0});
            auto corr = ct_2_2<TestType>(world);
            REQUIRE(allclose_tot(x, corr, 2));
        }

        SECTION("Transform mode 1") {
            auto x    = tot_transform(st, t, mode_vec{1});
            auto corr = tc_2_2<TestType>(world);
            REQUIRE(allclose_tot(x, corr, 2));
        }

        SECTION("Transform modes 0 and 1") {
            auto x    = tot_transform(st, t, mode_vec{0, 1});
            auto corr = ctc_2_2<TestType>(world);
            REQUIRE(allclose_tot(x, corr, 2));
        }
    }

    SECTION("Transform to independent basis") {
        SECTION("One independent, two dependent") {
            space_type st(c_1_1<TestType>(world), base_space{});
            auto t = t_1_2<TestType>(world);

            SECTION("No transform") {
                auto x = tot_transform(st, t, mode_vec{});
                REQUIRE(allclose_tot(x, t, 2));
            }

            SECTION("Transform mode 0") {
                auto x    = tot_transform(st, t, mode_vec{0});
                auto corr = ct_1_1<TestType>(world);
                REQUIRE(allclose_tot(x, corr, 1));
            }

            SECTION("Transform mode 1") {
                auto x    = tot_transform(st, t, mode_vec{1});
                auto corr = ct_1_1<TestType>(world);
                REQUIRE(allclose_tot(x, corr, 1));
            }

            SECTION("Transform modes 0 and 1") {
                mode_vec ms{0, 1};
                REQUIRE_THROWS_AS(tot_transform(st, t, ms), std::runtime_error);
            }
        }
    }
}
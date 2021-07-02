#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/type_traits/tensor_traits.hpp"
#include "libchemist/tensor/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

/* Testing strategy:
 *
 * This series of unit tests test the entire expression layer from input to
 * result. The pieces of the individual classes are tested
 */

TEMPLATE_LIST_TEST_CASE("TensorWrapper DSL", "", tensor_variant_t) {
    auto& world    = TA::get_default_world();
    using TWrapper = TensorWrapper<tensor_variant_t>;
    using t_type   = TestType;

    // Declare the TA tensors
    t_type ta_vec(world, {1.0, 2.0, 3.0});
    t_type ta_matrix(world, {{1.0, 2.0}, {3.0, 4.0}});
    t_type ta_tensor(world,
                     {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}});

    // Declare non-const wrappers
    TWrapper vec(ta_vec);
    TWrapper mat(ta_matrix);
    TWrapper t3(ta_tensor);

    // Declare const wrappers
    const TWrapper const_vec(ta_vec);
    const TWrapper const_mat(ta_matrix);
    const TWrapper const_t3(ta_tensor);

    // Get LabeledTensorWrappers
    auto lvec       = vec("i");
    auto const_lvec = const_vec("i");
    auto lmat       = mat("i,j");
    auto const_lmat = const_mat("i,j");
    auto lt3        = t3("i,j,k");
    auto const_lt3  = const_t3("i,j,k");

    TWrapper result(t_type{});
    SECTION("operator+") {
        SECTION("operator+(other labeled tensor)") {
            SECTION("vector") {
                result("i") = lvec + lvec;
                auto& rv    = result.get<t_type>();
                t_type corr(world, {2.0, 4.0, 6.0});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("matrix") {
                result("i,j") = lmat + lmat;
                auto& rv      = result.get<t_type>();
                t_type corr(world, {{2.0, 4.0}, {6.0, 8.0}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("rank-3 tensor") {
                result("i,j,k") = lt3 + lt3;
                auto& rv        = result.get<t_type>();
                t_type corr(world, {{{2.0, 4.0}, {6.0, 8.0}},
                                    {{10.0, 12.0}, {14.0, 16.0}}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
        }
        SECTION("operator+(const other labeled tensor)const") {
            SECTION("vector") {
                result("i") = lvec + const_lvec;
                auto& rv    = result.get<t_type>();
                t_type corr(world, {2.0, 4.0, 6.0});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("matrix") {
                result("i,j") = lmat + const_lmat;
                auto& rv      = result.get<t_type>();
                t_type corr(world, {{2.0, 4.0}, {6.0, 8.0}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("rank-3 tensor") {
                result("i,j,k") = lt3 + const_lt3;
                auto& rv        = result.get<t_type>();
                t_type corr(world, {{{2.0, 4.0}, {6.0, 8.0}},
                                    {{10.0, 12.0}, {14.0, 16.0}}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
        }
        SECTION("operator+(other labeled tensor) const") {
            SECTION("vector") {
                result("i") = const_lvec + lvec;
                auto& rv    = result.get<t_type>();
                t_type corr(world, {2.0, 4.0, 6.0});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("matrix") {
                result("i,j") = const_lmat + lmat;
                auto& rv      = result.get<t_type>();
                t_type corr(world, {{2.0, 4.0}, {6.0, 8.0}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("rank-3 tensor") {
                result("i,j,k") = const_lt3 + lt3;
                auto& rv        = result.get<t_type>();
                t_type corr(world, {{{2.0, 4.0}, {6.0, 8.0}},
                                    {{10.0, 12.0}, {14.0, 16.0}}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
        }
        SECTION("operator+( const other labeled tensor) const") {
            SECTION("vector") {
                result("i") = const_lvec + const_lvec;
                auto& rv    = result.get<t_type>();
                t_type corr(world, {2.0, 4.0, 6.0});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("matrix") {
                result("i,j") = const_lmat + const_lmat;
                auto& rv      = result.get<t_type>();
                t_type corr(world, {{2.0, 4.0}, {6.0, 8.0}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("rank-3 tensor") {
                result("i,j,k") = const_lt3 + const_lt3;
                auto& rv        = result.get<t_type>();
                t_type corr(world, {{{2.0, 4.0}, {6.0, 8.0}},
                                    {{10.0, 12.0}, {14.0, 16.0}}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
        }
        SECTION("operator+(operation)") {
            SECTION("vector") {
                auto op     = lvec + lvec;
                result("i") = lvec + op;
                auto& rv    = result.get<t_type>();
                t_type corr(world, {3.0, 6.0, 9.0});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("matrix") {
                auto op       = lmat + lmat;
                result("i,j") = lmat + op;
                auto& rv      = result.get<t_type>();
                t_type corr(world, {{3.0, 6.0}, {9.0, 12.0}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
            SECTION("rank-3 tensor") {
                auto op         = lt3 + lt3;
                result("i,j,k") = lt3 + op;
                auto& rv        = result.get<t_type>();
                t_type corr(world, {{{3.0, 6.0}, {9.0, 12.0}},
                                    {{15.0, 18.0}, {21.0, 24.0}}});
                REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
            }
        }
    }
    SECTION("operator-(other labeled tensor)") {
        SECTION("vector") {
            result("i") = lvec - lvec;
            auto& rv    = result.get<t_type>();
            t_type corr(world, {0.0, 0.0, 0.0});
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("matrix") {
            result("i,j") = lmat - lmat;
            auto& rv      = result.get<t_type>();
            t_type corr(world, {{0.0, 0.0}, {0.0, 0.0}});
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("rank-3 tensor") {
            result("i,j,k") = lt3 - lt3;
            auto& rv        = result.get<t_type>();
            t_type corr(world,
                        {{{0.0, 0.0}, {0.0, 0.0}}, {{0.0, 0.0}, {0.0, 0.0}}});
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
    }

    SECTION("operator-(operation)") {
        SECTION("vector") {
            auto op     = lvec - lvec;
            result("i") = lvec - op;
            auto& rv    = result.get<t_type>();
            auto& corr  = vec.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("matrix") {
            auto op       = lmat - lmat;
            result("i,j") = lmat - op;
            auto& rv      = result.get<t_type>();
            auto& corr    = mat.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("rank-3 tensor") {
            auto op         = lt3 - lt3;
            result("i,j,k") = lt3 - op;
            auto& rv        = result.get<t_type>();
            auto& corr      = t3.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
    }

    SECTION("operator*(other labeled tensor)") {
        SECTION("vector") {
            result("i") = lvec * lvec;
            t_type corr(world, {1.0, 4.0, 9.0});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("matrix") {
            result("i,j") = lmat * lmat;
            t_type corr(world, {{1.0, 4.0}, {9.0, 16.0}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("rank-3 tensor") {
            result("i,j,k") = lt3 * lt3;
            t_type corr(
              world, {{{1.0, 4.0}, {9.0, 16.0}}, {{25.0, 36.0}, {49.0, 64.0}}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
    }

    // SECTION("operator*(operation)") {
    //     SECTION("vector") {
    //         auto op     = lvec * lvec;
    //         result("i") = lvec * op;
    //         t_type corr(world, {1.0, 8.0, 27.0});
    //         auto& rv = result.get<t_type>();
    //         REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
    //     }
    //     SECTION("matrix") {
    //         auto op       = lmat * lmat;
    //         result("i,j") = lmat * op;
    //         t_type corr(world, {{1.0, 8.0}, {27.0, 64.0}});
    //         auto& rv = result.get<t_type>();
    //         REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
    //     }
    //     SECTION("rank-3 tensor") {
    //         auto op         = lt3 * lt3;
    //         result("i,j,k") = lt3 * op;
    //         t_type corr(world, {{{1.0, 8.0}, {27.0, 64.0}},
    //                             {{125.0, 216.0}, {343.0, 512.0}}});
    //         auto& rv = result.get<t_type>();
    //         REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
    //     }
    // }

    SECTION("operator*(double)") {
        SECTION("vector") {
            result("i") = lvec * 3.0;
            t_type corr(world, {3.0, 6.0, 9.0});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("matrix") {
            result("i,j") = lmat * 3.0;
            t_type corr(world, {{3.0, 6.0}, {9.0, 12.0}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("rank-3 tensor") {
            result("i,j,k") = lt3 * 3.0;
            t_type corr(
              world, {{{3.0, 6.0}, {9.0, 12.0}}, {{15.0, 18.0}, {21.0, 24.0}}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
    }

    SECTION("double * OpWrapper") {
        SECTION("vector") {
            result("i") = 3.0 * lvec;
            t_type corr(world, {3.0, 6.0, 9.0});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("matrix") {
            result("i,j") = 3.0 * lmat;
            t_type corr(world, {{3.0, 6.0}, {9.0, 12.0}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("rank-3 tensor") {
            result("i,j,k") = 3.0 * lt3;
            t_type corr(
              world, {{{3.0, 6.0}, {9.0, 12.0}}, {{15.0, 18.0}, {21.0, 24.0}}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
    }
}

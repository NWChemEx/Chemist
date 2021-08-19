//#include "libchemist/ta_helpers/pow.hpp"
//#include "libchemist/ta_helpers/ta_helpers.hpp"
//#include <catch2/catch.hpp>
//
// using namespace libchemist::ta_helpers;
//
// using scalar_types = std::tuple<double>;
//
// TEMPLATE_LIST_TEST_CASE("hmatrix_pow", "", scalar_types) {
//    using tensor_type = TA::TSpArray<TestType>;
//    using il_type     = TA::detail::matrix_il<TestType>;
//    auto& world       = TA::get_default_world();
//
//    il_type t0_il{{1.0, 2.36704e-01, 7.83949e-18, 0.00000e+00},
//                  {2.36704e-01, 1.0, -2.46745e-17, 0.00000e+00},
//                  {7.83949e-18, -2.46745e-17, 1.0, 0.00000e+00},
//                  {0., 0., 0., 1.0}};
//
//    tensor_type t0(world, t0_il);
//    SECTION("Integer power") {
//        il_type corr_il{
//          {1.0560286045074463, 0.47340795397758484, 1.5678980209093575e-17,
//          0},
//          {0.47340795397758484, 1.0560286045074463, 1.8556332932942957e-18,
//          0}, {1.5678980209093575e-17, 1.8556332932942957e-18, 1, 0}, {0, 0,
//          0, 1}};
//        tensor_type corr(world, corr_il);
//        auto r = hmatrix_pow(t0, 2);
//        REQUIRE(allclose(r, corr));
//    }
//
//    SECTION("Square root") {
//        il_type corr_il{
//          {9.92870035e-01, 1.19201905e-01, 4.69113524e-18, 0.00000000e+00},
//          {1.19201905e-01, 9.92870035e-01, -1.26619859e-17, 0.00000000e+00},
//          {4.69113524e-18, -1.26619859e-17, 1.00000000e+00, 0.00000000e+00},
//          {0.00000000e+00, 0.00000000e+00, 0.00000000e+00, 1.00000000e+00}};
//        tensor_type corr(world, corr_il);
//        auto r = hmatrix_pow(t0, 0.5);
//        REQUIRE(allclose(r, corr));
//    }
//
//    SECTION("Inverse square root") {
//        il_type corr_il{
//          {1.02191089e+00, -1.22688490e-01, -6.34740212e-18, 0.00000000e+00},
//          {-1.22688490e-01, 1.02191089e+00, 1.35149696e-17, 0.00000000e+00},
//          {-6.34740212e-18, 1.35149696e-17, 1.00000000e+00, 0.00000000e+00},
//          {0.00000000e+00, 0.00000000e+00, 0.00000000e+00, 1.00000000e+00}};
//        tensor_type corr(world, corr_il);
//        auto r = hmatrix_pow(t0, -0.5);
//        REQUIRE(allclose(r, corr));
//    }
//}

#include <catch2/catch.hpp>
#include <libchemist/types.hpp>
#include <libchemist/ta_helpers/remove_redundancy.hpp>

using namespace libchemist;

using tensor_type = type::tensor<double>;
using vector_t    = std::initializer_list<double>;
using matrix_t    = std::initializer_list<vector_t>;

namespace {

// Correct overlap of redundant_PAOs_corr
constexpr matrix_t redundant_PAO_overlap_corr{{0.41629351, -0.34700249},
                                              {-0.34700249, 0.41629351}};

// Correct redundant PAOs for C_data and S_data
constexpr matrix_t redundant_PAOs_corr{{0.381648, -0.618352},
                                       {-0.618352, 0.381648}};

// Correct normalized, non-redundant PAOs for C_data and S_data
constexpr matrix_t NRC_corr_data{{0.6358462574920218, -0.8093539841320376},
                                 {0.6358462574920218, 0.8093539841320376}};

// Returns a facade module which returns the eigen values/vectors of STilde
//inline auto eigen_solver() {
//    auto& world = TA::get_default_world();
//    return sde::make_lambda<property_types::SelfAdjointEigenSolver<double>>(
//      [](const tensor_type&) {
//          auto& world = TA::get_default_world();
//          tensor_type evals(world, {0.763296, 0.06929102});
//          tensor_type evecs(
//            world, {{0.70710678, 0.70710678}, {-0.70710678, 0.70710678}});
//          return std::make_pair(evals, evecs);
//      });
//}

} // namespace

TEST_CASE("remove_redundancy") {
    auto& world = TA::get_default_world();
    tensor_type CTilde_corr(world, redundant_PAOs_corr);
    tensor_type STilde_corr(world, redundant_PAO_overlap_corr);

    SECTION("No redundancy") {
        auto NRC = remove_redundancy(CTilde_corr, STilde_corr);
        tensor_type NRC_corr(world, NRC_corr_data);
        REQUIRE(libchemist::allclose(NRC, NRC_corr));
    }

    SECTION("One redundancy") {
        auto NRC = remove_redundancy(CTilde_corr, STilde_corr, 0.1);
        // Note this differs from NRC_corr_data by the second column being 0
        tensor_type NRC_corr(world, {{0.0, -0.8093539841320377}, {0.0, 0.8093539841320377}});
        REQUIRE(libchemist::allclose(NRC, NRC_corr));
    }
}
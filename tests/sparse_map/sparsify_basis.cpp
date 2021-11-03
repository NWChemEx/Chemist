// #include "libchemist/sparse_map/sparse_map.hpp"
// #include <catch2/catch.hpp>

// using namespace libchemist;

// using matrix_il = TA::detail::matrix_il<double>;
// using vector_il = TA::detail::vector_il<double>;

// TEST_CASE("sparsify_basis") {
//     auto& world = TA::get_default_world();

//     sparse_map::ElementIndex i0{0}, i1{1}, i2{2};
//     sparse_map::SparseMapEE sm;
//     sm.add_to_domain(i0, i1);
//     sm.add_to_domain(i2, i0);

//     matrix_il il{vector_il{0, 1, 2}, vector_il{3, 4, 5}, vector_il{6, 7, 8}};
//     TA::TSpArrayD t(world, il);

//     SECTION("One big tile") {
//         type::tensor newt(t);
//         auto sparset = sparse_map::sparsify_basis(newt, sm);
//         REQUIRE(tensor::allclose(sparset, newt));
//     }

//     SECTION("Single element tiles") {
//         TA::TiledRange tr{{0, 1, 2, 3}, {0, 1, 2, 3}};
//         type::tensor newt(TA::retile(t, tr));

//         matrix_il i{vector_il{0, 0, 2}, vector_il{3, 0, 0}, vector_il{0, 0,
//         0}}; TA::TSpArrayD corr_ta(world, i); type::tensor
//         corr(TA::retile(corr_ta, tr));

//         auto sparset = sparse_map::sparsify_basis(newt, sm);
//         REQUIRE(tensor::allclose(sparset, corr));
//     }
// }

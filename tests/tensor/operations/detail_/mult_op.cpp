#include "../../test_tensor.hpp"

using namespace libchemist;

/* Testing Notes:
 *
 * For construction purposes we don't care if the labels make any sense. For
 * example we're fine with `lhs("i0,i1") * rhs("i0;i1")` even though for a
 * tensor times a ToT contraction can only occur over the independent indices.
 * What we are testing for construction purposes is that we've covered all of
 * the possible products.
 *
 * Actual correctness of the `variant` member is tested later
 */
// TEMPLATE_LIST_TEST_CASE("MultOp: CTor", "", tensor::all_tensor_variant_t) {
//     using lhs_t = TestType;
//     using tensor_wrapper =
//     tensor::TensorWrapper<tensor::all_tensor_variant_t>; constexpr bool
//     is_tot = tensor::is_tot_v<lhs_t>; auto lhs_tensors =
//     testing::get_tensors<lhs_t>(); auto rhs_tensors =
//     testing::get_tensors<type::tensor<double>>(); auto rhs_tots    =
//     testing::get_tensors<type::tensor_of_tensors<double>>();

//     SECTION("Construction") {

//     // LHS is non-const
//     for(auto& [lhs_desc, lhs]: lhs_tensors) {
//         tensor_wrapper wrapped_lhs(lhs);
//         auto labeled_lhs = wrapped_lhs(wrapped_lhs.make_annotation());
//         // RHS is non-const normal tensor
//         for(auto& [rhs_desc, rhs] : rhs_tensors){
//             tensor_wrapper wrapped_rhs(rhs);
//             auto labeled_rhs = wrapped_rhs(wrapped_rhs.make_annotation());
//             SECTION(lhs_desc + " times " + rhs_desc){
//                 REQUIRE_NOTHROW(labeled_lhs * labeled_rhs);
//             }
//         }

//         // RHS is non-const tensor-of-tensors
//         for(auto& [rhs_desc, rhs] : rhs_tots){
//             tensor_wrapper wrapped_rhs(rhs);
//             auto labeled_rhs = wrapped_rhs(wrapped_rhs.make_annotation());
//             SECTION(lhs_desc + "times " + rhs_desc){
//                 REQUIRE_NOTHROW(labeled_lhs * labeled_rhs);
//             }
//         }
//     }
//     }
// }

TEST_CASE("Tensor = Tensor * Tensor") {
    using result_t       = type::tensor<double>;
    using lhs_t          = type::tensor<double>;
    using rhs_t          = type::tensor<double>;
    using tensor_wrapper = tensor::SparseTensorWrapper;

    auto lhs = testing::get_tensors<lhs_t>().at("vector");
    auto rhs = testing::get_tensors<rhs_t>().at("vector");

    auto& world = TA::get_default_world();
    result_t corr(world, {{1.0, 2.0, 3.0}, {2.0, 4.0, 6.0}, {3.0, 6.0, 9.0}});

    SECTION("all non-const") {
        tensor_wrapper wrapped_lhs(lhs);
        tensor_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i,j") = wrapped_lhs("i") * wrapped_rhs("j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }
}

// #include "chemist/tensor/tensor.hpp"
// #include <catch2/catch.hpp>

// using namespace chemist::tensor;

// TEST_CASE("LabeledTensorWrapper") {
//     using tensor         = ScalarTensorWrapper;
//     using labeled_tensor = detail_::LabeledTensorWrapper<tensor>;
//     using ta_tensor_type = TA::TSpArrayD;
//     using vector_il      = TA::detail::vector_il<int>;

//     auto& world = TA::get_default_world();
//     ta_tensor_type ta_tensor(world, vector_il{1, 2, 3});
//     tensor t(ta_tensor);
//     labeled_tensor lt("i", t);

//     SECTION("Typedefs") {
//         SECTION("annotation_type") {
//             using type = typename labeled_tensor::annotation_type;
//             using corr = std::string;
//             STATIC_REQUIRE(std::is_same_v<type, corr>);
//         }

//         SECTION("tensor_wrapper_type") {
//             using type = typename labeled_tensor::tensor_wrapper_type;
//             using corr = tensor;
//             STATIC_REQUIRE(std::is_same_v<type, corr>);
//         }
//     }
// }

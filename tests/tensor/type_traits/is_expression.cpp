#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/type_traits/is_expression.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

TEST_CASE("is_expression_v") {
    STATIC_REQUIRE_FALSE(is_expression_v<type::SparseTensorWrapper>);
    STATIC_REQUIRE(
      is_expression_v<detail_::LabeledTensorWrapper<type::ToTWrapper>>);
}

TEST_CASE("enable_if_expression_t") {
    using expr = detail_::LabeledTensorWrapper<type::ToTWrapper>;
    using type = enable_if_expression_t<expr, int>;
    STATIC_REQUIRE(std::is_same_v<type, int>);
}

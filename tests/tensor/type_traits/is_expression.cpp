#include "libchemist/tensor/type_traits/is_expression.hpp"
#include "libchemist/tensor/tensor_wrapper.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

TEST_CASE("is_expression_v"){
    STATIC_REQUIRE_FALSE(is_expression_v<SparseTensorWrapper>);
    STATIC_REQUIRE(is_expression_v<detail_::LabeledTensorWrapper<ToTWrapper>>);
}

TEST_CASE("enable_if_expression_t"){
    using expr = detail_::LabeledTensorWrapper<ToTWrapper>;
    using type = enable_if_expression_t<expr, int>;
    STATIC_REQUIRE(std::is_same_v<type, int>);
}

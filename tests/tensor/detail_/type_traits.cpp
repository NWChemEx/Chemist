#include "libchemist/tensor/detail_/type_traits.hpp"
#include "libchemist/tensor/types.hpp"
#include "libchemist/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;
using TensorD = libchemist::type::tensor<double>;
using TensorF = libchemist::type::tensor<float>;
using ToTD    = libchemist::type::tensor_of_tensors<double>;
using ToTF    = libchemist::type::tensor_of_tensors<float>;

TEMPLATE_LIST_TEST_CASE("labeled_tensor_t", "",  tensor_variant_t) {
    using tensor_type  = TestType;
    using corr_type    = decltype(std::declval<TestType>()("i,j"));
    using labeled_type = labeled_tensor_t<tensor_type>;
    STATIC_REQUIRE(std::is_same_v<labeled_type, corr_type>);
}

TEST_CASE("labeled_variant") {
    using t0 = labeled_tensor_t<TensorD>;
    // using t2        = labeled_tensor_t<tensor_t<float>>;
    using type = typename detail_::labeled_variant<tensor_variant_t>::type;
    using corr_type = std::variant<t0>; //, t2>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}

TEST_CASE("labeled_variant_t") {
    using t0        = labeled_tensor_t<TensorD>;
    using t2        = labeled_tensor_t<TensorF>;
    using type      = labeled_variant_t<tensor_variant_t>;
    using corr_type = std::variant<t0>; //, t2>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}

TEMPLATE_LIST_TEST_CASE("add_expr_t", "", tensor_variant_t) {
    using lhs_type = labeled_tensor_t<TestType>;
    SECTION("RHS is tensor") {
        using rhs_type = labeled_tensor_t<TensorD>;
        using type     = add_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() + std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
    SECTION("RHS is tensor of tensors") {
        using rhs_type = labeled_tensor_t<ToTD>;
        using type     = add_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() + std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
}

TEMPLATE_LIST_TEST_CASE("subt_expr_t", "", tensor_variant_t) {
    using lhs_type = labeled_tensor_t<TestType>;
    SECTION("RHS is tensor") {
        using rhs_type = labeled_tensor_t<TensorD>;
        using type     = subt_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() - std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
    SECTION("RHS is tensor of tensors") {
        using rhs_type = labeled_tensor_t<ToTD>;
        using type     = subt_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() - std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
}

TEMPLATE_LIST_TEST_CASE("mult_expr_t", "", tensor_variant_t) {
    using lhs_type = labeled_tensor_t<TestType>;
    SECTION("RHS is tensor") {
        using rhs_type = labeled_tensor_t<TensorD>;
        using type     = mult_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() * std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
    SECTION("RHS is tensor of tensors") {
        using rhs_type = labeled_tensor_t<ToTD>;
        using type     = mult_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() * std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
}

TEST_CASE("add_variant_t") {
    using t0        = labeled_tensor_t<TensorD>;
    using t1        = labeled_tensor_t<ToTF>;
    using t_variant = std::variant<t0, t1>;
    using t00       = add_expr_t<t0, t0>;
    using t01       = add_expr_t<t0, t1>;
    using t10       = add_expr_t<t1, t0>;
    using t11       = add_expr_t<t1, t1>;
    using corr_type = std::variant<t00, t01, t10, t11>;
    using type      = add_variant_t<t_variant, t_variant>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}

TEST_CASE("subt_variant_t") {
    using t0        = labeled_tensor_t<TensorD>;
    using t1        = labeled_tensor_t<ToTF>;
    using t_variant = std::variant<t0, t1>;
    using t00       = subt_expr_t<t0, t0>;
    using t01       = subt_expr_t<t0, t1>;
    using t10       = subt_expr_t<t1, t0>;
    using t11       = subt_expr_t<t1, t1>;
    using corr_type = std::variant<t00, t01, t10, t11>;
    using type      = subt_variant_t<t_variant, t_variant>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}

TEST_CASE("mult_variant_t") {
    using t0        = labeled_tensor_t<TensorD>;
    using t1        = labeled_tensor_t<ToTF>;
    using t_variant = std::variant<t0, t1>;
    using t00       = mult_expr_t<t0, t0>;
    using t01       = mult_expr_t<t0, t1>;
    using t10       = mult_expr_t<t1, t0>;
    using t11       = mult_expr_t<t1, t1>;
    using corr_type = std::variant<t00, t01, t10, t11>;
    using type      = mult_variant_t<t_variant, t_variant>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}

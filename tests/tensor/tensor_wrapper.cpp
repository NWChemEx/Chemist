#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

TEMPLATE_LIST_TEST_CASE("TensorWrapper", "", tensor_variant_t) {
    auto& world    = TA::get_default_world();
    using TWrapper = TensorWrapper<tensor_variant_t>;
    using t_type   = TestType;

    TWrapper vec(t_type(world, {1.0, 2.0, 3.0}));
    TWrapper mat(t_type(world, {{1.0, 2.0}, {3.0, 4.0}}));
    TWrapper t3(
      t_type(world, {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}}));

    SECTION("Wrapping CTor") {
        REQUIRE(vec.rank() == 1);
        REQUIRE(mat.rank() == 2);
        REQUIRE(t3.rank() == 3);
    }

    SECTION("make_annotation") {
        REQUIRE(vec.make_annotation() == "i0");
        REQUIRE(mat.make_annotation("j") == "j0,j1");
        REQUIRE(t3.make_annotation() == "i0,i1,i2");
    }

    SECTION("operator()") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(vec("i"));
        REQUIRE_NOTHROW(mat("i,j"));
        REQUIRE_NOTHROW(t3("i,j,k"));
    }

    SECTION("operator() const") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(std::as_const(vec)("i"));
        REQUIRE_NOTHROW(std::as_const(mat)("i,j"));
        REQUIRE_NOTHROW(std::as_const(t3)("i,j,k"));
    }
}

TEMPLATE_LIST_TEST_CASE("TensorWrapper", "", tot_variant_t) {
    auto& world      = TA::get_default_world();
    using TWrapper   = TensorWrapper<tot_variant_t>;
    using t_type     = TestType;
    using tile_type  = typename TestType::value_type;
    using inner_tile = typename tile_type::value_type;

    inner_tile vec0(TA::Range({0}, {2}), {1.0, 2.0});
    inner_tile vec1(TA::Range({0}, {3}), {3.0, 4.0, 5.0});
    inner_tile mat0(TA::Range({0, 0}, {2, 3}), {1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

    TWrapper mat(t_type(world, {vec0, vec1}));
    TWrapper t3(t_type(world, {{vec0, vec1}, {vec0, vec1}}));
    TWrapper t3_2(t_type(world, {mat0, mat0}));

    SECTION("Wrapping CTor") {
        REQUIRE(mat.rank() == 2);
        REQUIRE(t3.rank() == 3);
        REQUIRE(t3_2.rank() == 3);
    }

    SECTION("make_annotation") {
        REQUIRE(mat.make_annotation("j") == "j0;j1");
        REQUIRE(t3.make_annotation() == "i0,i1;i2");
        REQUIRE(t3_2.make_annotation() == "i0;i1,i2");
    }

    SECTION("operator()") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(mat("i;j"));
        REQUIRE_NOTHROW(t3("i,j;k"));
        REQUIRE_NOTHROW(t3_2("i;j,k"));
    }

    SECTION("operator() const") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(std::as_const(mat)("i;j"));
        REQUIRE_NOTHROW(std::as_const(t3)("i,j;k"));
        REQUIRE_NOTHROW(std::as_const(t3_2)("i;j,k"));
    }
}
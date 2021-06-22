#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/tensor.hpp"
#include "libchemist/tensor/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

TEMPLATE_LIST_TEST_CASE("TensorWrapper", "", type::tensor_variant) {
    auto& world    = TA::get_default_world();
    using TWrapper = TensorWrapper<type::tensor_variant>;
    using t_type   = TestType;

    t_type vec_data(world, {1.0, 2.0, 3.0});
    t_type mat_data(world, {{1.0, 2.0}, {3.0, 4.0}});
    t_type t3_data(world, {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}});

    TWrapper vec(vec_data);
    TWrapper mat(mat_data);
    TWrapper t3(t3_data);

    SECTION("Typedefs") {
        SECTION("variant_type") {
            using vtype = typename TWrapper::variant_type;
            using corr  = type::tensor_variant;
            STATIC_REQUIRE(std::is_same_v<vtype, corr>);
        }
        SECTION("annotation_type") {
            using type = typename TWrapper::annotation_type;
            using corr = std::string;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
        SECTION("labeled_tensor_type") {
            using type = typename TWrapper::labeled_tensor_type;
            using corr = detail_::LabeledTensorWrapper<TWrapper>;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
        SECTION("const_labeled_tensor_type") {
            using type = typename TWrapper::const_labeled_tensor_type;
            using corr = detail_::LabeledTensorWrapper<const TWrapper>;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
    }

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

    SECTION("rank()") {
        REQUIRE(vec.rank() == 1);
        REQUIRE(mat.rank() == 2);
        REQUIRE(t3.rank() == 3);
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

    SECTION("get()") {
        using libchemist::ta_helpers::allclose;
        SECTION("Vector") { REQUIRE(allclose(vec.get<t_type>(), vec_data)); }
        SECTION("Matrix") { REQUIRE(allclose(mat.get<t_type>(), mat_data)); }
        SECTION("Rank 3 Tenosr") {
            REQUIRE(allclose(t3.get<t_type>(), t3_data));
        }
    }

    SECTION("get() const") {
        using libchemist::ta_helpers::allclose;
        SECTION("Vector") {
            REQUIRE(allclose(std::as_const(vec).get<t_type>(), vec_data));
        }
        SECTION("Matrix") {
            REQUIRE(allclose(std::as_const(mat).get<t_type>(), mat_data));
        }
        SECTION("Rank 3 Tenosr") {
            REQUIRE(allclose(std::as_const(t3).get<t_type>(), t3_data));
        }
    }

    SECTION("print") {
        std::stringstream ss;
        auto pss = &(vec.print(ss));
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [3] ) { 1 2 3 }\n";
            REQUIRE(corr == ss.str());
        }
    }

    SECTION("operator<<") {
        std::stringstream ss;
        auto pss = &(ss << vec);
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [3] ) { 1 2 3 }\n";
            REQUIRE(corr == ss.str());
        }
    }
}

TEMPLATE_LIST_TEST_CASE("TensorWrapper", "", type::tot_variant) {
    auto& world      = TA::get_default_world();
    using TWrapper   = TensorWrapper<type::tot_variant>;
    using t_type     = TestType;
    using tile_type  = typename TestType::value_type;
    using inner_tile = typename tile_type::value_type;

    inner_tile vec0(TA::Range({0}, {2}), {1.0, 2.0});
    inner_tile vec1(TA::Range({0}, {3}), {3.0, 4.0, 5.0});
    inner_tile mat0(TA::Range({0, 0}, {2, 3}), {1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

    t_type mat_data(world, {vec0, vec1});
    t_type t3_data(world, {{vec0, vec1}, {vec0, vec1}});
    t_type t3_2_data(world, {mat0, mat0});

    TWrapper mat(mat_data);
    TWrapper t3(t3_data);
    TWrapper t3_2(t3_2_data);

    SECTION("Typedefs") {
        SECTION("variant_type") {
            using corr = type::tot_variant;
            using type = typename TWrapper::variant_type;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
        SECTION("annotation_type") {
            using type = typename TWrapper::annotation_type;
            using corr = std::string;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
        SECTION("labeled_tensor_type") {
            using type = typename TWrapper::labeled_tensor_type;
            using corr = detail_::LabeledTensorWrapper<TWrapper>;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
        SECTION("const_labeled_tensor_type") {
            using type = typename TWrapper::const_labeled_tensor_type;
            using corr = detail_::LabeledTensorWrapper<const TWrapper>;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
    }

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

    SECTION("rank()") {
        REQUIRE(mat.rank() == 2);
        REQUIRE(t3.rank() == 3);
        REQUIRE(t3_2.rank() == 3);
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

    SECTION("get()") {
        using libchemist::ta_helpers::allclose_tot;
        SECTION("Matrix") {
            REQUIRE(allclose_tot(mat.get<t_type>(), mat_data, 1));
        }
        SECTION("Rank 3 Tensor") {
            REQUIRE(allclose_tot(t3.get<t_type>(), t3_data, 2));
        }
        SECTION("Rank 3 Tensor - 2") {
            REQUIRE(allclose_tot(t3_2.get<t_type>(), t3_2_data, 1));
        }
    }

    SECTION("get() const") {
        using libchemist::ta_helpers::allclose_tot;
        using std::as_const;
        SECTION("Matrix") {
            REQUIRE(allclose_tot(as_const(mat).get<t_type>(), mat_data, 1));
        }
        SECTION("Rank 3 Tensor") {
            REQUIRE(allclose_tot(as_const(t3).get<t_type>(), t3_data, 2));
        }
        SECTION("Rank 3 Tensor - 2") {
            REQUIRE(allclose_tot(as_const(t3_2).get<t_type>(), t3_2_data, 1));
        }
    }

    SECTION("print") {
        std::stringstream ss;
        auto pss = &(mat.print(ss));
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [2] ) {\n  [0]:[ [0], [2] ) { 1 2 }";
            corr += "\n  [1]:[ [0], [3] ) { 3 4 5 }\n}\n";
            REQUIRE(corr == ss.str());
        }
    }

    SECTION("operator<<") {
        std::stringstream ss;
        auto pss = &(ss << mat);
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [2] ) {\n  [0]:[ [0], [2] ) { 1 2 }";
            corr += "\n  [1]:[ [0], [3] ) { 3 4 5 }\n}\n";
            REQUIRE(corr == ss.str());
        }
    }
}

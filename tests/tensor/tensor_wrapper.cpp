#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/tensor.hpp"
#include "libchemist/tensor/types.hpp"
#include "libchemist/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

TEMPLATE_LIST_TEST_CASE("TensorWrapper", "", type::tensor_variant) {
    auto& world    = TA::get_default_world();
    using TWrapper = TensorWrapper<type::tensor_variant>;
    using t_type   = TestType;
    using extents  = typename TWrapper::extents_type;

    t_type vec_data(world, {1.0, 2.0, 3.0});
    t_type mat_data(world, {{1.0, 2.0}, {3.0, 4.0}});
    t_type t3_data(world, {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}});

    TWrapper defaulted;

    TWrapper vec(vec_data);
    TWrapper mat(mat_data);
    TWrapper t3(t3_data);

    SECTION("Typedefs") {
        SECTION("variant_type") {
            using type = typename TWrapper::variant_type;
            using corr = libchemist::tensor::type::tensor_variant;
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
        SECTION("size_type") {
            using type = typename TWrapper::size_type;
            using corr = std::size_t;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
        SECTION("extents_type") {
            using corr = std::vector<std::size_t>;
            STATIC_REQUIRE(std::is_same_v<extents, corr>);
        }
    }

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.rank() == 0);
            REQUIRE(defaulted.extents() == extents{});
        }

        SECTION("Wrapping CTor") {
            REQUIRE(vec.rank() == 1);
            REQUIRE(vec.extents() == extents{3});
            REQUIRE(mat.rank() == 2);
            REQUIRE(mat.extents() == extents{2, 2});
            REQUIRE(t3.rank() == 3);
            REQUIRE(t3.extents() == extents{2, 2, 2});
        }

        SECTION("Copy") {
            TWrapper copied(vec);
            REQUIRE(copied.rank() == 1);
            REQUIRE(copied.extents() == extents{3});
        }

        SECTION("Move") {
            TWrapper moved(std::move(vec));
            REQUIRE(moved.rank() == 1);
            REQUIRE(moved.extents() == extents{3});
        }

        SECTION("Copy assignment") {
            TWrapper copied;
            auto pcopied = &(copied = vec);
            REQUIRE(pcopied == &copied);
            REQUIRE(copied.rank() == 1);
            REQUIRE(copied.extents() == extents{3});
        }

        SECTION("Move assignment") {
            TWrapper moved;
            auto pmoved = &(moved = std::move(vec));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.rank() == 1);
            REQUIRE(moved.extents() == extents{3});
        }
    }

    SECTION("make_annotation") {
        REQUIRE(defaulted.make_annotation() == "");
        REQUIRE(vec.make_annotation() == "i0");
        REQUIRE(mat.make_annotation("j") == "j0,j1");
        REQUIRE(t3.make_annotation() == "i0,i1,i2");
    }

    SECTION("rank()") {
        REQUIRE(defaulted.rank() == 0);
        REQUIRE(vec.rank() == 1);
        REQUIRE(mat.rank() == 2);
        REQUIRE(t3.rank() == 3);
    }

    SECTION("extents()") {
        REQUIRE(defaulted.extents() == extents{});
        REQUIRE(vec.extents() == extents{3});
        REQUIRE(mat.extents() == extents{2, 2});
        REQUIRE(t3.extents() == extents{2, 2, 2});
    }

    SECTION("slice()") {
        SECTION("Vector") {
            TWrapper corr(t_type(world, {1.0, 2.0}));
            auto slice = vec.slice({0ul}, {2ul});
            REQUIRE(slice == corr);
        }
        SECTION("Matrix") {
            TWrapper corr(t_type(world, {{2.0}}));
            auto slice = mat.slice({0ul, 1ul}, {1ul, 2ul});
            REQUIRE(slice == corr);
        }
        SECTION("Tensor") {
            TWrapper corr(t_type(world, {{{2.0}, {4.0}}, {{6.0}, {8.0}}}));
            auto slice = t3.slice({0ul, 0ul, 1ul}, {2ul, 2ul, 2ul});
            REQUIRE(slice == corr);
        }
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

    SECTION("hash") {
        TWrapper other_vec(vec_data);
        REQUIRE(pluginplay::hash_objects(other_vec) ==
                pluginplay::hash_objects(vec));
        REQUIRE(pluginplay::hash_objects(vec) != pluginplay::hash_objects(mat));
    }

    SECTION("comparisons") {
        SECTION("Are same") {
            TWrapper other_vec(vec_data);
            REQUIRE(vec == other_vec);
            REQUIRE_FALSE(vec != other_vec);
        }
        SECTION("Different") {
            REQUIRE_FALSE(vec == mat);
            REQUIRE(vec != mat);
        }
    }
}

TEMPLATE_LIST_TEST_CASE("TensorWrapper", "", type::tot_variant) {
    auto& world        = TA::get_default_world();
    using variant_type = type::tot_variant;
    using TWrapper     = TensorWrapper<type::tot_variant>;
    using t_type       = TestType;
    using tile_type    = typename TestType::value_type;
    using inner_tile   = typename tile_type::value_type;

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
            using type = typename TWrapper::variant_type;
            using corr = variant_type;
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

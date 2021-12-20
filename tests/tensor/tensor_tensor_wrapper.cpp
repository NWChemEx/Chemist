#include "test_tensor.hpp"

using namespace chemist::tensor;

/* Testing Strategy:
 *
 * The actual TensorWrapper class is a pass through to the PIMPL in many
 * circumstances. For these unit tests we assume that the PIMPLs work and are
 * thoroughly tested. Thus for functions which are just pass throughs, we simply
 * need to ensure that arguments and returns are forwarded correctly.
 */

using tot_traits  = backends::TiledArrayTraits<field::Tensor>;
using tot_variant = typename tot_traits::variant_type;

TEMPLATE_LIST_TEST_CASE("TensorWrapper<Tensor>", "", tot_variant) {
    using field_type   = field::Tensor;
    using TWrapper     = TensorWrapper<field_type>;
    using shape_type   = typename TWrapper::shape_type;
    using extents_type = typename TWrapper::extents_type;
    using t_type       = TestType;

    // Assumed different type than the one returned by default_allocator
    using other_alloc = SingleElementTiles<field_type>;

    auto ta_tensors = testing::get_tensors<t_type>();
    t_type vov_data = ta_tensors.at("vector-of-vectors");
    t_type mov_data = ta_tensors.at("matrix-of-vectors");
    t_type vom_data = ta_tensors.at("vector-of-matrices");

    TWrapper vov(vov_data);
    TWrapper mov(mov_data);
    TWrapper vom(vom_data);

    SECTION("CTors") {
        SECTION("Wrapping CTor") {
            REQUIRE(vov.rank() == 2);
            REQUIRE(mov.rank() == 3);
            REQUIRE(vom.rank() == 3);
        }
        SECTION("From normal tensor") {
            using scalar_tensor  = TensorWrapper<field::Scalar>;
            using scalar_traits  = backends::TiledArrayTraits<field::Scalar>;
            using scalar_variant = typename scalar_traits::variant_type;
            using ta_type      = std::variant_alternative_t<0, scalar_variant>;
            using sparse_shape = SparseShape<field::Tensor>;
            using sparse_map   = typename sparse_shape::sparse_map_type;
            using index_type   = typename sparse_map::key_type;

            auto scalar_tensors = testing::get_tensors<ta_type>();

            index_type i0{0}, i1{1};
            sparse_map sm{{i0, {i1}}, {i1, {i0}}};
            auto shape = std::make_unique<sparse_shape>(extents_type{2}, sm);
            // SECTION("Vector to vector-of-vectors") {
            //     const auto& v = scalar_tensors.at("vector");
            //     TWrapper vov2(v, std::move(shape));
            //     std::cout << vov2 << std::endl;
            // }

            SECTION("Matrix to vector-of-vectors") {
                using outer_tile = typename t_type::value_type;
                using inner_tile = typename outer_tile::value_type;

                inner_tile v0(TA::Range({1}), {2.0});
                inner_tile v1(TA::Range({1}), {3.0});
                t_type corr_data(vov.allocator().runtime(), {v0, v1});
                TWrapper corr(std::move(corr_data), shape->clone());

                scalar_tensor m(scalar_tensors.at("matrix"));
                TWrapper vov2(m, std::move(shape));
                REQUIRE(vov2 == corr);
            }
        }
    }

    SECTION("make_annotation") {
        REQUIRE(vov.make_annotation("j") == "j0;j1");
        REQUIRE(mov.make_annotation() == "i0,i1;i2");
        REQUIRE(vom.make_annotation() == "i0;i1,i2");
    }

    SECTION("rank()") {
        REQUIRE(vov.rank() == 2);
        REQUIRE(mov.rank() == 3);
        REQUIRE(vom.rank() == 3);
    }

    SECTION("operator()") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(vov("i;j"));
        REQUIRE_NOTHROW(mov("i,j;k"));
        REQUIRE_NOTHROW(vom("i;j,k"));
    }

    SECTION("operator() const") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(std::as_const(vov)("i;j"));
        REQUIRE_NOTHROW(std::as_const(mov)("i,j;k"));
        REQUIRE_NOTHROW(std::as_const(vom)("i;j,k"));
    }

    SECTION("print") {
        std::stringstream ss;
        auto pss = &(vov.print(ss));
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [2] ) {\n"
                               "  [0]:[ [0], [2] ) { 1 2 }\n"
                               "  [1]:[ [0], [2] ) { 3 4 }\n"
                               "}\n";
            REQUIRE(corr == ss.str());
        }
    }

    SECTION("operator<<") {
        std::stringstream ss;
        auto pss = &(ss << vov);
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [2] ) {\n"
                               "  [0]:[ [0], [2] ) { 1 2 }\n"
                               "  [1]:[ [0], [2] ) { 3 4 }\n"
                               "}\n";
            REQUIRE(corr == ss.str());
        }
    }
}

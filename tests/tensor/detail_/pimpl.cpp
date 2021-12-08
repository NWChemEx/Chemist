#include "../test_tensor.hpp"
#include "chemist/tensor/detail_/pimpl.hpp"

using namespace chemist::tensor;

/* Testing Strategy:
 *
 * We assume that all allocators and shapes work correctly. this means that
 * functions which depend on the shape and allocator state should work correctly
 * as long as those functions properly call and process the results of
 * interacting with allocators/shapes. In practice there are a lot of
 */

TEST_CASE("TensorWrapperPIMPL") {
    using field_type     = field::Scalar;
    using pimpl_type     = detail_::TensorWrapperPIMPL<field_type>;
    using variant_type   = typename pimpl_type::variant_type;
    using ta_tensor_type = std::variant_alternative_t<0, variant_type>;
    using shape_type     = typename pimpl_type::shape_type;
    using extents_type   = typename pimpl_type::extents_type;

    using default_alloc = OneBigTile<field_type>;
    using other_alloc   = SingleElementTiles<field_type>;

    auto alloc   = std::make_unique<default_alloc>();
    auto tensors = testing::get_tensors<ta_tensor_type>();

    variant_type vector{tensors.at("vector")};
    variant_type matrix{tensors.at("matrix")};
    variant_type tensor{tensors.at("tensor")};

    pimpl_type v(vector, alloc->clone());
    pimpl_type m(matrix, alloc->clone());
    pimpl_type t(tensor, alloc->clone());

    auto v_shape = std::make_unique<shape_type>(extents_type{3});
    auto m_shape = std::make_unique<shape_type>(extents_type{2, 2});
    auto t_shape = std::make_unique<shape_type>(extents_type{2, 2, 2});

    pimpl_type v2(vector, v_shape->clone(), alloc->clone());
    pimpl_type m2(matrix, m_shape->clone(), alloc->clone());
    pimpl_type t2(tensor, t_shape->clone(), alloc->clone());

    SECTION("CTors") {
        SECTION("No Shape") {
            REQUIRE(v.allocator() == *alloc);
            REQUIRE(std::get<0>(v.variant()) == tensors.at("vector"));
            REQUIRE(v.size() == 3);

            REQUIRE(m.allocator() == *alloc);
            REQUIRE(std::get<0>(m.variant()) == tensors.at("matrix"));
            REQUIRE(m.size() == 4);

            REQUIRE(t.allocator() == *alloc);
            REQUIRE(std::get<0>(t.variant()) == tensors.at("tensor"));
            REQUIRE(t.size() == 8);

            SECTION("Reallocates if necessary") {
                auto new_alloc = std::make_unique<other_alloc>();
                auto tr        = new_alloc->make_tiled_range(extents_type{3});
                ta_tensor_type corr(new_alloc->runtime(), tr, {1, 2, 3});

                pimpl_type v3(vector, new_alloc->clone());
                REQUIRE(v3.allocator() == *alloc);
                REQUIRE(std::get<0>(v3.variant()) == corr);
                REQUIRE(v3.size() == 3);
            }
        }

        SECTION("With shape") {
            REQUIRE(v2.allocator() == *alloc);
            REQUIRE(std::get<0>(v2.variant()) == tensors.at("vector"));
            REQUIRE(v2.size() == 3);

            REQUIRE(m2.allocator() == *alloc);
            REQUIRE(std::get<0>(m2.variant()) == tensors.at("matrix"));
            REQUIRE(m2.size() == 4);

            REQUIRE(t2.allocator() == *alloc);
            REQUIRE(std::get<0>(t2.variant()) == tensors.at("tensor"));
            REQUIRE(t2.size() == 8);

            // TODO: Check that it resizes

            SECTION("Reallocates if necessary") {
                auto new_alloc = std::make_unique<other_alloc>();
                auto tr        = new_alloc->make_tiled_range(extents_type{3});
                ta_tensor_type corr(new_alloc->runtime(), tr, {1, 2, 3});

                pimpl_type v3(vector, v_shape->clone(), new_alloc->clone());
                REQUIRE(v3.allocator() == *alloc);
                REQUIRE(std::get<0>(v3.variant()) == corr);
                REQUIRE(v3.size() == 3);
            }
        }

        SECTION("clone") {}
    }

    SECTION("Allocator") {
        REQUIRE(v.allocator() == *alloc);
        REQUIRE(m.allocator() == *alloc);
        REQUIRE(t.allocator() == *alloc);
    }
}

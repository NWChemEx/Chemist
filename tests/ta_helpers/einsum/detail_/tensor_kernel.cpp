#include <catch2/catch.hpp>
#include "libchemist/ta_helpers/einsum/einsum.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"

using namespace libchemist::ta_helpers::einsum;
using namespace libchemist::ta_helpers::einsum::detail_;

TEST_CASE("get_block_ranges") {
    using block_map_t = std::map<types::index, TA::TiledRange1>;

    SECTION("Empty ranges") {
        auto rv = get_block_ranges(block_map_t{});
        REQUIRE(rv == types::assoc_range{});
    }

    SECTION("Single range") {
        block_map_t inp{{"i", TA::TiledRange1{0, 5, 10, 15}}};
        types::assoc_range corr{{"i", {0, 3}}};
        REQUIRE(get_block_ranges(inp) == corr);
    }

    SECTION("Two ranges") {
        block_map_t inp{{"i", TA::TiledRange1{0, 5, 10, 15}},
                        {"j", TA::TiledRange1{0, 3, 9, 12, 15}}};
        types::assoc_range corr{{"i", {0, 3}}, {"j", {0, 4}}};
        REQUIRE(get_block_ranges(inp) == corr);
    }
}

TEST_CASE("get_block_range") {
    using block_map_t = std::map<types::index, TA::TiledRange1>;

    SECTION("Empty ranges") {
        auto rv = get_block_range(types::assoc_index{}, block_map_t{});
        REQUIRE(rv == types::assoc_range{});
    }

    SECTION("Single range") {
        block_map_t inp{{"i", TA::TiledRange1{0, 5, 10, 15}}};
        SECTION("Block 0") {
            types::assoc_index idx{{"i", 0}};
            types::assoc_range corr{{"i", {0, 5}}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }

        SECTION("Block 1") {
            types::assoc_index idx{{"i", 1}};
            types::assoc_range corr{{"i", {5, 10}}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }

        SECTION("Block 2") {
            types::assoc_index idx{{"i", 2}};
            types::assoc_range corr{{"i", {10, 15}}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }
    }

    SECTION("Two ranges") {
        block_map_t inp{{"i", TA::TiledRange1{0, 5, 10, 15}},
                        {"j", TA::TiledRange1{0, 3, 9}}};

        SECTION("Block 0,0") {
            types::assoc_range corr{{"i", {0, 5}}, {"j", {0, 3}}};
            types::assoc_index idx{{"i", 0}, {"j", 0}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }

        SECTION("Block 0,1") {
            types::assoc_range corr{{"i", {0, 5}}, {"j", {3, 9}}};
            types::assoc_index idx{{"i", 0}, {"j", 1}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }

        SECTION("Block 1,0") {
            types::assoc_range corr{{"i", {5, 10}}, {"j", {0, 3}}};
            types::assoc_index idx{{"i", 1}, {"j", 0}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }

        SECTION("Block 1,1") {
            types::assoc_range corr{{"i", {5, 10}}, {"j", {3, 9}}};
            types::assoc_index idx{{"i", 1}, {"j", 1}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }

        SECTION("Block 2,0") {
            types::assoc_range corr{{"i", {10, 15}}, {"j", {0, 3}}};
            types::assoc_index idx{{"i", 2}, {"j", 0}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }

        SECTION("Block 2,1") {
            types::assoc_range corr{{"i", {10, 15}}, {"j", {3, 9}}};
            types::assoc_index idx{{"i", 2}, {"j", 1}};
            REQUIRE(get_block_range(idx, inp) == corr);
        }
    }
}

TEST_CASE("collapse_range") {
    SECTION("Single range") {
        types::assoc_range ranges{{"i", {0, 3}}};

        SECTION("No collapse") {
            types::index_set idx_set{};
            std::vector<std::size_t> idx;
            auto result = collapse_range(idx_set, idx, ranges);
            REQUIRE(result == ranges);
        }

        SECTION("Collapse index i") {
            types::index_set idx_set{"i"};
            std::vector<std::size_t> idx{1};
            types::assoc_range corr{{"i", {1, 2}}};
            auto result = collapse_range(idx_set, idx, ranges);
            REQUIRE(result == corr);
        }
    }

    SECTION("Two ranges") {
        types::assoc_range ranges{{"i", {0, 3}}, {"j", {3, 5}}};

        SECTION("No collapse") {
            types::index_set idx_set{};
            std::vector<std::size_t> idx;
            auto result = collapse_range(idx_set, idx, ranges);
            REQUIRE(result == ranges);
        }

        SECTION("Collapse index i") {
            types::index_set idx_set{"i"};
            std::vector<std::size_t> idx{1};
            types::assoc_range corr{{"i", {1, 2}}, {"j", {3, 5}}};
            auto result = collapse_range(idx_set, idx, ranges);
            REQUIRE(result == corr);
        }

        SECTION("Collapse index j") {
            types::index_set idx_set{"j"};
            std::vector<std::size_t> idx{3};
            types::assoc_range corr{{"i", {0, 3}}, {"j", {3, 4}}};
            auto result = collapse_range(idx_set, idx, ranges);
            REQUIRE(result == corr);
        }

        SECTION("Collapse both indices") {
            types::index_set idx_set{"i", "j"};
            std::vector<std::size_t> idx{1, 3};
            types::assoc_range corr{{"i", {1, 2}}, {"j", {3, 4}}};
            auto result = collapse_range(idx_set, idx, ranges);
            REQUIRE(result == corr);
        }
    }
}

/* tensor_kernel loops over blocks of the tensors and combines them using the
 * provided kernel. We know the provided kernel (block_kernel) works from other
 * unit tests hence we need to make sure that tensor_kernel works with:
 * - multiple blocks
 * - non-local blocks (punted on)
 * - multiple indices
 *
 */
TEST_CASE("tensor_kernel") {
    auto& world     = TA::get_default_world();
    using tile_type = TA::TensorD;
    using range_t   = std::map<types::index, TA::TiledRange1>;

    auto l = [](const IndexMap& indices, const types::assoc_range& ranges,
                const tile_type& lhs, const tile_type& rhs) {
        return block_kernel(indices, ranges, lhs, rhs);
    };

    SECTION("Single block") {
        TA::TiledRange1 tr1{0, 2};

        SECTION("vector-vector") {
            SECTION("No summed indices") {
		using vector_il = TA::detail::vector_il<double>;
                IndexMap im("i", "i", "i");
                range_t ranges{{"i", tr1}};
                TA::TSpArrayD lhs(world, vector_il{1.0, 2.0});
                TA::TSpArrayD rhs(world, vector_il{3.0, 4.0});
                TA::TSpArrayD corr(world, vector_il{3.0, 8.0});
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }

            SECTION("Summed indices") {
		using vector_il = TA::detail::vector_il<double>;
                IndexMap im("i", "i", "j");
                range_t ranges{{"i", tr1}, {"j", tr1}};
                TA::TSpArrayD lhs(world, vector_il{1.0, 2.0});
                TA::TSpArrayD rhs(world, vector_il{3.0, 4.0});
                TA::TSpArrayD corr(world, vector_il{7.0, 14.0});
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }
        }

        SECTION("vector-matrix") {
            range_t ranges{{"i", tr1}, {"j", tr1}};
            SECTION("No summed indices") {
		using vector_il = TA::detail::vector_il<double>;
		using matrix_il = TA::detail::matrix_il<double>;
                IndexMap im("i,j", "i", "i,j");
                TA::TSpArrayD lhs(world, vector_il{1.0, 2.0});
                TA::TSpArrayD rhs(world, matrix_il{vector_il{3.0, 4.0}, vector_il{5.0, 6.0}});
                TA::TSpArrayD corr(world, matrix_il{vector_il{3.0, 4.0}, vector_il{10.0, 12.0}});
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }

            SECTION("Summed indices") {
		using vector_il = TA::detail::vector_il<double>;
		using matrix_il = TA::detail::matrix_il<double>;
                IndexMap im("i", "j", "j, i");
                TA::TSpArrayD lhs(world, vector_il{1.0, 2.0});
                TA::TSpArrayD rhs(world, matrix_il{vector_il{3.0, 4.0}, vector_il{5.0, 6.0}});
                TA::TSpArrayD corr(world, vector_il{13.0, 16.0});
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }
        }
    }

    SECTION("Multiple blocks") {
        TA::TiledRange1 tr1{0, 1, 2};
        TA::TiledRange v_trange{tr1};
        TA::TiledRange m_trange{tr1, tr1};
        TA::TiledRange t_trange{tr1, tr1, tr1};

        SECTION("vector-vector") {
            SECTION("No summed indices") {
                IndexMap im("i", "i", "i");
                range_t ranges{{"i", tr1}};
                TA::TSpArrayD lhs(world, v_trange);
                lhs.fill(2);
                TA::TSpArrayD rhs(world, v_trange);
                rhs.fill(3);
                TA::TSpArrayD corr(world, v_trange);
                corr.fill(6);
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }

            SECTION("Summed indices") {
                IndexMap im("i", "i", "j");
                range_t ranges{{"i", tr1}, {"j", tr1}};
                TA::TSpArrayD lhs(world, v_trange);
                lhs.fill(2);
                TA::TSpArrayD rhs(world, v_trange);
                rhs.fill(3);
                TA::TSpArrayD corr(world, v_trange);
                corr.fill(12);
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }
        }

        SECTION("vector-matrix") {
            range_t ranges{{"i", tr1}, {"j", tr1}};

            SECTION("No summed indices") {
                IndexMap im("i,j", "i", "i,j");
                TA::TSpArrayD lhs(world, v_trange);
                lhs.fill(2);
                TA::TSpArrayD rhs(world, m_trange);
                rhs.fill(3);
                TA::TSpArrayD corr(world, m_trange);
                corr.fill(6);
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }

            SECTION("Summed indices") {
                IndexMap im("j", "i", "i,j");
                TA::TSpArrayD lhs(world, v_trange);
                lhs.fill(2);
                TA::TSpArrayD rhs(world, m_trange);
                rhs.fill(3);
                TA::TSpArrayD corr(world, v_trange);
                corr.fill(12);
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }
        }

        SECTION("matrix-tensor3") {
            range_t ranges{{"i", tr1}, {"j", tr1}, {"k", tr1}};

            SECTION("No summed indices") {
                IndexMap im("i,j,k", "i,j", "i,j,k");
                TA::TSpArrayD lhs(world, m_trange);
                lhs.fill(2);
                TA::TSpArrayD rhs(world, t_trange);
                rhs.fill(3);
                TA::TSpArrayD corr(world, t_trange);
                corr.fill(6);
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }

            SECTION("Summed indices") {
                IndexMap im("i,j", "i,k", "i,k,j");
                TA::TSpArrayD lhs(world, m_trange);
                lhs.fill(2);
                TA::TSpArrayD rhs(world, t_trange);
                rhs.fill(3);
                TA::TSpArrayD corr(world, m_trange);
                corr.fill(12);
                auto result = tensor_kernel(im, ranges, lhs, rhs, l);
                REQUIRE(libchemist::ta_helpers::allclose(result, corr));
            }
        }
    }
}

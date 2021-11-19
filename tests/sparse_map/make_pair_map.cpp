#include "test_sparse_map.hpp"

using namespace chemist::sparse_map;

TEMPLATE_LIST_TEST_CASE("make_pair_map", "", testing::index_pairs_tuple_t) {
    using ind_idx_type    = std::tuple_element_t<0, TestType>;
    using dep_idx_type    = std::tuple_element_t<1, TestType>;
    using ind_dep_sm_type = SparseMap<ind_idx_type, dep_idx_type>;
    using ind_ind_sm_type = SparseMap<ind_idx_type, ind_idx_type>;

    // Make indices we'll need
    const auto& [i0, i1, i2, i3, i4] = testing::make_indices<ind_idx_type, 1>();
    const auto& [j0, j1, j2, j3, j4] = testing::make_indices<dep_idx_type, 1>();
    const auto& [i00, i01, i10, i11] = testing::make_indices<ind_idx_type, 2>();

    ind_idx_type i22{i2[0], i2[0]}, i23{i2[0], i3[0]}, i32{i3[0], i2[0]};
    ind_idx_type i33{i3[0], i3[0]};

    SECTION("Normal usage") {
        // Make sparse maps
        ind_dep_sm_type L_ia{{i0, {j1}}, {i1, {j2}}, {i2, {j3}}, {i3, {j4}}};
        ind_ind_sm_type L_ij{
          {i0, {i0, i1}}, {i1, {i0, i1}}, {i2, {i2, i3}}, {i3, {i2, i3}}};

        // Correct answer
        ind_dep_sm_type corr{{i00, {j1}},     {i01, {j1, j2}}, {i10, {j1, j2}},
                             {i11, {j2}},     {i22, {j3}},     {i23, {j3, j4}},
                             {i32, {j3, j4}}, {i33, {j4}}};

        auto rv = make_pair_map(L_ia, L_ij);
        REQUIRE(rv == corr);
    }

    SECTION("Throws if i  in L_ij is not in L_ia") {
        ind_dep_sm_type L_ia{{i1, {j1}}};
        ind_ind_sm_type L_ij{{i0, {i1}}};
        REQUIRE_THROWS_AS(make_pair_map(L_ia, L_ij), std::out_of_range);
    }

    SECTION("Throws if j in L_ij is not in L_ia") {
        ind_dep_sm_type L_ia{{i1, {j1}}};
        ind_ind_sm_type L_ij{{i1, {i0}}};
        REQUIRE_THROWS_AS(make_pair_map(L_ia, L_ij), std::out_of_range);
    }

    SECTION("Throws if independent rank of L_ij != independent rank of L_ia") {
        ind_dep_sm_type L_ia{{i1, {j1}}};
        ind_ind_sm_type L_ij{{i01, {i1}}};
        REQUIRE_THROWS_AS(make_pair_map(L_ia, L_ij), std::runtime_error);
    }

    SECTION("Throws if dependent rank of L_ij != independent rank of L_ia") {
        ind_dep_sm_type L_ia{{i1, {j1}}};
        ind_ind_sm_type L_ij{{i1, {i01}}};
        REQUIRE_THROWS_AS(make_pair_map(L_ia, L_ij), std::runtime_error);
    }

    SECTION("Throws if independent rank of L_ia != 1") {
        ind_dep_sm_type L_ia{{i01, {j1}}};
        ind_ind_sm_type L_ij{{i01, {i01}}};
        REQUIRE_THROWS_AS(make_pair_map(L_ia, L_ij), std::runtime_error);
    }
}

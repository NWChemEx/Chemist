#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/get_block_idx.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist::ta_helpers;

TEST_CASE("get_block_idx"){
    auto& world = TA::get_default_world();
    SECTION("Vector"){
        TA::TiledRange trange{{0, 1, 2}};
        TA::TSpArrayD t(world, trange, {0, 1});
        for(std::size_t i = 0; i < 2; ++i){
            auto idx = get_block_idx(t, t.find(i).get());
            decltype(idx) corr;
            corr.push_back(i);
            REQUIRE(idx == corr);
        }
    }

    SECTION("Matrix"){
        TA::TiledRange trange{{0, 1, 2}, {0, 1, 2}};
        TA::TSpArrayD t(world, trange, {{0, 1}, {2, 3}});
        for(std::size_t i = 0; i < 2; ++i){
            for(std::size_t j = 0; j < 2; ++j){
                auto idx = get_block_idx(t, t.find({i,j}).get());
                decltype(idx) corr{i, j};
                REQUIRE(idx == corr);
            }
        }
    }

    SECTION("Tensor"){
        TA::TiledRange trange{{0, 1, 2}, {0, 1, 2}, {0, 1, 2}};
        TA::TSpArrayD t(world, trange, {{{0, 1}, {2, 3}},
                                        {{4, 5}, {6, 7}}});
        for(std::size_t i = 0; i < 2; ++i){
            for(std::size_t j = 0; j < 2; ++j){
                for(std::size_t k = 0; k < 2; ++k) {
                    auto idx = get_block_idx(t, t.find({i, j, k}).get());
                    decltype(idx) corr{i, j, k};
                    REQUIRE(idx == corr);
                }
            }
        }
    }
}

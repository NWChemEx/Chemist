#include "libchemist/ta_helpers/slice.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;
using scalar_types = std::tuple<float, double>;

TEST_CASE("compare_lo_hi_") {
    compare_lo_hi_()
}

TEMPLATE_LIST_TEST_CASE("slice", "", scalar_types) {
    using tensor_type = TA::TSpArray<TestType>;
    using idx_type    = sparse_map::ElementIndex;
    auto& world       = TA::get_default_world();

    TA::detail::vector_il<TestType> vil{1, 2, 3, 4, 5, 6};

    std::map<std::string, tensor_type> vs;
    vs["one tile vector"] = tensor_type(world, vil);
    vs["two tile vector"] = tensor_type(world, TA::TiledRange{{0, 3, 6}}, vil);

    SECTION("Slice is entire tensor") {
        for(const auto& [k, t] : vs) {
            tensor_type r;
            SECTION(k) {
                r("i") = slice(t, idx_type{0}, idx_type{6});
                REQUIRE(allclose(r, t));
            }
        }
    }

    SECTION("Slice is on tile boundaries") {
        SECTION("two tile vector") {
            const auto& t = vs.at("two tile vector");
            tensor_type r, corr;

            SECTION("Block 0") {
                r("i")    = slice(t, idx_type{0}, idx_type{3});
                corr("i") = t("i").block({0}, {1});
                REQUIRE(allclose(r, t));
            }

            SECTION("Block 1") {
                r("i")    = slice(t, idx_type{3}, idx_type{6});
                corr("i") = t("i").block({1}, {2});
                REQUIRE(allclose(r, t));
            }
        }
    }
}

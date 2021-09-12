#include "libchemist/set_theory/family_of_sets_traits.hpp"
#include "libchemist/set_theory/subset.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::set_theory;

TEST_CASE("FamilyOfSetsTraits<Tuple>") {
    using set0_t      = std::vector<std::string>;
    using set1_t      = std::vector<std::string>;
    using traits_type = FamilyOfSetsTraits<std::tuple<set0_t, set1_t>>;

    using parent_set_type = typename traits_type::parent_set_type;
    using value_type      = typename traits_type::value_type;
    using ptr_type        = typename traits_type::ptr_type;

    SECTION("ptr_type") {
        using ptr0_t = std::shared_ptr<const set0_t>;
        using ptr1_t = std::shared_ptr<const set1_t>;
        using corr   = std::tuple<ptr0_t, ptr1_t>;
    }

    set0_t set0{"0.0", "1.0", "2.0", "3.0", "4.0", "5.0"};
    set1_t set1{"a", "b", "c", "d"};
    parent_set_type sets{set0, set1};

    auto ptr = traits_type::make_pointer(sets);

    SECTION("make_pointer") {
        REQUIRE(*std::get<0>(ptr) == set0);
        REQUIRE(*std::get<1>(ptr) == set1);
    }

    SECTION("new_subset") {
        auto s = traits_type::new_subset(ptr, {{0, 2}, {1, 3}});
        Subset<set0_t> corr0(std::get<0>(ptr), {0, 2});
        Subset<set1_t> corr1(std::get<1>(ptr), {1, 3});
        value_type corr{corr0, corr1};
        REQUIRE(s == corr);
    }
}
#include "../test_set_theory.hpp"
#include "chemist/set_theory/subset.hpp"
#include "chemist/set_theory/traits/family_of_sets_tuple.hpp"
#include <catch2/catch.hpp>
#include <sstream>

using namespace chemist::set_theory;

TEST_CASE("FamilyOfSetsTraits<Tuple>") {
    using set0_t      = std::tuple_element_t<0, container_types>;
    using set1_t      = std::tuple_element_t<1, container_types>;
    using traits_type = FamilyOfSetsTraits<std::tuple<set0_t, set1_t>>;

    using parent_set_type = typename traits_type::parent_set_type;
    using value_type      = typename traits_type::value_type;
    using ptr_type        = typename traits_type::ptr_type;
    using subset_il_type  = typename traits_type::subset_il_type;

    SECTION("Typedefs") {
        SECTION("parent_set_type") {
            using corr = std::tuple<set0_t, set1_t>;
            STATIC_REQUIRE(std::is_same_v<parent_set_type, corr>);
        }

        SECTION("const_parent_reference") {
            using corr = std::tuple<std::reference_wrapper<const set0_t>,
                                    std::reference_wrapper<const set1_t>>;
            using t    = typename traits_type::const_parent_reference;
            STATIC_REQUIRE(std::is_same_v<corr, t>);
        }

        SECTION("value_type") {
            using corr = std::tuple<Subset<set0_t>, Subset<set1_t>>;
            STATIC_REQUIRE(std::is_same_v<corr, value_type>);
        }

        SECTION("reference_type") {
            using corr           = value_type&;
            using reference_type = typename traits_type::reference_type;
            STATIC_REQUIRE(std::is_same_v<corr, reference_type>);
        }

        SECTION("const_reference") {
            using corr            = const value_type&;
            using const_reference = typename traits_type::const_reference;
            STATIC_REQUIRE(std::is_same_v<corr, const_reference>);
        }

        SECTION("subset_il_type") {
            using corr = std::tuple<std::initializer_list<std::size_t>,
                                    std::initializer_list<std::size_t>>;
            using subset_il_type = typename traits_type::subset_il_type;
            STATIC_REQUIRE(std::is_same_v<corr, subset_il_type>);
        }

        SECTION("il_type") {
            using corr    = std::initializer_list<subset_il_type>;
            using il_type = typename traits_type::il_type;
            STATIC_REQUIRE(std::is_same_v<corr, il_type>);
        }

        SECTION("ptr_type") {
            using ptr0_t = std::shared_ptr<const set0_t>;
            using ptr1_t = std::shared_ptr<const set1_t>;
            using corr   = std::tuple<ptr0_t, ptr1_t>;
            STATIC_REQUIRE(std::is_same_v<corr, ptr_type>);
        }
    }
    auto set0 = testing::make_object<set0_t>();
    auto set1 = testing::make_object<set1_t>();

    parent_set_type sets{set0, set1};

    auto ptr = traits_type::make_pointer(sets);

    SECTION("make_pointer") {
        REQUIRE(*std::get<0>(ptr) == set0);
        REQUIRE(*std::get<1>(ptr) == set1);
    }

    SECTION("new_subset") {
        std::size_t zero{0}, two{2};
        auto s = traits_type::new_subset(ptr, {{zero, two}, {1, 2}});
        Subset<set0_t> corr0(std::get<0>(ptr), {zero, two});
        Subset<set1_t> corr1(std::get<1>(ptr), {1, 2});
        value_type corr{corr0, corr1};
        REQUIRE(s == corr);
    }

    SECTION("dereference_object") {
        REQUIRE(std::get<0>(traits_type::dereference_object(ptr)) == set0);
        REQUIRE(std::get<1>(traits_type::dereference_object(ptr)) == set1);
    }

    SECTION("is_subset") {
        std::size_t zero{0}, two{2};
        auto s = traits_type::new_subset(ptr, {{zero, two}, {1, 2}});
        REQUIRE(traits_type::is_subset(s, ptr));

        auto other = traits_type::make_pointer(parent_set_type{});
        REQUIRE_FALSE(traits_type::is_subset(s, other));
    }

    SECTION("disjoint") {
        std::size_t zero{0}, two{2};
        auto s  = traits_type::new_subset(ptr, {{zero, two}, {1, 2}});
        auto s1 = traits_type::new_subset(ptr);

        REQUIRE(traits_type::disjoint(s, s1));
        REQUIRE_FALSE(traits_type::disjoint(s, s));
    }

    SECTION("print_elem") {
        SECTION("Non-empty") {
            std::size_t zero{0}, two{2};
            auto s = traits_type::new_subset(ptr, {{zero, two}, {1, 2}});
            std::stringstream ss;
            traits_type::print_elem(ss, s);
            REQUIRE(ss.str() == "({0 2 },{1 2 })");
        }
        SECTION("Empty") {
            auto s1 = traits_type::new_subset(ptr);
            std::stringstream ss;
            traits_type::print_elem(ss, s1);
            REQUIRE(ss.str() == "({},{})");
        }
    }
}
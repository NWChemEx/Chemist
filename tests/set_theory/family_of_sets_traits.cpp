#include "libchemist/set_theory/family_of_sets_traits.hpp"
#include "libchemist/set_theory/subset.hpp"
#include "test_set_theory.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::set_theory;

TEMPLATE_LIST_TEST_CASE("FilyOfSetsTraits<single container>", "",
                        container_types) {
    using parent_type = TestType;
    using traits_type = FamilyOfSetsTraits<parent_type>;
    using value_type  = typename traits_type::value_type;
    using size_type   = typename SetTraits<parent_type>::size_type;

    SECTION("Typedefs") {
        SECTION("value_type") {
            using corr = Subset<parent_type>;
            STATIC_REQUIRE(std::is_same_v<corr, value_type>);
        }

        SECTION("reference_type") {
            using corr           = value_type&;
            using reference_type = typename traits_type::reference_type;
            STATIC_REQUIRE(std::is_same_v<reference_type, corr>);
        }

        SECTION("const_reference") {
            using corr            = const value_type&;
            using const_reference = typename traits_type::const_reference;
            STATIC_REQUIRE(std::is_same_v<const_reference, corr>);
        }

        SECTION("il_type") {
            using corr =
              std::initializer_list<std::initializer_list<size_type>>;
            using il_type = typename traits_type::il_type;
            STATIC_REQUIRE(std::is_same_v<corr, il_type>);
        }

        SECTION("ptr_type") {
            using corr     = std::shared_ptr<const parent_type>;
            using ptr_type = typename traits_type::ptr_type;
            STATIC_REQUIRE(std::is_same_v<corr, ptr_type>);
        }
    }

    SECTION("make_pointer") {
        auto obj = testing::make_object<parent_type>();
        auto ptr = traits_type::make_pointer(obj);
        REQUIRE(*ptr == obj);
    }

    SECTION("new_subset") {
        auto obj = testing::make_object<parent_type>();
        auto ptr = traits_type::make_pointer(obj);

        SECTION("Default") {
            auto subset = traits_type::new_subset(ptr);
            auto corr   = value_type(ptr);
            REQUIRE(corr == subset);
        }

        SECTION("Non-empty") {
            size_type zero{0};
            auto subset = traits_type::new_subset(ptr, {zero});
            auto corr   = value_type(ptr, {zero});
            REQUIRE(corr == subset);
        }
    }
}

TEST_CASE("FamilyOfSetsTraits<Tuple>") {
    using set0_t      = std::tuple_element_t<0, container_types>;
    using set1_t      = std::tuple_element_t<1, container_types>;
    using traits_type = FamilyOfSetsTraits<std::tuple<set0_t, set1_t>>;

    using parent_set_type = typename traits_type::parent_set_type;
    using value_type      = typename traits_type::value_type;
    using ptr_type        = typename traits_type::ptr_type;

    SECTION("Typedefs") {
        SECTION("ptr_type") {
            using ptr0_t = std::shared_ptr<const set0_t>;
            using ptr1_t = std::shared_ptr<const set1_t>;
            using corr   = std::tuple<ptr0_t, ptr1_t>;
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
}
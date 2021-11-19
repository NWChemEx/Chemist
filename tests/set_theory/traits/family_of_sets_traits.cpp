#include "../test_set_theory.hpp"
#include "chemist/set_theory/subset.hpp"
#include "chemist/set_theory/traits/family_of_sets.hpp"
#include <catch2/catch.hpp>

using namespace chemist::set_theory;

TEMPLATE_LIST_TEST_CASE("FamilyOfSetsTraits<single container>", "",
                        container_types) {
    using parent_type    = TestType;
    using traits_type    = FamilyOfSetsTraits<parent_type>;
    using value_type     = typename traits_type::value_type;
    using size_type      = typename SetTraits<parent_type>::size_type;
    using subset_il_type = typename traits_type::subset_il_type;

    SECTION("Typedefs") {
        SECTION("parent_type") {
            using t = typename traits_type::parent_type;
            STATIC_REQUIRE(std::is_same_v<t, parent_type>);
        }

        SECTION("const_parent_reference") {
            using corr = const parent_type&;
            using t    = typename traits_type::const_parent_reference;
            STATIC_REQUIRE(std::is_same_v<corr, t>);
        }

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

        SECTION("subset_il_type") {
            using corr = std::initializer_list<std::size_t>;
            STATIC_REQUIRE(std::is_same_v<corr, subset_il_type>);
        }

        SECTION("il_type") {
            using corr    = std::initializer_list<subset_il_type>;
            using il_type = typename traits_type::il_type;
            STATIC_REQUIRE(std::is_same_v<corr, il_type>);
        }

        SECTION("ptr_type") {
            using corr     = std::shared_ptr<const parent_type>;
            using ptr_type = typename traits_type::ptr_type;
            STATIC_REQUIRE(std::is_same_v<corr, ptr_type>);
        }
    }

    auto obj = testing::make_object<parent_type>();
    auto ptr = traits_type::make_pointer(obj);

    SECTION("make_pointer") { REQUIRE(*ptr == obj); }

    SECTION("new_subset") {
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

    SECTION("dereference_object") {
        REQUIRE(traits_type::dereference_object(ptr) == obj);
    }

    SECTION("is_subset") {
        SECTION("Is subset") {
            REQUIRE(traits_type::is_subset(traits_type::new_subset(ptr), ptr));
        }

        SECTION("Not a subset") {
            decltype(obj) other;
            auto other_ptr    = traits_type::make_pointer(other);
            auto other_subset = traits_type::new_subset(other_ptr);
            REQUIRE_FALSE(traits_type::is_subset(other_subset, ptr));
        }
    }

    SECTION("disjoint") {
        auto e0 = traits_type::new_subset(ptr, {size_type{0}});

        SECTION("Are disjoint") {
            auto e1 = traits_type::new_subset(ptr, {size_type{1}});
            REQUIRE(traits_type::disjoint(e0, e1));
        }

        SECTION("Intersect") {
            auto e1 = traits_type::new_subset(ptr, {size_type{0}});
            REQUIRE_FALSE(traits_type::disjoint(e0, e1));
        }
    }

    SECTION("print_elem") {
        SECTION("Non-empty") {
            auto s1 = traits_type::new_subset(ptr, {size_type{0}});
            std::stringstream ss;
            traits_type::print_elem(ss, s1);
            REQUIRE(ss.str() == "{0 }");
        }
        SECTION("Empty") {
            auto s1 = traits_type::new_subset(ptr, {});
            std::stringstream ss;
            traits_type::print_elem(ss, s1);
            REQUIRE(ss.str() == "{}");
        }
    }
}

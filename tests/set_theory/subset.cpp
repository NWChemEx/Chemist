#include "libchemist/set_theory/family_of_sets.hpp"
#include "libchemist/set_theory/subset.hpp"
#include "test_set_theory.hpp"

TEMPLATE_LIST_TEST_CASE("Subset", "", container_types) {
    // Work out some types we'll need for the unit tests
    using subset_type = Subset<TestType>;
    using family_type = typename subset_type::parent_type;

    SECTION("Typedefs") {
        SECTION("parent_type") {
            using corr = FamilyOfSets<TestType>;
            STATIC_REQUIRE(std::is_same_v<family_type, corr>);
        }
        SECTION("value_type") {
            using value_type = typename subset_type::value_type;
            using corr       = typename TestType::value_type;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }
        SECTION("const_reference") {
            using const_reference = typename subset_type::const_reference;
            using corr            = const typename TestType::value_type&;
            STATIC_REQUIRE(std::is_same_v<const_reference, corr>);
        }
        SECTION("size_type") {
            using size_type = typename subset_type::size_type;
            using corr      = typename TestType::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
        SECTION("object_type") {
            using object_type = typename subset_type::object_type;
            using corr        = TestType;
            STATIC_REQUIRE(std::is_same_v<object_type, corr>);
        }
        SECTION("const_obj_ref") {
            using const_obj_ref = typename subset_type::const_obj_ref;
            using corr          = const TestType&;
            STATIC_REQUIRE(std::is_same_v<const_obj_ref, corr>);
        }
    }

    // Make a default/empty and non-default parent set
    TestType default_obj;
    auto non_default_obj = testing::make_object<TestType>();

    // Make family of sets around the parent sets
    family_type non_default(non_default_obj);
    family_type defaulted(default_obj);

    // Only possible subset for an empty parent set
    subset_type empty_defaulted(defaulted);

    // Make some subsets for the non-default parent set
    subset_type empty_non_defaulted(non_default);
    subset_type e0(non_default, {0ul});
    subset_type e2(non_default, {2ul});
    subset_type e01(non_default, {0ul, 1ul});

    SECTION("CTors/Assignment") {
        SECTION("Initializer list cTor") {
            SECTION("Default il") {
                REQUIRE(empty_defaulted.empty());
                REQUIRE(empty_defaulted.size() == 0);
                REQUIRE(empty_defaulted.object() == default_obj);

                REQUIRE(empty_non_defaulted.empty());
                REQUIRE(empty_non_defaulted.size() == 0);
                REQUIRE(empty_non_defaulted.object() == non_default_obj);
            }

            SECTION("Non-default il") {
                REQUIRE_FALSE(e0.empty());
                REQUIRE(e0.size() == 1);
                REQUIRE(e0[0] == non_default_obj[0]);
                REQUIRE(e0.object() == non_default_obj);

                REQUIRE_FALSE(e2.empty());
                REQUIRE(e2.size() == 1);
                REQUIRE(e2[0] == non_default_obj[2]);
                REQUIRE(e2.object() == non_default_obj);

                REQUIRE_FALSE(e01.empty());
                REQUIRE(e01.size() == 2);
                REQUIRE(e01[0] == non_default_obj[0]);
                REQUIRE(e01[1] == non_default_obj[1]);
                REQUIRE(e01.object() == non_default_obj);

                SECTION("Throws if invalid index") {
                    REQUIRE_THROWS_AS(subset_type(non_default, {3ul}),
                                      std::out_of_range);
                }
            }

            SECTION("Copy CTor") {
                subset_type copy(e0);
                REQUIRE(copy.size() == 1);
                REQUIRE(copy[0] == non_default_obj[0]);
                REQUIRE(copy.object() == non_default_obj);
            }

            SECTION("Move CTor") {
                subset_type moved(std::move(e0));
                REQUIRE(moved.size() == 1);
                REQUIRE(moved[0] == non_default_obj[0]);
                REQUIRE(moved.object() == non_default_obj);
            }

            SECTION("Copy Assignment") {
                subset_type copy(defaulted);
                auto pcopy = &(copy = e0);
                REQUIRE(pcopy == &copy);
                REQUIRE(copy.size() == 1);
                REQUIRE(copy[0] == non_default_obj[0]);
                REQUIRE(copy.object() == non_default_obj);
            }

            SECTION("Move Assignment") {
                subset_type moved(defaulted);
                auto pmoved = &(moved = std::move(e0));
                REQUIRE(pmoved == &moved);
                REQUIRE(moved.size() == 1);
                REQUIRE(moved[0] == non_default_obj[0]);
                REQUIRE(moved.object() == non_default_obj);
            }
        }
    }

    SECTION("accessors") {
        SECTION("size()") {
            REQUIRE(empty_defaulted.size() == 0);
            REQUIRE(empty_non_defaulted.size() == 0);
            REQUIRE(e0.size() == 1);
            REQUIRE(e2.size() == 1);
            REQUIRE(e01.size() == 2);
        }

        SECTION("empty()") {
            REQUIRE(empty_defaulted.empty());
            REQUIRE(empty_non_defaulted.empty());
            REQUIRE_FALSE(e0.empty());
            REQUIRE_FALSE(e2.empty());
            REQUIRE_FALSE(e01.empty());
        }

        SECTION("at") {
            SECTION("Throws if index is out of bounds") {
                REQUIRE_THROWS_AS(empty_defaulted.at(0), std::out_of_range);
            }
            SECTION("Returns by reference") {
                REQUIRE(&(e0.at(0)) == &(non_default.object()[0]));
                REQUIRE(&(e2.at(0)) == &(non_default.object()[2]));
                REQUIRE(&(e01.at(0)) == &(non_default.object()[0]));
                REQUIRE(&(e01.at(1)) == &(non_default.object()[1]));
            }
        }

        SECTION("operator[]") {
            // Check addresses to ensure they're not copies
            REQUIRE(&(e0[0]) == &(non_default.object()[0]));
            REQUIRE(&(e2[0]) == &(non_default.object()[2]));
            REQUIRE(&(e01[0]) == &(non_default.object()[0]));
            REQUIRE(&(e01[1]) == &(non_default.object()[1]));
        }

        SECTION("count(elem_type)") {
            SECTION("Object is in subset (and parent set)") {
                REQUIRE(e0.count(non_default_obj[0]));
            }
            SECTION("Object is in parent set, but not in subset") {
                REQUIRE_FALSE(e0.count(non_default_obj[1]));
            }
            SECTION("Object is not in parent set and thus not in subset") {
                typename subset_type::value_type v;
                const auto& v_ref = v;
                REQUIRE_FALSE(e0.count(v_ref));
            }
        }

        SECTION("object") {
            REQUIRE(empty_defaulted.object() == default_obj);
            REQUIRE(empty_non_defaulted.object() == non_default_obj);
            REQUIRE(e0.object() == non_default_obj);
            REQUIRE(e2.object() == non_default_obj);
            REQUIRE(e01.object() == non_default_obj);
        }
    }

    SECTION("insert(size_type)") {
        SECTION("Throws if out of bounds") {
            using except_t = std::out_of_range;
            REQUIRE_THROWS_AS(empty_defaulted.insert(0), except_t);
        }

        SECTION("Add unique value") {
            empty_non_defaulted.insert(1ul);
            REQUIRE(empty_non_defaulted.size() == 1);
            REQUIRE(&(empty_non_defaulted[0]) == &(non_default.object()[1]));
        }

        SECTION("Adding duplicate value does nothing") {
            e0.insert(0ul);
            REQUIRE(e0.size() == 1);
            REQUIRE(&(e0[0]) == &(non_default.object()[0]));
        }
    }

    SECTION("insert(elem_type)") {
        auto& i0 = non_default.object()[0];
        SECTION("Throws if out of bounds") {
            using except_t = std::out_of_range;
            REQUIRE_THROWS_AS(empty_defaulted.insert(i0), except_t);
        }

        SECTION("Add unique value") {
            empty_non_defaulted.insert(non_default.object()[1]);
            REQUIRE(empty_non_defaulted.size() == 1);
            REQUIRE(&(empty_non_defaulted[0]) == &(non_default.object()[1]));
        }

        SECTION("Adding duplicate value does nothing") {
            e0.insert(i0);
            REQUIRE(e0.size() == 1);
            REQUIRE(&(e0[0]) == &(non_default.object()[0]));
        }
    }

    SECTION("operator+=") {
        SECTION("empty += filled") {
            auto p = &(empty_non_defaulted += e0);
            REQUIRE(p == &empty_non_defaulted);
            REQUIRE(empty_non_defaulted == e0);
        }
        SECTION("filled += empty") {
            subset_type corr(e0);
            auto p = &(e0 += empty_non_defaulted);
            REQUIRE(p == &e0);
            REQUIRE(e0 == corr);
        }
        SECTION("e0 += e0") {
            subset_type corr(e0);
            auto p = &(e0 += e0);
            REQUIRE(p == &e0);
            REQUIRE(e0 == corr);
        }
        SECTION("e0 += e2") {
            subset_type corr(non_default, {0ul, 2ul});
            auto p = &(e0 += e2);
            REQUIRE(e0 == corr);
        }
        SECTION("e2 += e0") {
            subset_type corr(non_default, {0ul, 2ul});
            auto p = &(e2 += e0);
            REQUIRE(e2 == corr);
        }
        SECTION("Throws if sets have different parent sets") {
            REQUIRE_THROWS_AS(empty_defaulted += e0, std::runtime_error);
        }
    }

    SECTION("operator+") {
        SECTION("empty + filled") {
            auto r = empty_non_defaulted + e0;
            REQUIRE(r == e0);
        }
        SECTION("filled + empty") {
            auto r = e0 + empty_non_defaulted;
            REQUIRE(r == e0);
        }
        SECTION("e0 + e0") {
            auto r = e0 + e0;
            REQUIRE(r == e0);
        }
        SECTION("e0 + e2") {
            subset_type corr(non_default, {0ul, 2ul});
            auto r = e0 + e2;
            REQUIRE(r == corr);
        }
        SECTION("e2 + e0") {
            subset_type corr(non_default, {0ul, 2ul});
            auto r = e2 + e0;
            REQUIRE(r == corr);
        }
        SECTION("Throws if sets have different parent sets") {
            REQUIRE_THROWS_AS(empty_defaulted + e0, std::runtime_error);
        }
    }

    SECTION("Comparisons") {
        SECTION("Different types") {
            using parent_type = std::vector<double>;
            FamilyOfSets<parent_type> f(parent_type{1, 2, 3});
            Subset<parent_type> f_subset(f);
            REQUIRE_FALSE(f_subset == empty_defaulted);
            REQUIRE(f_subset != empty_defaulted);
        }
        SECTION("Different parent objects") {
            REQUIRE_FALSE(empty_defaulted == empty_non_defaulted);
            REQUIRE(empty_defaulted != empty_non_defaulted);
        }
        SECTION("Different family of sets, but otherwise equal") {
            family_type f(non_default_obj);
            subset_type s(f);
            REQUIRE(s == empty_non_defaulted);
            REQUIRE_FALSE(s != empty_non_defaulted);
        }
        SECTION("Empty sets are equal") {
            SECTION("Empty parent set") {
                subset_type s(defaulted);
                REQUIRE(s == empty_defaulted);
                REQUIRE_FALSE(s != empty_defaulted);
            }

            SECTION("Non-empty parent set") {
                subset_type s(non_default);
                REQUIRE(s == empty_non_defaulted);
                REQUIRE_FALSE(s != empty_non_defaulted);
            }
        }
        SECTION("Different number of elements") {
            REQUIRE_FALSE(e0 == e01);
            REQUIRE(e0 != e01);
        }

        SECTION("Different elements") {
            REQUIRE_FALSE(e0 == e2);
            REQUIRE(e0 != e2);
        }

        SECTION("Same elements") {
            subset_type s(non_default, {0ul});
            REQUIRE(s == e0);
            REQUIRE_FALSE(s != e0);
        }
    }
}
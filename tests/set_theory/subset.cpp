#include "chemist/set_theory/family_of_sets.hpp"
#include "chemist/set_theory/subset.hpp"
#include "test_set_theory.hpp"

using namespace chemist::set_theory;

TEMPLATE_LIST_TEST_CASE("Subset", "", container_types) {
    // Work out some types we'll need for the unit tests
    using subset_type = Subset<TestType>;

    SECTION("Typedefs") {
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
        SECTION("object_ptr") {
            using object_ptr = typename subset_type::object_ptr;
            using corr       = std::shared_ptr<const TestType>;
            STATIC_REQUIRE(std::is_same_v<object_ptr, corr>);
        }
        SECTION("const_obj_ref") {
            using const_obj_ref = typename subset_type::const_obj_ref;
            using corr          = const TestType&;
            STATIC_REQUIRE(std::is_same_v<const_obj_ref, corr>);
        }
    }

    // Make a default/empty and non-default parent set
    auto default_ptr = std::make_shared<TestType>();
    auto non_default_ptr =
      std::make_shared<TestType>(testing::make_object<TestType>());

    auto& non_default = *non_default_ptr;
    // Only possible subset for an empty parent set
    subset_type empty_defaulted(default_ptr);

    // Make some subsets for the non-default parent set
    subset_type empty_non_defaulted(non_default_ptr);
    subset_type e0(non_default_ptr, {0ul});
    subset_type e2(non_default_ptr, {2ul});
    subset_type e01(non_default_ptr, {0ul, 1ul});

    SECTION("CTors/Assignment") {
        SECTION("Initializer list cTor") {
            SECTION("Default il") {
                REQUIRE(empty_defaulted.empty());
                REQUIRE(empty_defaulted.size() == 0);
                REQUIRE(empty_defaulted.object() == *default_ptr);

                REQUIRE(empty_non_defaulted.empty());
                REQUIRE(empty_non_defaulted.size() == 0);
                REQUIRE(empty_non_defaulted.object() == *non_default_ptr);
            }

            SECTION("Non-default il") {
                REQUIRE_FALSE(e0.empty());
                REQUIRE(e0.size() == 1);
                REQUIRE(e0[0] == (*non_default_ptr)[0]);
                REQUIRE(e0.object() == *non_default_ptr);

                REQUIRE_FALSE(e2.empty());
                REQUIRE(e2.size() == 1);
                REQUIRE(e2[0] == (*non_default_ptr)[2]);
                REQUIRE(e2.object() == *non_default_ptr);

                REQUIRE_FALSE(e01.empty());
                REQUIRE(e01.size() == 2);
                REQUIRE(e01[0] == (*non_default_ptr)[0]);
                REQUIRE(e01[1] == (*non_default_ptr)[1]);
                REQUIRE(e01.object() == *non_default_ptr);

                SECTION("Throws if invalid index") {
                    REQUIRE_THROWS_AS(subset_type(non_default_ptr, {3ul}),
                                      std::out_of_range);
                }

                SECTION("Throws if pointer is null") {
                    typename subset_type::object_ptr null_ptr;
                    using except_t = std::runtime_error;
                    REQUIRE_THROWS_AS(subset_type(null_ptr, {0ul}), except_t);
                }
            }

            SECTION("Range Ctor") {
                SECTION("Insert by index") {
                    std::vector elems{0ul, 2ul};
                    subset_type e02(non_default_ptr, elems.begin(),
                                    elems.end());
                    REQUIRE(e02.size() == 2);
                    REQUIRE(e02[0] == (*non_default_ptr)[0]);
                    REQUIRE(e02[1] == (*non_default_ptr)[2]);
                }
                SECTION("Insert by value") {
                    std::vector elems{(*non_default_ptr)[0],
                                      (*non_default_ptr)[2]};
                    subset_type e02(non_default_ptr, elems.begin(),
                                    elems.end());
                    REQUIRE(e02.size() == 2);
                    REQUIRE(e02[0] == (*non_default_ptr)[0]);
                    REQUIRE(e02[1] == (*non_default_ptr)[2]);
                }

                SECTION("Throws if pointer is null") {
                    typename subset_type::object_ptr null_ptr;
                    using except_t = std::runtime_error;
                    REQUIRE_THROWS_AS(subset_type(null_ptr, {0ul}), except_t);
                }

                SECTION("Throws if invalid index") {
                    std::vector<unsigned long> idxs{3ul};
                    REQUIRE_THROWS_AS(
                      subset_type(non_default_ptr, idxs.begin(), idxs.end()),
                      std::out_of_range);
                }
            }

            SECTION("Copy CTor") {
                subset_type copy(e0);
                REQUIRE(copy.size() == 1);
                REQUIRE(copy[0] == (*non_default_ptr)[0]);
                REQUIRE(copy.object() == *non_default_ptr);
            }

            SECTION("Move CTor") {
                subset_type moved(std::move(e0));
                REQUIRE(moved.size() == 1);
                REQUIRE(moved[0] == (*non_default_ptr)[0]);
                REQUIRE(moved.object() == *non_default_ptr);
            }

            SECTION("Copy Assignment") {
                subset_type copy(empty_defaulted);
                auto pcopy = &(copy = e0);
                REQUIRE(pcopy == &copy);
                REQUIRE(copy.size() == 1);
                REQUIRE(copy[0] == (*non_default_ptr)[0]);
                REQUIRE(copy.object() == *non_default_ptr);
            }

            SECTION("Move Assignment") {
                subset_type moved(empty_defaulted);
                auto pmoved = &(moved = std::move(e0));
                REQUIRE(pmoved == &moved);
                REQUIRE(moved.size() == 1);
                REQUIRE(moved[0] == (*non_default_ptr)[0]);
                REQUIRE(moved.object() == *non_default_ptr);
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
                REQUIRE(&(e0.at(0)) == &(non_default[0]));
                REQUIRE(&(e2.at(0)) == &(non_default[2]));
                REQUIRE(&(e01.at(0)) == &(non_default[0]));
                REQUIRE(&(e01.at(1)) == &(non_default[1]));
            }
        }

        SECTION("operator[]") {
            // Check addresses to ensure they're not copies
            REQUIRE(&(e0[0]) == &(non_default[0]));
            REQUIRE(&(e2[0]) == &(non_default[2]));
            REQUIRE(&(e01[0]) == &(non_default[0]));
            REQUIRE(&(e01[1]) == &(non_default[1]));
        }

        SECTION("count(elem_type)") {
            SECTION("Object is in subset (and parent set)") {
                REQUIRE(e0.count((*non_default_ptr)[0]));
            }
            SECTION("Object is in parent set, but not in subset") {
                REQUIRE_FALSE(e0.count((*non_default_ptr)[1]));
            }
            SECTION("Object is not in parent set and thus not in subset") {
                typename subset_type::value_type v;
                const auto& v_ref = v;
                REQUIRE_FALSE(e0.count(v_ref));
            }
        }

        SECTION("object") {
            REQUIRE(empty_defaulted.object() == *default_ptr);
            REQUIRE(empty_non_defaulted.object() == *non_default_ptr);
            REQUIRE(e0.object() == *non_default_ptr);
            REQUIRE(e2.object() == *non_default_ptr);
            REQUIRE(e01.object() == *non_default_ptr);
        }
    }

    SECTION("iterators") {
        SECTION("Empty container") {
            REQUIRE(empty_non_defaulted.begin() == empty_non_defaulted.end());
        }
        SECTION("Single element") {
            auto b = e0.begin();
            REQUIRE(b != e0.end());
            REQUIRE(*b == 0ul);
            ++b;
            REQUIRE(b == e0.end());
        }
        SECTION("Two elemnts") {
            auto b = e01.begin();
            REQUIRE(b != e01.end());
            REQUIRE(*b == 0ul);
            ++b;
            REQUIRE(*b == 1ul);
            ++b;
            REQUIRE(b == e01.end());
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
            REQUIRE(&(empty_non_defaulted[0]) == &(non_default[1]));
        }

        SECTION("Adding duplicate value does nothing") {
            e0.insert(0ul);
            REQUIRE(e0.size() == 1);
            REQUIRE(&(e0[0]) == &(non_default[0]));
        }
    }

    SECTION("insert(elem_type)") {
        auto& i0 = non_default[0];
        SECTION("Throws if out of bounds") {
            using except_t = std::out_of_range;
            REQUIRE_THROWS_AS(empty_defaulted.insert(i0), except_t);
        }

        SECTION("Add unique value") {
            empty_non_defaulted.insert(non_default[1]);
            REQUIRE(empty_non_defaulted.size() == 1);
            REQUIRE(&(empty_non_defaulted[0]) == &(non_default[1]));
        }

        SECTION("Adding duplicate value does nothing") {
            e0.insert(i0);
            REQUIRE(e0.size() == 1);
            REQUIRE(&(e0[0]) == &(non_default[0]));
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
            subset_type corr(non_default_ptr, {0ul, 2ul});
            auto p = &(e0 += e2);
            REQUIRE(p == &e0);
            REQUIRE(e0 == corr);
        }
        SECTION("e2 += e0") {
            subset_type corr(non_default_ptr, {0ul, 2ul});
            auto p = &(e2 += e0);
            REQUIRE(p == &e2);
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
            subset_type corr(non_default_ptr, {0ul, 2ul});
            auto r = e0 + e2;
            REQUIRE(r == corr);
        }
        SECTION("e2 + e0") {
            subset_type corr(non_default_ptr, {0ul, 2ul});
            auto r = e2 + e0;
            REQUIRE(r == corr);
        }
        SECTION("Throws if sets have different parent sets") {
            REQUIRE_THROWS_AS(empty_defaulted + e0, std::runtime_error);
        }
    }

    SECTION("operator-=") {
        SECTION("empty -= filled") {
            auto p = &(empty_non_defaulted -= e0);
            REQUIRE(p == &empty_non_defaulted);
            REQUIRE(empty_non_defaulted == subset_type{non_default_ptr});
        }
        SECTION("filled -= empty") {
            subset_type corr(e0);
            auto p = &(e0 -= empty_non_defaulted);
            REQUIRE(p == &e0);
            REQUIRE(e0 == corr);
        }
        SECTION("e0 -= e0") {
            auto p = &(e0 -= e0);
            REQUIRE(p == &e0);
            REQUIRE(e0 == empty_non_defaulted);
        }
        SECTION("e0 -= e2") {
            subset_type corr(non_default_ptr, {0ul});
            auto p = &(e0 -= e2);
            REQUIRE(p == &e0);
            REQUIRE(e0 == corr);
        }
        SECTION("e01 -= e0") {
            subset_type corr(non_default_ptr, {1ul});
            auto p = &(e01 -= e0);
            REQUIRE(p == &e01);
            REQUIRE(e01 == corr);
        }
        SECTION("Throws if sets have different parent sets") {
            REQUIRE_THROWS_AS(empty_defaulted -= e0, std::runtime_error);
        }
    }

    SECTION("operator-") {
        SECTION("empty - filled") {
            auto r = empty_non_defaulted - e0;
            REQUIRE(empty_non_defaulted == r);
        }
        SECTION("filled - empty") {
            auto r = e0 - empty_non_defaulted;
            REQUIRE(e0 == r);
        }
        SECTION("e0 - e0") {
            auto r = e0 - e0;
            REQUIRE(r == empty_non_defaulted);
        }
        SECTION("e0 - e2") {
            auto r = e0 - e2;
            REQUIRE(e0 == r);
        }
        SECTION("e01 - e0") {
            subset_type corr(non_default_ptr, {1ul});
            auto r = e01 - e0;
            REQUIRE(r == corr);
        }
        SECTION("Throws if sets have different parent sets") {
            REQUIRE_THROWS_AS(empty_defaulted - e0, std::runtime_error);
        }
    }

    SECTION("operator^=") {
        SECTION("empty ^= filled") {
            auto p = &(empty_non_defaulted ^= e0);
            REQUIRE(p == &empty_non_defaulted);
            REQUIRE(empty_non_defaulted == subset_type{non_default_ptr});
        }
        SECTION("filled ^= empty") {
            auto p = &(e0 ^= empty_non_defaulted);
            REQUIRE(p == &e0);
            REQUIRE(e0 == empty_non_defaulted);
        }
        SECTION("e0 ^= e0") {
            subset_type corr(e0);
            auto p = &(e0 ^= e0);
            REQUIRE(p == &e0);
            REQUIRE(e0 == corr);
        }
        SECTION("e0 ^= e2") {
            auto p = &(e0 ^= e2);
            REQUIRE(p == &e0);
            REQUIRE(e0 == empty_non_defaulted);
        }
        SECTION("e01 ^= e0") {
            auto p = &(e01 ^= e0);
            REQUIRE(p == &e01);
            REQUIRE(e01 == e0);
        }
        SECTION("e0 ^= e01") {
            subset_type corr(e0);
            auto p = &(e0 ^= e01);
            REQUIRE(p == &e0);
            REQUIRE(e0 == corr);
        }
        SECTION("e01 ^= e02") {
            subset_type e02(non_default_ptr, {0ul, 2ul});
            auto p = &(e01 ^= e02);
            REQUIRE(p == &e01);
            REQUIRE(e01 == e0);
        }
        SECTION("e02 ^= e01") {
            subset_type e02(non_default_ptr, {0ul, 2ul});
            auto p = &(e02 ^= e01);
            REQUIRE(p == &e02);
            REQUIRE(e02 == e0);
        }
        SECTION("Throws if sets have different parent sets") {
            REQUIRE_THROWS_AS(empty_defaulted ^= e0, std::runtime_error);
        }
    }
    SECTION("operator^") {
        SECTION("empty ^ filled") {
            auto r = empty_non_defaulted ^ e0;
            REQUIRE(empty_non_defaulted == r);
        }
        SECTION("filled ^ empty") {
            auto r = e0 ^ empty_non_defaulted;
            REQUIRE(r == empty_non_defaulted);
        }
        SECTION("e0 ^ e0") {
            auto r = e0 ^ e0;
            REQUIRE(e0 == r);
        }
        SECTION("e0 ^ e2") {
            auto r = e0 ^ e2;
            REQUIRE(r == empty_non_defaulted);
        }
        SECTION("e01 ^ e0") {
            auto r = e01 ^ e0;
            REQUIRE(r == e0);
        }
        SECTION("e0 ^ e01") {
            auto r = e0 ^ e01;
            REQUIRE(e0 == r);
        }
        SECTION("e01 ^ e02") {
            subset_type e02(non_default_ptr, {0ul, 2ul});
            auto r = e01 ^ e02;
            REQUIRE(r == e0);
        }
        SECTION("e02 ^= e01") {
            subset_type e02(non_default_ptr, {0ul, 2ul});
            auto r = e02 ^ e01;
            REQUIRE(r == e0);
        }
        SECTION("Throws if sets have different parent sets") {
            REQUIRE_THROWS_AS(empty_defaulted ^ e0, std::runtime_error);
        }
    }

    SECTION("operator<") {
        SECTION("Less same number of elements") { REQUIRE(e0 < e2); }
        SECTION("Less different number of elements") {
            REQUIRE(e0 < e01);
            REQUIRE(e01 < e2);
        }
        SECTION("Greater same number of elements") { REQUIRE_FALSE(e2 < e0); }
        SECTION("Greater different number of elements") {
            REQUIRE_FALSE(e01 < e0);
            REQUIRE_FALSE(e2 < e01);
        }
        SECTION("Same subset") { REQUIRE_FALSE(e0 < e0); }

        SECTION("Different parents") {
            REQUIRE_FALSE(empty_defaulted < e0);
            REQUIRE_FALSE(e0 < empty_defaulted);
        }
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        SECTION("Different parent objects") {
            auto lhs = hash_objects(empty_defaulted);
            auto rhs = hash_objects(empty_non_defaulted);
            REQUIRE(lhs != rhs);
        }
        SECTION("Empty sets are equal") {
            SECTION("Empty parent set") {
                auto lhs = hash_objects(empty_defaulted);
                subset_type s(default_ptr);
                auto rhs = hash_objects(s);
                REQUIRE(lhs == rhs);
            }

            SECTION("Non-empty parent set") {
                auto lhs = hash_objects(empty_non_defaulted);
                subset_type s(non_default_ptr);
                auto rhs = hash_objects(s);
                REQUIRE(lhs == rhs);
            }
        }
        SECTION("Different number of elements") {
            REQUIRE(hash_objects(e0) != hash_objects(e01));
        }

        SECTION("Different elements") {
            REQUIRE(hash_objects(e0) != hash_objects(e2));
        }

        SECTION("Same elements") {
            auto lhs = hash_objects(e0);
            subset_type s(non_default_ptr, {0ul});
            auto rhs = hash_objects(s);
            REQUIRE(lhs == rhs);
        }
    }

    SECTION("Comparisons") {
        SECTION("Different types") {
            using parent_type = std::vector<double>;
            auto f = std::make_shared<parent_type>(parent_type{1, 2, 3});
            Subset<parent_type> f_subset(f);
            REQUIRE_FALSE(f_subset == empty_defaulted);
            REQUIRE(f_subset != empty_defaulted);
        }
        SECTION("Different parent objects") {
            REQUIRE_FALSE(empty_defaulted == empty_non_defaulted);
            REQUIRE(empty_defaulted != empty_non_defaulted);
        }
        SECTION("Empty sets are equal") {
            SECTION("Empty parent set") {
                subset_type s(default_ptr);
                REQUIRE(s == empty_defaulted);
                REQUIRE_FALSE(s != empty_defaulted);
            }

            SECTION("Non-empty parent set") {
                subset_type s(non_default_ptr);
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
            subset_type s(non_default_ptr, {0ul});
            REQUIRE(s == e0);
            REQUIRE_FALSE(s != e0);
        }
    }
}

/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test_set_theory.hpp"
#include <chemist/set_theory/family_of_sets.hpp>

using namespace chemist::set_theory;

TEMPLATE_LIST_TEST_CASE("FamilyOfSets", "", container_types) {
    // Types we will need
    using parent_type    = TestType;
    using family_type    = FamilyOfSets<TestType>;
    using value_type     = typename family_type::value_type;
    using container_type = boost::container::flat_set<value_type>;

    SECTION("typedefs") {
        SECTION("superset_type") {
            using superset_type = typename family_type::superset_type;
            STATIC_REQUIRE(std::is_same_v<superset_type, parent_type>);
        }
        SECTION("value_type") {
            using corr = Subset<parent_type>;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }

        SECTION("reference_type") {
            using reference_type = typename family_type::reference_type;
            using corr           = value_type&;
            STATIC_REQUIRE(std::is_same_v<reference_type, corr>);
        }

        SECTION("const_reference") {
            using const_reference = typename family_type::const_reference;
            using corr            = const value_type&;
            STATIC_REQUIRE(std::is_same_v<const_reference, corr>);
        }

        SECTION("size_type") {
            using size_type = typename family_type::size_type;
            using corr      = typename container_type::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }

    // Make a default and a non-default parent set
    TestType default_obj;
    auto non_default_obj = testing::make_object<TestType>();

    // Make family of sets for the empty and non-empty parent sets
    family_type non_default(non_default_obj);
    family_type defaulted(default_obj);

    // Families containg subsets
    family_type monomers(non_default_obj, {{0ul}, {1ul}, {2ul}});
    family_type dimers(non_default_obj, {{0ul, 1ul}, {0ul, 2ul}, {1ul, 2ul}});

    // For convenience the subsets in the above families
    value_type m0(monomers.data(), {0ul}), m1(monomers.data(), {1ul});
    value_type m2(monomers.data(), {2ul}), d01(dimers.data(), {0ul, 1ul});
    value_type d02(dimers.data(), {0ul, 2ul}), d12(dimers.data(), {1ul, 2ul});

    SECTION("CTors/Assignment") {
        SECTION("Default Ctor") {
            family_type empty;
            REQUIRE(empty.size() == 0);
        }

        SECTION("Value Ctor") {
            SECTION("Empty family, empty superset") {
                REQUIRE(defaulted.object() == default_obj);
                REQUIRE(defaulted.size() == 0);
            }
            SECTION("Empty family, non-empty superset") {
                REQUIRE(non_default.object() == non_default_obj);
                REQUIRE(non_default.size() == 0);
            }
            SECTION("Disjoint family") {
                REQUIRE(monomers.object() == non_default_obj);
                REQUIRE(monomers.size() == 3);
                REQUIRE(monomers[0] == m0);
                REQUIRE(monomers[1] == m1);
                REQUIRE(monomers[2] == m2);
            }
            SECTION("Intersecting family") {
                REQUIRE(dimers.object() == non_default_obj);
                REQUIRE(dimers.size() == 3);
                REQUIRE(dimers[0] == d01);
                REQUIRE(dimers[1] == d02);
                REQUIRE(dimers[2] == d12);
            }
        }

        SECTION("Copy Ctor") {
            SECTION("Empty") {
                family_type copy(non_default);
                SECTION("value") { REQUIRE(copy.object() == non_default_obj); }
            }
            SECTION("Non-empty") {
                family_type copy(monomers);
                SECTION("value") {
                    REQUIRE(copy.object() == non_default_obj);
                    REQUIRE(copy.size() == 3);
                    REQUIRE(copy[0] == m0);
                    REQUIRE(copy[1] == m1);
                    REQUIRE(copy[2] == m2);
                }
            }
        }

        SECTION("Move Ctor") {
            SECTION("Empty") {
                family_type moved(std::move(non_default));
                REQUIRE(moved.object() == non_default_obj);
            }
            SECTION("Non-empty") {
                family_type moved(std::move(monomers));
                REQUIRE(moved.object() == non_default_obj);
                REQUIRE(moved.size() == 3);
                REQUIRE(moved[0] == m0);
                REQUIRE(moved[1] == m1);
                REQUIRE(moved[2] == m2);
            }
        }

        SECTION("Copy Assignment") {
            SECTION("Empty") {
                family_type copy(defaulted);
                auto p = &(copy = non_default);
                REQUIRE(p == &copy);
                SECTION("value") { REQUIRE(copy.object() == non_default_obj); }
            }
            SECTION("Non-empty") {
                family_type copy(defaulted);
                auto p = &(copy = monomers);
                REQUIRE(p == &copy);
                SECTION("value") {
                    REQUIRE(copy.object() == non_default_obj);
                    REQUIRE(copy.size() == 3);
                    REQUIRE(copy[0] == m0);
                    REQUIRE(copy[1] == m1);
                    REQUIRE(copy[2] == m2);
                }
            }
        }

        SECTION("Move Assignment") {
            SECTION("Empty") {
                family_type moved(defaulted);
                auto p = &(moved = std::move(non_default));
                REQUIRE(p == &moved);
                REQUIRE(moved.object() == non_default_obj);
            }
            SECTION("Non-empty") {
                family_type moved(defaulted);
                auto p = &(moved = std::move(monomers));
                REQUIRE(p == &moved);
                REQUIRE(moved.object() == non_default_obj);
                REQUIRE(moved.size() == 3);
                REQUIRE(moved[0] == m0);
                REQUIRE(moved[1] == m1);
                REQUIRE(moved[2] == m2);
            }
        }
    }

    SECTION("Accessors") {
        SECTION("object") {
            REQUIRE(defaulted.object() == default_obj);
            REQUIRE(non_default.object() == non_default_obj);
            REQUIRE(monomers.object() == non_default_obj);
            REQUIRE(dimers.object() == non_default_obj);
        }
        SECTION("data") {
            REQUIRE(&(*defaulted.data()) == &(defaulted.object()));
            REQUIRE(&(*non_default.data()) == &(non_default.object()));
            REQUIRE(&(*monomers.data()) == &(monomers.object()));
            REQUIRE(&(*dimers.data()) == &(dimers.object()));
        }
        SECTION("empty") {
            REQUIRE(defaulted.empty());
            REQUIRE(non_default.empty());
            REQUIRE_FALSE(monomers.empty());
            REQUIRE_FALSE(dimers.empty());
        }
        SECTION("size") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(non_default.size() == 0);
            REQUIRE(monomers.size() == 3);
            REQUIRE(dimers.size() == 3);
        }
        SECTION("operator[]") {
            REQUIRE(monomers[0] == m0);
            REQUIRE(monomers[1] == m1);
            REQUIRE(monomers[2] == m2);
            REQUIRE(dimers[0] == d01);
            REQUIRE(dimers[1] == d02);
            REQUIRE(dimers[2] == d12);
        }
        SECTION("at()") {
            SECTION("In bounds") {
                REQUIRE(monomers.at(0) == m0);
                REQUIRE(monomers.at(1) == m1);
                REQUIRE(monomers.at(2) == m2);
                REQUIRE(dimers.at(0) == d01);
                REQUIRE(dimers.at(1) == d02);
                REQUIRE(dimers.at(2) == d12);
            }
            SECTION("Throws if out of bounds") {
                REQUIRE_THROWS_AS(dimers.at(3), std::out_of_range);
            }
        }
    }

    SECTION("iterators") {
        SECTION("Empty") { REQUIRE(non_default.begin() == non_default.end()); }
        SECTION("Non-empty") {
            auto b = monomers.begin();
            REQUIRE(b != monomers.end());
            REQUIRE(*b == m0);
            ++b;
            REQUIRE(*b == m1);
            ++b;
            REQUIRE(*b == m2);
            ++b;
            REQUIRE(b == monomers.end());
        }
    }

    SECTION("insert") {
        SECTION("New value") {
            value_type new_val(monomers.data(), {0ul, 2ul});
            monomers.insert(new_val);
            REQUIRE(monomers.size() == 4);
            REQUIRE(monomers[0] == m0);
            REQUIRE(monomers[1] == new_val);
            REQUIRE(monomers[2] == m1);
            REQUIRE(monomers[3] == m2);
        }
        SECTION("Existing value") {
            monomers.insert(m2);
            REQUIRE(monomers.size() == 3);
            REQUIRE(monomers[0] == m0);
            REQUIRE(monomers[1] == m1);
            REQUIRE(monomers[2] == m2);
        }

        SECTION("Throws if different superset") {
            value_type bad_subset(defaulted.data());
            using except_t = std::runtime_error;
            REQUIRE_THROWS_AS(monomers.insert(bad_subset), except_t);
        }
    }

    SECTION("disjoint") {
        REQUIRE(monomers.disjoint());
        REQUIRE_FALSE(dimers.disjoint());
    }

    SECTION("Comparisons") {
        SECTION("Both empty") {
            family_type rhs(default_obj);
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }
        SECTION("Different supersets") {
            REQUIRE_FALSE(defaulted == non_default);
            REQUIRE(defaulted != non_default);
        }
        SECTION("Same non-empty") {
            family_type rhs(non_default_obj, {{0ul}, {1ul}, {2ul}});
            REQUIRE(monomers == rhs);
            REQUIRE_FALSE(monomers != rhs);
        }
        SECTION("Different non-empty size") {
            family_type rhs(non_default_obj, {{0ul}, {1ul}});
            REQUIRE_FALSE(monomers == rhs);
            REQUIRE(monomers != rhs);
        }
        SECTION("Different non-empty subset") {
            family_type rhs(non_default_obj, {{0ul}, {1ul}, {0ul, 1ul}});
            REQUIRE_FALSE(monomers == rhs);
            REQUIRE(monomers != rhs);
        }
        SECTION("Different types") {
            using set_type = std::vector<char>;
            FamilyOfSets<set_type> rhs(set_type{});
            REQUIRE_FALSE(defaulted == rhs);
            REQUIRE(defaulted != rhs);
        }
    }

    SECTION("Subsets work when FoS goes out of scope") {
        value_type subset = m0;
        {
            auto non_default_obj2 = testing::make_object<TestType>();
            family_type f(non_default_obj2, {{0ul}, {1ul}, {2ul}});
            subset = f[1];
        }
        REQUIRE(subset.object() == non_default_obj);
        REQUIRE(subset.size() == 1);
        REQUIRE(subset.count(non_default_obj[1]));
    }
}

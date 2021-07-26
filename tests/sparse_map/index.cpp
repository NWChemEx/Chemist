#include "libchemist/sparse_map/index.hpp"
#include <catch2/catch.hpp>
#include <sstream>

using namespace libchemist::sparse_map;

// Types commonly used as integers
using integral_types = std::tuple<int, unsigned int, std::size_t>;

using index_type = typename Index::index_type;

/* Testing Notes:
 *
 * - We need to ensure that we don't have to use std::size_t instances to
 *   initialize Index instances as that's annoying for literals.
 */

TEST_CASE("Index::Typedefs") {
    SECTION("size_type") {
        using corr_t   = std::size_t;
        using the_type = typename Index::size_type;
        STATIC_REQUIRE(std::is_same_v<the_type, corr_t>);
    }

    SECTION("index_type") {
        using corr_t = std::vector<std::size_t>;
        STATIC_REQUIRE(std::is_same_v<index_type, corr_t>);
    }
}

TEST_CASE("Index::default ctor") {
    Index i;
    index_type corr;
    REQUIRE(i == corr);
}

TEMPLATE_LIST_TEST_CASE("Index::value ctor", "", integral_types) {
    TestType one{1}, two{2}, three{3};
    index_type corr{1, 2, 3};
    Index i(one, two, three);
    REQUIRE(i == corr);
}

TEST_CASE("Index::initializer_list ctor") {
    index_type corr{1, 2, 3};
    Index i({1, 2, 3});
    REQUIRE(i == corr);
}

TEST_CASE("Index::vector ctor") {
    index_type corr{1, 2, 3};

    SECTION("By copy") {
        Index i(corr);
        REQUIRE(i == corr);
    }

    SECTION("By move") {
        auto* pdata = corr.data();
        Index i(std::move(corr));
        REQUIRE(i.m_index.data() == pdata);
    }
}

TEST_CASE("Index::range ctor") {
    index_type corr{1, 2, 3};
    Index i(corr.begin(), corr.end());
    REQUIRE(i == corr);
}

TEST_CASE("Index::copy ctor") {
    SECTION("Default") {
        Index i;
        Index j(i);
        REQUIRE(i == j);
    }

    SECTION("Rank 1") {
        Index i(1);
        Index j(i);
        REQUIRE(i == j);
    }

    SECTION("Rank 2") {
        Index i(1, 2);
        Index j(i);
        REQUIRE(i == j);
    }
}

TEST_CASE("Index::move ctor") {
    SECTION("Default") {
        Index i, j;
        Index k(std::move(j));
        REQUIRE(k == i);
    }

    SECTION("Rank 1") {
        Index i(1);
        Index j(i);
        Index k(std::move(i));
        REQUIRE(k == j);
    }

    SECTION("Rank 2") {
        Index i(1, 2);
        Index j(i);
        Index k(std::move(i));
        REQUIRE(k == j);
    }
}

TEST_CASE("Index::copy assignment") {
    SECTION("Default") {
        Index i, j;
        auto pj = &(j = i);
        SECTION("value") { REQUIRE(i == j); }
        SECTION("returns *this") { REQUIRE(pj == &j); }
    }

    SECTION("Rank 1") {
        Index i(1), j;
        auto pj = &(j = i);
        SECTION("value") { REQUIRE(i == j); }
        SECTION("returns *this") { REQUIRE(pj == &j); }
    }

    SECTION("Rank 2") {
        Index i(1, 2), j;
        auto pj = &(j = i);
        SECTION("value") { REQUIRE(i == j); }
        SECTION("returns *this") { REQUIRE(pj == &j); }
    }
}

TEST_CASE("Index::move assignment") {
    SECTION("Default") {
        Index i, j, k;
        auto pj = &(j = std::move(i));
        SECTION("value") { REQUIRE(j == k); }
        SECTION("returns *this") { REQUIRE(pj == &j); }
    }

    SECTION("Rank 1") {
        Index i(1), j;
        Index k(i);
        auto pj = &(j = std::move(i));
        SECTION("value") { REQUIRE(j == k); }
        SECTION("returns *this") { REQUIRE(pj == &j); }
    }

    SECTION("Rank 2") {
        Index i(1, 2), j;
        Index k(i);
        auto pj = &(j = std::move(i));
        SECTION("value") { REQUIRE(j == k); }
        SECTION("returns *this") { REQUIRE(pj == &j); }
    }
}

TEST_CASE("Index::size") {
    SECTION("Default") {
        Index i;
        REQUIRE(i.size() == 0);
    }

    SECTION("Rank 1") {
        Index i(1);
        REQUIRE(i.size() == 1);
    }

    SECTION("Rank 2") {
        Index i(1, 2);
        REQUIRE(i.size() == 2);
    }
}

TEST_CASE("Index::operator[]") {
    SECTION("Default") {
        Index i;
        REQUIRE_THROWS_AS(i[0], std::out_of_range);
    }

    SECTION("Rank 1") {
        Index i(1);
        SECTION("Good mode") { REQUIRE(i[0] == 1); }
        SECTION("Bad mode") { REQUIRE_THROWS_AS(i[1], std::out_of_range); }
    }

    SECTION("Rank 2") {
        Index i(1, 2);
        SECTION("Good mode") {
            REQUIRE(i[0] == 1);
            REQUIRE(i[1] == 2);
        }
        SECTION("Bad mode") { REQUIRE_THROWS_AS(i[2], std::out_of_range); }
    }
}

TEST_CASE("Index::begin") {
    SECTION("Default") {
        Index i;
        REQUIRE(i.begin() == i.m_index.begin());
    }

    SECTION("Rank 1") {
        Index i(1);
        REQUIRE(i.begin() == i.m_index.begin());
    }

    SECTION("Rank 2") {
        Index i(1, 2);
        REQUIRE(i.begin() == i.m_index.begin());
    }
}

TEST_CASE("Index::begin const") {
    SECTION("Default") {
        const Index i;
        REQUIRE(i.begin() == i.m_index.begin());
    }

    SECTION("Rank 1") {
        const Index i(1);
        REQUIRE(i.begin() == i.m_index.begin());
    }

    SECTION("Rank 2") {
        const Index i(1, 2);
        REQUIRE(i.begin() == i.m_index.begin());
    }
}

TEST_CASE("Index::end") {
    SECTION("Default") {
        Index i;
        REQUIRE(i.end() == i.m_index.end());
    }

    SECTION("Rank 1") {
        Index i(1);
        REQUIRE(i.end() == i.m_index.end());
    }

    SECTION("Rank 2") {
        Index i(1, 2);
        REQUIRE(i.end() == i.m_index.end());
    }
}

TEST_CASE("Index::end const") {
    SECTION("Default") {
        const Index i;
        REQUIRE(i.end() == i.m_index.end());
    }

    SECTION("Rank 1") {
        const Index i(1);
        REQUIRE(i.end() == i.m_index.end());
    }

    SECTION("Rank 2") {
        const Index i(1, 2);
        REQUIRE(i.end() == i.m_index.end());
    }
}

TEST_CASE("Index::hash") {
    SECTION("Default Index") {
        Index i;
        auto hi = pluginplay::hash_objects(i);

        SECTION("Default Index") {
            Index j;
            REQUIRE(hi == pluginplay::hash_objects(j));
        }

        SECTION("Rank 1 Index") {
            Index j{1};
            REQUIRE(hi != pluginplay::hash_objects(j));
        }

        SECTION("Rank 2 Index") {
            Index j{1, 2};
            REQUIRE(hi != pluginplay::hash_objects(j));
        }
    }

    SECTION("Rank 1 Index") {
        Index i{1};
        auto hi = pluginplay::hash_objects(i);

        SECTION("Default Index") {
            Index j;
            REQUIRE(hi != pluginplay::hash_objects(j));
        }

        SECTION("Same Rank 1 Index") {
            Index j{1};
            REQUIRE(hi == pluginplay::hash_objects(j));
        }

        SECTION("Different Rank 1 Index") {
            Index j{2};
            REQUIRE(hi != pluginplay::hash_objects(j));
        }

        SECTION("Rank 2 Index") {
            Index j{1, 2};
            REQUIRE(hi != pluginplay::hash_objects(j));
        }
    }

    SECTION("Rank 2 Index") {
        Index i{1, 2};
        auto hi = pluginplay::hash_objects(i);

        SECTION("Default Index") {
            Index j;
            REQUIRE(hi != pluginplay::hash_objects(j));
        }

        SECTION("Rank 1 Index") {
            Index j{1};
            REQUIRE(hi != pluginplay::hash_objects(j));
        }

        SECTION("Same Rank 2 Index") {
            Index j{1, 2};
            REQUIRE(hi == pluginplay::hash_objects(j));
        }

        SECTION("Different Rank 2 Index") {
            Index j{2, 1};
            REQUIRE(hi != pluginplay::hash_objects(j));
        }
    }
}

TEST_CASE("Index::print") {
    std::stringstream ss;

    SECTION("Default") {
        Index i;
        std::string corr("{}");
        auto pss = &(i.print(ss));
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }

    SECTION("Rank 1") {
        Index i{1};
        std::string corr("{1}");
        auto pss = &(i.print(ss));
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }

    SECTION("Rank 2") {
        Index i{1, 2};
        std::string corr("{1, 2}");
        auto pss = &(i.print(ss));
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }

    SECTION("Rank 3") {
        Index i{1, 2, 3};
        std::string corr("{1, 2, 3}");
        auto pss = &(i.print(ss));
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }
}

TEST_CASE("Comparisons among Index instances") {
    SECTION("Default Index") {
        Index i;

        SECTION("Default Index") {
            Index j;
            REQUIRE(i == j);
            REQUIRE_FALSE(i != j);
        }

        SECTION("Rank 1 Index") {
            Index j{1};
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }

        SECTION("Rank 2 Index") {
            Index j{1, 2};
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }
    }

    SECTION("Rank 1 Index") {
        Index i{1};

        SECTION("Default Index") {
            Index j;
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }

        SECTION("Same Rank 1 Index") {
            Index j{1};
            REQUIRE(i == j);
            REQUIRE_FALSE(i != j);
        }

        SECTION("Different Rank 1 Index") {
            Index j{2};
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }

        SECTION("Rank 2 Index") {
            Index j{1, 2};
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }
    }

    SECTION("Rank 2 Index") {
        Index i{1, 2};

        SECTION("Default Index") {
            Index j;
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }

        SECTION("Rank 1 Index") {
            Index j{1};
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }

        SECTION("Same Rank 2 Index") {
            Index j{1, 2};
            REQUIRE(i == j);
            REQUIRE_FALSE(i != j);
        }

        SECTION("Different Rank 2 Index") {
            Index j{2, 1};
            REQUIRE_FALSE(i == j);
            REQUIRE(i != j);
        }
    }
}

TEST_CASE("Comparisons between index and std::vector") {
    SECTION("Default Index") {
        Index i;

        SECTION("Default std::vector") {
            index_type v;
            REQUIRE(i == v);
            REQUIRE(v == i);
            REQUIRE_FALSE(i != v);
            REQUIRE_FALSE(v != i);
        }

        SECTION("Single element std::vector") {
            index_type v{1};
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }

        SECTION("Two element std::vector") {
            index_type v{1, 2};
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }
    }

    SECTION("Rank 1 index") {
        Index i(1);

        SECTION("Default std::vector") {
            index_type v;
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }

        SECTION("Same valued std::vector") {
            index_type v{1};
            REQUIRE(i == v);
            REQUIRE(v == i);
            REQUIRE_FALSE(i != v);
            REQUIRE_FALSE(v != i);
        }

        SECTION("Different element std::vector") {
            index_type v{2};
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }

        SECTION("Two element std::vector") {
            index_type v{1, 2};
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }
    }

    SECTION("Rank 2 index") {
        Index i(1, 2);

        SECTION("Default std::vector") {
            index_type v;
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }

        SECTION("Single element std::vector") {
            index_type v{1};
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }

        SECTION("Same value, two-element std::vector") {
            index_type v{1, 2};
            REQUIRE(i == v);
            REQUIRE(v == i);
            REQUIRE_FALSE(i != v);
            REQUIRE_FALSE(v != i);
        }

        SECTION("Different value, two element std::vector") {
            index_type v{2, 1};
            REQUIRE_FALSE(i == v);
            REQUIRE_FALSE(v == i);
            REQUIRE(i != v);
            REQUIRE(v != i);
        }
    }
}

TEST_CASE("operator<<(std::ostream, Index)") {
    std::stringstream ss;

    SECTION("Default") {
        Index i;
        std::string corr("{}");
        auto pss = &(ss << i);
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }

    SECTION("Rank 1") {
        Index i{1};
        std::string corr("{1}");
        auto pss = &(ss << i);
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }

    SECTION("Rank 2") {
        Index i{1, 2};
        std::string corr("{1, 2}");
        auto pss = &(ss << i);
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }

    SECTION("Rank 3") {
        Index i{1, 2, 3};
        std::string corr("{1, 2, 3}");
        auto pss = &(ss << i);
        SECTION("Value") { REQUIRE(ss.str() == corr); }
        SECTION("Supports chaining") { REQUIRE(pss == &ss); }
    }
}

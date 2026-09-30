/*
 * Copyright 2026 NWChemEx-Project
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

// AOViewCache is tested directly here, with a stand-in view type, so that its
// contract can be pinned down independently of the shells which use it. That
// the shells use it correctly (e.g. that a copied shell's AOs alias the copy)
// is tested in cca_shell_common.cpp.

#include "../../../test_helpers.hpp"
#include <chemist/experimental/basis_set/detail_/ao_view_cache.hpp>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

using chemist::experimental::detail_::AOViewCache;

namespace {

/** @brief Stands in for an AO view.
 *
 *  Like SphericalAOView it can be copied and moved but not assigned, which is
 *  the most restrictive kind of view the cache has to hold. It records which
 *  offset and which "build" it came from, so a test can tell a stored view
 *  from a rebuilt one.
 */
struct FakeView {
    FakeView(std::size_t offset, int build) : offset(offset), build(build) {}
    FakeView(const FakeView&)            = default;
    FakeView(FakeView&&) noexcept        = default;
    FakeView& operator=(const FakeView&) = delete;
    FakeView& operator=(FakeView&&)      = delete;

    std::size_t offset;
    int build;
};

using cache_type = AOViewCache<FakeView>;

/** @brief Makes views, counting how many it has made.
 *
 *  Each view is tagged with @p build, so the views of two different builds
 *  can be told apart even if they sit at the same address.
 */
struct Maker {
    int build  = 0;
    int n_made = 0;

    auto operator()() {
        return [this](std::size_t i) {
            ++n_made;
            return FakeView(i, build);
        };
    }
};

} // namespace

TEST_CASE("experimental::detail_::AOViewCache") {
    Maker maker;
    cache_type cache;

    SECTION("Is copyable and movable, and never throws doing so") {
        STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<cache_type>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<cache_type>);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<cache_type>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<cache_type>);
    }

    SECTION("get builds one view per offset") {
        const auto& views = cache.get(2, 6, maker());
        REQUIRE(views.size() == 6);
        REQUIRE(maker.n_made == 6);
        for(std::size_t i = 0; i < views.size(); ++i)
            REQUIRE(views[i].offset == i);
    }

    SECTION("get reuses the views while l is unchanged") {
        const auto& first  = cache.get(2, 6, maker());
        const auto* p_view = &first[3];
        const auto& second = cache.get(2, 6, maker());

        REQUIRE(maker.n_made == 6);    // Not rebuilt ...
        REQUIRE(&second[3] == p_view); // ... so references stay valid
    }

    SECTION("get rebuilds the views when l changes") {
        cache.get(2, 6, maker());
        maker.build       = 1;
        const auto& views = cache.get(3, 10, maker());

        REQUIRE(views.size() == 10);
        REQUIRE(maker.n_made == 16);
        REQUIRE(views[0].build == 1);
    }

    SECTION("get rebuilds the views when l changes back") {
        // The cache remembers only the l it was built for, not a history
        cache.get(2, 6, maker());
        cache.get(3, 10, maker());
        const auto& views = cache.get(2, 6, maker());
        REQUIRE(views.size() == 6);
        REQUIRE(maker.n_made == 22);
    }

    SECTION("get builds the views even for l = 0") {
        // l = 0 is also the l an empty cache records, so being empty, not the
        // recorded l, has to be what triggers the first build.
        const auto& views = cache.get(0, 1, maker());
        REQUIRE(views.size() == 1);
        REQUIRE(maker.n_made == 1);
    }

    SECTION("clear forces the next get to rebuild") {
        cache.get(2, 6, maker());
        cache.clear();
        maker.build       = 1;
        const auto& views = cache.get(2, 6, maker());

        REQUIRE(maker.n_made == 12);
        REQUIRE(views[0].build == 1);
    }

    SECTION("get has the strong throw guarantee") {
        cache.get(2, 6, maker());

        // Fails partway through building the l = 3 views
        auto failing = [](std::size_t i) -> FakeView {
            if(i == 4) throw std::runtime_error("no");
            return FakeView(i, 1);
        };
        REQUIRE_THROWS_AS(cache.get(3, 10, failing), std::runtime_error);

        // The l = 2 views are intact, and still recorded as being for l = 2
        const auto& views = cache.get(2, 6, maker());
        REQUIRE(maker.n_made == 6);
        REQUIRE(views.size() == 6);
        REQUIRE(views[0].build == 0);
    }

    SECTION("Copying yields an empty cache and leaves the source alone") {
        const auto* p_view = &cache.get(2, 6, maker())[0];

        cache_type copy(cache);
        maker.build            = 1;
        const auto& copy_views = copy.get(2, 6, maker());
        REQUIRE(maker.n_made == 12); // copy had to build its own ...
        REQUIRE(copy_views[0].build == 1);

        // ... while the source kept its views
        REQUIRE(&cache.get(2, 6, maker())[0] == p_view);
        REQUIRE(maker.n_made == 12);
    }

    SECTION("Moving yields an empty cache and empties the source") {
        cache.get(2, 6, maker());

        cache_type moved(std::move(cache));
        moved.get(2, 6, maker());
        REQUIRE(maker.n_made == 12);

        // The source's views aliased state which was moved out, so it must
        // not keep handing them out.
        cache.get(2, 6, maker());
        REQUIRE(maker.n_made == 18);
    }

    SECTION("Copy assignment empties the target and leaves the source alone") {
        cache_type target;
        target.get(2, 6, maker());
        const auto* p_view = &cache.get(2, 6, maker())[0];
        REQUIRE(maker.n_made == 12);

        target = cache;
        target.get(2, 6, maker());
        REQUIRE(maker.n_made == 18);

        REQUIRE(&cache.get(2, 6, maker())[0] == p_view);
        REQUIRE(maker.n_made == 18);
    }

    SECTION("Move assignment empties both") {
        cache_type target;
        target.get(2, 6, maker());
        cache.get(2, 6, maker());
        REQUIRE(maker.n_made == 12);

        target = std::move(cache);
        target.get(2, 6, maker());
        cache.get(2, 6, maker());
        REQUIRE(maker.n_made == 24);
    }

    SECTION("get works through a const cache") {
        // The shells populate the cache from const methods
        const cache_type& const_cache = cache;
        REQUIRE(const_cache.get(1, 3, maker()).size() == 3);
    }
}

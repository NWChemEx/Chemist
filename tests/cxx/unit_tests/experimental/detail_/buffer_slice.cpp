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

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/detail_/buffer_slice.hpp>
#include <stdexcept>
#include <vector>

using namespace chemist::experimental::detail_;
using test_chemist::as_double;

TEST_CASE("experimental::detail_::slice_buffer") {
    using buffer_type     = wtf::buffer::FloatBuffer;
    using view_type       = wtf::buffer::BufferView<wtf::fp::Float>;
    using const_view_type = wtf::buffer::BufferView<const wtf::fp::Float>;

    buffer_type buffer(std::vector<double>{1.0, 2.0, 3.0, 4.0});
    const buffer_type& cbuffer = buffer;

    SECTION("Mutable slice") {
        view_type slice = slice_buffer(buffer.as_view(), 1, 2);
        REQUIRE(slice.size() == 2);
        REQUIRE(as_double(std::as_const(slice).at(0)) == 2.0);
        REQUIRE(as_double(std::as_const(slice).at(1)) == 3.0);
        REQUIRE(slice.is_contiguous());

        // Writes through the slice land in the original buffer
        slice.at(0) = 42.0;
        REQUIRE(as_double(cbuffer.as_view().at(1)) == 42.0);
    }

    SECTION("Read-only slice") {
        const_view_type slice = slice_buffer(cbuffer.as_view(), 2, 2);
        REQUIRE(slice.size() == 2);
        REQUIRE(as_double(slice.at(0)) == 3.0);
        REQUIRE(as_double(slice.at(1)) == 4.0);

        // Writes to the original are visible through the slice
        buffer.as_view().at(3) = 42.0;
        REQUIRE(as_double(slice.at(1)) == 42.0);
    }

    SECTION("The whole buffer") {
        auto slice = slice_buffer(cbuffer.as_view(), 0, 4);
        REQUIRE(slice == cbuffer.as_view());
    }

    SECTION("Empty slices") {
        REQUIRE(slice_buffer(buffer.as_view(), 1, 0).size() == 0);
        // Including the one at the end, and the one of an empty buffer
        REQUIRE(slice_buffer(cbuffer.as_view(), 4, 0).size() == 0);
        REQUIRE(slice_buffer(const_view_type{}, 0, 0).size() == 0);
    }

    SECTION("Out of range") {
        REQUIRE_THROWS_AS(slice_buffer(buffer.as_view(), 3, 2),
                          std::out_of_range);
        REQUIRE_THROWS_AS(slice_buffer(cbuffer.as_view(), 5, 0),
                          std::out_of_range);
        REQUIRE_THROWS_AS(slice_buffer(const_view_type{}, 0, 1),
                          std::out_of_range);
    }
}

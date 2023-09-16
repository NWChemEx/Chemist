/*
 * Copyright 2023 NWChemEx-Project
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

#include "chemist_examples.hpp"
#include <catch2/catch.hpp>

using namespace chemist_examples;
TEST_CASE("Doc snippets") {
    REQUIRE(nucleus_example() == 0);
    REQUIRE(atom_example() == 0);
    REQUIRE(molecule_example() == 0);
}

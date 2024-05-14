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

#include <catch2/catch.hpp>
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian.hpp>
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian_traits.hpp>
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp>

using namespace chemist::basis_set;

// This unit test basically just ensures that all of the types we plan to use
// ContractedGaussianTraits with actually define the correct API to be used with
// ContractedGaussianTraits

TEMPLATE_TEST_CASE("ContractedGaussianTraits", "", float, double) {
    using cg_type = ContractedGaussian<Primitive<TestType>>;

    REQUIRE_NOTHROW(ContractedGaussianTraits<cg_type>());

    REQUIRE_NOTHROW(
      ContractedGaussianTraits<ContractedGaussianView<cg_type>>());

    REQUIRE_NOTHROW(
      ContractedGaussianTraits<ContractedGaussianView<const cg_type>>());
}

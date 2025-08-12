/*
 * Copyright 2024 NWChemEx-Project
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

#include "../catch.hpp"
#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <chemist/quantum_mechanics/wavefunction/wavefunction_class.hpp>
#include <chemist/traits/quantum_mechanics_traits.hpp>

using namespace chemist;
using namespace chemist::qm_operator;
using namespace chemist::wavefunction;

using operator_list =
  std::tuple<OperatorBase, Kinetic<Electron>, Kinetic<ManyElectrons>>;
using wavefunction_list = std::tuple<Wavefunction>;
using vector_space_list = std::tuple<VectorSpace, AOs>;

TEMPLATE_LIST_TEST_CASE("is_operator_v == true", "", operator_list) {
    STATIC_REQUIRE(is_operator_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("is_operator_v == false", "", wavefunction_list) {
    STATIC_REQUIRE_FALSE(is_operator_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("is_wavefunction_v == true", "", wavefunction_list) {
    STATIC_REQUIRE(is_wavefunction_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("is_wavefunction_v == false", "", vector_space_list) {
    STATIC_REQUIRE_FALSE(is_wavefunction_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("is_vector_space_v == true", "", vector_space_list) {
    STATIC_REQUIRE(is_vector_space_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("is_vector_space_v == false", "", operator_list) {
    STATIC_REQUIRE_FALSE(is_vector_space_v<TestType>);
}

TEST_CASE("is_tensor_element_v") {
    STATIC_REQUIRE(
      is_tensor_element_v<Wavefunction, OperatorBase, Wavefunction>);
    STATIC_REQUIRE_FALSE(is_tensor_element_v<AOs, OperatorBase, Wavefunction>);
    STATIC_REQUIRE_FALSE(is_tensor_element_v<Wavefunction, AOs, Wavefunction>);
    STATIC_REQUIRE_FALSE(is_tensor_element_v<Wavefunction, OperatorBase, AOs>);
}

TEST_CASE("is_tensor_representation_v") {
    STATIC_REQUIRE(is_tensor_representation_v<AOs, OperatorBase, AOs>);
    STATIC_REQUIRE_FALSE(
      is_tensor_representation_v<Wavefunction, OperatorBase, AOs>);
    STATIC_REQUIRE_FALSE(is_tensor_representation_v<AOs, AOs, AOs>);
    STATIC_REQUIRE_FALSE(
      is_tensor_representation_v<AOs, OperatorBase, Wavefunction>);
}

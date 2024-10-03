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
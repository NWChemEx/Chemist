#pragma once
#include <type_traits>

namespace chemist {
namespace qm_operator {
class OperatorBase;
}

namespace wavefunction {
class Wavefunction;
class VectorSpace;
} // namespace wavefunction

/** @brief Determines if @p T is or inherits from OperatorBase.
 *
 *  All objects representing quantum mechanical operators derive from
 *  OperatorBase. This template variable will be set to true if @p T is either
 *  OperatorBase or a class that derives from it.
 *
 *  @tparam T The type to inspect.
 */
template<typename T>
static constexpr auto is_operator_v =
  std::is_base_of_v<qm_operator::OperatorBase, T>;

/** @brief Determines if @p T is or inherits from Wavefunction.
 *
 *  All objects representing wavefunctions derive from Wavefunction. This
 *  template variable will be set to true if @p T is either Wavefunction or a
 *  class that derives from it.
 *
 *  @tparam T The type to inspect.
 */
template<typename T>
static constexpr auto is_wavefunction_v =
  std::is_base_of_v<wavefunction::Wavefunction, T>;

/** @brief Determines if @p T is or inherits from VectorSpace.
 *
 *  All objects representing vector spaces derive from VectorSpace. This
 *  template variable will be set to true if @p T is either VectorSpace or a
 *  class that derives from it.
 *
 *  @tparam T The type to inspect.
 */
template<typename T>
static constexpr auto is_vector_space_v =
  std::is_base_of_v<wavefunction::VectorSpace, T>;

/** @brief Determines if the template type parameters passed to a BraKet object
 *         make it result in a tensor element.
 *
 *  BraKet objects are templated on the type of the bra, operator, and the ket.
 *  If the types of the bra and the ket both derive from Wavefunction, then the
 *  bra-ket described by the BraKet object will be a single element of a tensor
 *  (usually a scalar, but if @p OperatorType is say the dipole operator it
 *  may actually be a tensor too). This template variable is set to true if the
 *  template type parameters map to a tensor element and false otherwise.
 *
 *  @tparam BraType The type of the object occupying the bra slot of the BraKet.
 *  @tparam OperatorType the type of the object occupying the operator slot of
 *                       the BraKet.
 *  @tparam KetType The type of the object occupying hte ket slot of the BraKet.
 */
template<typename BraType, typename OperatorType, typename KetType>
static constexpr auto is_tensor_element_v =
  is_wavefunction_v<BraType> && is_operator_v<OperatorType> &&
  is_wavefunction_v<KetType>;

/** @brief Determines if the template type parameters passed to a BraKet object
 *         make it result in the tensor representation of the operator.
 *
 *  BraKet objects are templated on the type of the bra, operator, and the ket.
 *  If the types of the bra and the ket both derive from VectorSpace, then the
 *  BraKet defines the tensor representation of the operator. This template
 *  variable is set to true if the template type parameters map to a tensor
 *  representation and false otherwise.
 *
 *  @tparam BraType The type of the object occupying the bra slot of the BraKet.
 *  @tparam OperatorType the type of the object occupying the operator slot of
 *                       the BraKet.
 *  @tparam KetType The type of the object occupying hte ket slot of the BraKet.
 */
template<typename BraType, typename OperatorType, typename KetType>
static constexpr auto is_tensor_representation_v =
  is_vector_space_v<BraType> && is_operator_v<OperatorType> &&
  is_vector_space_v<KetType>;

} // namespace chemist
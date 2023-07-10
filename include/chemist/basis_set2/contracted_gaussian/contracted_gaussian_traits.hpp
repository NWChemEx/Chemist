#pragma once
#include <chemist/basis_set2/primitive/primitive_traits.hpp>

namespace chemist::basis_set {

/** @brief Propagates types associated with a ContractedGaussian of type
 *         @p CGType.
 *
 *  This struct uses inheritance because the total set of types needed for
 *  interacting with a ContractedGaussian is the union of the types needed for
 *  the primitives making up the ContractedGaussian plus the types exclusive
 *  to the ContractedGaussian. *
 *
 *  @tparam CGType expected to be either an instantiation of ContractedGaussian
 *                 or ContractedGaussianView.
 */
template<typename CGType>
struct ContractedGaussianTraits
  : public PrimitiveTraits<typename CGType::reference> {
    /// Type of a Primitive in a ContractedGaussian of type @p CGType
    using primitive_type = typename CGType::value_type;

    /// Type of a reference to a mutable Primitive in a ContractedGaussian
    using primitive_reference = typename CGType::reference;

    /// Type of a reference to a read-only Primitive in a ContractedGaussian
    using const_primitive_reference = typename CGType::const_reference;
};

} // namespace chemist::basis_set

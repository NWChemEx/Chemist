#pragma once

namespace chemist {

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
struct ContractedGaussianTraits : public CGType::primitive_traits {
    using primitive_type = typename CGType::value_type;

    using primitive_reference = typename CGType::reference;

    using const_primitive_reference = typename CGType::const_reference;

    using coefficient_vector = typename CGType::coefficient_vector;

    using exponent_vector = typename CGType::exponent_vector;
};

} // namespace chemist

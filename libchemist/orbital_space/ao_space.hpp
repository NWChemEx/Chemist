#pragma once
#include "libchemist/orbital_space/orbital_space_class.hpp"

namespace libchemist {

// A class representing an AO space
template<typename element_type = double, typename tensor_type  = type::tensor<element_type>>
class AOSpace : public OrbitalSpace<element_type, tensor_type> {
public:
    using basis_type = AOBasisSet<element_type>; // Basis set type
    using size_vec = std::vector<std::size_t>; // Type of list of modes for conversion
    using tensor_elems = typename tensor_type::value_type::numeric_type; // type of the tensor elements

    /** @brief Creates a new AOSpace instance containing empty members. */
    AOSpace();

    /** @brief Creates a new AOSpace instance with the provided values.
     *
     * C and Cdagger are set to identity with the shape of S, and the density is set to S.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     */
    AOSpace(basis_type bs, tensor_type S);

    /** @brief Return the provided matrix with the specified modes transformed from the AO space
     *
     * For AOSpace, just returns X.
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    tensor_type transform_from_ao(tensor_type X, const size_vec& modes) const override;

    /** @brief Return the provided matrix with the specified modes transformed to the AO space
     *
     * For AOSpace, just returns X.
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    tensor_type transform_to_ao(tensor_type X, const size_vec& modes) const override;
}; // class AOSpace

extern template class AOSpace<float, type::tensor<float>>;
extern template class AOSpace<float, type::tensor<double>>;
extern template class AOSpace<double, type::tensor<float>>;
extern template class AOSpace<double, type::tensor<double>>;
//extern template class AOSpace<float, type::tensor_of_tensors<float>>;
//extern template class AOSpace<float, type::tensor_of_tensors<double>>;
//extern template class AOSpace<double, type::tensor_of_tensors<float>>;
//extern template class AOSpace<double, type::tensor_of_tensors<double>>;

} // namespace libchemist

#pragma once
#include <chemist/basis_set/ao_basis_set/ao_basis_set.hpp>
#include <chemist/molecule.hpp>
#include <chemist/ta_helpers/ta_helpers.hpp>
#include <chemist/types.hpp>
#include <sde/hasher.hpp>

namespace chemist {

// Forward declaration of PIMPL
namespace detail_ {
template<typename basis_type, typename tensor_type>
class OrbitalSpacePIMPL;
} // namespace detail_

// A class representing an orbital space
template<typename element_type = double,
         typename tensor_type  = type::tensor<element_type>>
class OrbitalSpace {
public:
    using basis_type = AOBasisSet<element_type>; // Basis set type
    using size_vec =
      std::vector<std::size_t>; // Type of list of modes for conversion

private:
    using pimpl_type =
      detail_::OrbitalSpacePIMPL<basis_type, tensor_type>; // PIMPL type
    std::unique_ptr<pimpl_type> m_pimpl_; // pointer to implementation

protected:
    /** @brief Accessor to m_pimpl_. */
    const pimpl_type& pimpl_() const;

    /** @brief Creates a new OrbitalSpace instance with the provided PIMPL.
     *
     * @param pimpl Pointer to the implementation instance
     */
    OrbitalSpace(std::unique_ptr<pimpl_type> pimpl) :
      m_pimpl_(std::move(pimpl)) {}

public:
    /** @brief Creates a new OrbitalSpace instance containing empty members. */
    OrbitalSpace();

    /** @brief Creates a new OrbitalSpace instance with the provided values.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     * @param D The denisty of the MOs
     */
    OrbitalSpace(basis_type bs, tensor_type S, tensor_type C,
                 tensor_type Cdagger, tensor_type D);

    /** @brief Creates a new OrbitalSpace instance with the provided values.
     *
     * The density of calculated from C and Cdagger
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     */
    OrbitalSpace(basis_type bs, tensor_type S, tensor_type C,
                 tensor_type Cdagger);

    /** @brief Creates a new OrbitalSpace instance with the provided values.
     *
     * Cdagger is assumed to be the transpose of C, and the density calculated
     * as such.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param C The coefficients going from the AO space to the MO space
     */
    OrbitalSpace(basis_type bs, tensor_type S, tensor_type C);

    /** @brief Creates a new OrbitalSpace instance by deep copying another
     * instance.
     *
     * @param rhs The OrbitalSpace to be copied
     */
    OrbitalSpace(const OrbitalSpace<element_type, tensor_type>& rhs);

    /** @brief Creates a new OrbitalSpace instance that takes ownership of
     * another instance's state.
     *
     * @param rhs The OrbitalSpace whose state is transferred
     */
    OrbitalSpace(OrbitalSpace<element_type, tensor_type>&& rhs) noexcept;

    /** @brief Makes the current OrbitalSpace instance a deep copy of another
     * instance.
     *
     * @param rhs The OrbitalSpace to be copied
     */
    OrbitalSpace& operator=(const OrbitalSpace<element_type, tensor_type>& rhs);

    /** @brief Makes the current OrbitalSpace instance take ownership of another
     * instance's state.
     *
     * @param rhs The OrbitalSpace to be copied
     */
    OrbitalSpace& operator=(
      OrbitalSpace<element_type, tensor_type>&& rhs) noexcept;

    ~OrbitalSpace() noexcept;

    /** @brief Return a reference to the basis set of the space
     *
     * @return A reference to the basis set
     */
    const basis_type& basis_set() const;

    /** @brief Return a reference to the overlap matrix of the space
     *
     * @return A reference to the overlap matrix
     */
    const tensor_type& S() const;

    /** @brief Return a reference to the coefficients of the space
     *
     * @return A reference to the coefficients
     */
    const tensor_type& C() const;

    /** @brief Return a reference to the conjugate coefficients of the space
     *
     * @return A reference to the conjugate coefficients
     */
    const tensor_type& Cdagger() const;

    /** @brief Return a reference to the density of the space
     *
     * @return A reference to the density
     */
    const tensor_type& density() const;

    /** @brief Return the provided matrix with the specified modes transformed
     * from the AO space
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    virtual tensor_type transform_from_ao(tensor_type X,
                                          const size_vec& modes) const;

    /** @brief Return the provided matrix with the specified modes transformed
     * to the AO space
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    virtual tensor_type transform_to_ao(tensor_type X,
                                        const size_vec& modes) const;

    /** @brief Hash the OrbitalSpace
     *
     * @param h The hash function
     */
    void hash(sde::Hasher& h) const;

}; // class OrbitalSpace

// A class representing an AO space
template<typename element_type = double,
         typename tensor_type  = type::tensor<element_type>>
class AOSpace : public OrbitalSpace<element_type, tensor_type> {
public:
    using basis_type = AOBasisSet<element_type>; // Basis set type
    using size_vec =
      std::vector<std::size_t>; // Type of list of modes for conversion
    using tensor_elems =
      typename tensor_type::value_type::numeric_type; // type of the tensor
    // elements

    /** @brief Creates a new AOSpace instance containing empty members. */
    AOSpace();

    /** @brief Creates a new AOSpace instance with the provided values.
     *
     * C and Cdagger are set to identity with the shape of S, and the density is
     * set to S.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     */
    AOSpace(basis_type bs, tensor_type S);

    /** @brief Return the provided matrix with the specified modes transformed
     * from the AO space
     *
     * For AOSpace, just returns X.
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    tensor_type transform_from_ao(tensor_type X,
                                  const size_vec& modes) const override;

    /** @brief Return the provided matrix with the specified modes transformed
     * to the AO space
     *
     * For AOSpace, just returns X.
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    tensor_type transform_to_ao(tensor_type X,
                                const size_vec& modes) const override;
}; // class AOSpace

extern template class OrbitalSpace<float, type::tensor<float>>;
extern template class OrbitalSpace<float, type::tensor<double>>;
extern template class OrbitalSpace<double, type::tensor<float>>;
extern template class OrbitalSpace<double, type::tensor<double>>;

extern template class AOSpace<float, type::tensor<float>>;
extern template class AOSpace<float, type::tensor<double>>;
extern template class AOSpace<double, type::tensor<float>>;
extern template class AOSpace<double, type::tensor<double>>;

/** @brief Determine where two OrbitalSpace instances are equivalent
 *
 * @param space1 The first instance
 * @param space2 The second instance
 *
 * @return true if the instances are equivalent
 */
template<typename E1, typename E2, typename T>
bool operator==(const OrbitalSpace<E1, T>& space1,
                const OrbitalSpace<E2, T>& space2) {
    return (space1.basis_set() == space2.basis_set() &&
            space1.S().trange() == space2.S().trange() &&
            space1.C().trange() == space2.C().trange() &&
            space1.Cdagger().trange() == space2.Cdagger().trange() &&
            space1.density().trange() == space2.density().trange() &&
            ta_helpers::allclose(space1.S(), space2.S()) &&
            ta_helpers::allclose(space1.C(), space2.C()) &&
            ta_helpers::allclose(space1.Cdagger(), space2.Cdagger()) &&
            ta_helpers::allclose(space1.density(), space2.density()));
}

/** @brief Determine where two OrbitalSpace instances are not equivalent
 *
 * @param space1 The first instance
 * @param space2 The second instance
 *
 * @return true if the instances are not equivalent
 */
template<typename E1, typename E2, typename T>
bool operator!=(const OrbitalSpace<E1, T>& space1,
                const OrbitalSpace<E2, T>& space2) {
    return !(space1 == space2);
}
} // end namespace chemist

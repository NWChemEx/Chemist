#pragma once
#include "libchemist/orbital_space/orthogonal_space.hpp"

namespace libchemist {

// Forward declaration of PIMPL
namespace detail_ {

template<typename basis_type, typename tensor_type, typename energy_type>
class CanonicalMOsPIMPL;

} // namespace detail_

// A class representing a canonical molecular orbital space
template<typename element_type = double,
         typename tensor_type  = type::tensor<element_type>,
         typename energy_type  = tensor_type>
class CanonicalMO : public OrthogonalSpace<element_type, tensor_type> {
private:
    /// Type of an instance of this class
    using my_type = CanonicalMO<element_type, tensor_type, energy_type>;

    /// Type of the base class
    using base_type = OrthogonalSpace<element_type, tensor_type>;

    /// Type of a PIMPL
    using pimpl_type = detail_::CanonicalMOsPIMPL<typename base_type::basis_type, tensor_type, energy_type>;

    /** @brief Accessor to PIMPL as pimpl_type, over the base PIMPL. */
    const pimpl_type& dpimpl_() const;

public:
    using basis_type = typename base_type::basis_type;

    /** @brief Creates a new CanonicalMO instance containing empty members. */
    CanonicalMO();

    /** @brief Creates a new CanonicalMO instance with the provided values.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param mo_energies The molecular orbital energies
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     * @param D The denisty of the MOs
     */
    CanonicalMO(basis_type bs, tensor_type S, energy_type mo_energies,
                tensor_type C, tensor_type Cdagger, tensor_type D);

    /** @brief Creates a new CanonicalMO instance with the provided values.
     *
     * The density of calculated from C and Cdagger
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param mo_energies The molecular orbital energies
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     */
    CanonicalMO(basis_type bs, tensor_type S, energy_type mo_energies,
                tensor_type C, tensor_type Cdagger);

    /** @brief Creates a new CanonicalMO instance with the provided values.
     *
     * Cdagger is assumed to be the transpose of C, and the density calculated
     * as such.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param mo_energies The molecular orbital energies
     * @param C The coefficients going from the AO space to the MO space
     */
    CanonicalMO(basis_type bs, tensor_type S, energy_type mo_energies,
                tensor_type C);

    /** @brief Creates a new CanonicalMO instance by deep copying another
     * instance.
     *
     * @param rhs The CanonicalMO to be copied
     */
    CanonicalMO(const my_type& rhs);

    /** @brief Creates a new CanonicalMO instance that takes ownership of
     * another instance's state.
     *
     * @param rhs The CanonicalMO whose state is transferred
     */
    CanonicalMO(my_type&& rhs) noexcept;

    /** @brief Makes the current CanonicalMO instance a deep copy of another
     * instance.
     *
     * @param rhs The CanonicalMO to be copied
     */
    CanonicalMO& operator=(const my_type& rhs);

    /** @brief Makes the current CanonicalMO instance take ownership of another
     * instance's state.
     *
     * @param rhs The CanonicalMO to be copied
     */
    CanonicalMO& operator=(my_type&& rhs) noexcept;

    ~CanonicalMO() noexcept;

    /** @brief Return a reference to the MO energies
     *
     * @return A reference to the MO energies
     */
    const energy_type& mo_energies() const;
};

/** @brief Determine where two OrbitalSpace instances are equivalent
 *
 * @param space1 The first instance
 * @param space2 The second instance
 *
 * @return true if the instances are equivalent
 */
template<typename E1, typename E2, typename T, typename T2>
bool operator==(const CanonicalMO<E1, T, T2>& space1,
                const CanonicalMO<E2, T, T2>& space2) {
    //TODO: Equality should be exact, remove allclose
    return (OrthogonalSpace<E1, T>(space1) == OrthogonalSpace<E2, T>(space2) &&
            space1.mo_energies().trange() == space2.mo_energies().trange() &&
            allclose(space1.mo_energies(), space2.mo_energies()));
}

/** @brief Determine where two OrbitalSpace instances are not equivalent
 *
 * @param space1 The first instance
 * @param space2 The second instance
 *
 * @return true if the instances are not equivalent
 */
template<typename E1, typename E2, typename T, typename T2>
bool operator!=(const CanonicalMO<E1, T, T2>& space1,
                const CanonicalMO<E2, T, T2>& space2) {
    return !(space1 == space2);
}

extern template class CanonicalMO<float, type::tensor<float>>;
extern template class CanonicalMO<float, type::tensor<double>>;
extern template class CanonicalMO<double, type::tensor<float>>;
extern template class CanonicalMO<double, type::tensor<double>>;
extern template class CanonicalMO<float, type::tensor_of_tensors<float>>;
extern template class CanonicalMO<float, type::tensor_of_tensors<double>>;
extern template class CanonicalMO<double, type::tensor_of_tensors<float>>;
extern template class CanonicalMO<double, type::tensor_of_tensors<double>>;
extern template class CanonicalMO<float, type::tensor_of_tensors<float>, type::tensor<float>>;
extern template class CanonicalMO<float, type::tensor_of_tensors<double>, type::tensor<double>>;
extern template class CanonicalMO<double, type::tensor_of_tensors<float>, type::tensor<float>>;
extern template class CanonicalMO<double, type::tensor_of_tensors<double>, type::tensor<double>>;
} // namespace libchemist

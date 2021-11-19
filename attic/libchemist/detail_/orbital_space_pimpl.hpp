#pragma once
#include "chemist/basis_set/ao_basis_set/ao_basis_set.hpp"
#include "chemist/orbital_space.hpp"
#include "chemist/ta_helpers/detail_/contraction_dummy_annotation.hpp"

namespace chemist::detail_ {

// Class that implements OrbitalSpace
template<typename basis_type, typename tensor_type>
class OrbitalSpacePIMPL {
    using size_vec =
      std::vector<std::size_t>; // Type of list of modes for conversion
    using basis_pointer =
      std::shared_ptr<basis_type>; // shared pointer to a basis set
    using tensor_pointer =
      std::shared_ptr<tensor_type>; // shared pointer to a tensor

    basis_pointer basis_set_; // basis set of the orbital space
    tensor_pointer S_;        // overlap matrix of the basis set
    tensor_type C_;           // conversion from AO basis to current basis
    mutable tensor_type
      Cdagger_;             // conversion from current orbital basis to AO basis
    mutable tensor_type D_; // density of the orbitals

    /** @brief Initialize Cdagger as the transpose of C */
    void init_Cdagger_() const {
        if(C_.is_initialized()) Cdagger_("a, b") = C_("b, a");
    }

    /** @brief Initialize density from C and Cdagger */
    void init_D_() const {
        if(!Cdagger_.is_initialized()) init_Cdagger_();
        if(C_.is_initialized()) D_("mu, nu") = C_("mu, i") * Cdagger_("i, nu");
    }

public:
    /** @brief Creates a new OrbitalSpacePIMPL instance containing empty
     * members. */
    OrbitalSpacePIMPL() :
      basis_set_(std::make_shared<basis_type>(basis_type())),
      S_(std::make_shared<tensor_type>(tensor_type())) {}

    /** @brief Creates a new OrbitalSpacePIMPL instance with the provided
     * values.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     * @param D The denisty of the MOs
     */
    OrbitalSpacePIMPL(basis_type basis_set, tensor_type S, tensor_type C,
                      tensor_type Cdagger, tensor_type D) :
      basis_set_(std::make_shared<basis_type>(basis_set)),
      S_(std::make_shared<tensor_type>(S)),
      C_(C),
      Cdagger_(Cdagger),
      D_(D) {}

    /** @brief Creates a new OrbitalSpacePIMPL instance with the provided
     * values.
     *
     * The density of calculated from C and Cdagger
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     */
    OrbitalSpacePIMPL(basis_type basis_set, tensor_type S, tensor_type C,
                      tensor_type Cdagger) :
      basis_set_(std::make_shared<basis_type>(basis_set)),
      S_(std::make_shared<tensor_type>(S)),
      C_(C),
      Cdagger_(Cdagger) {}

    /** @brief Creates a new OrbitalSpacePIMPL instance with the provided
     * values.
     *
     * Cdagger is assumed to be the transpose of C, and the density calculated
     * as such.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param C The coefficients going from the AO space to the MO space
     */
    OrbitalSpacePIMPL(basis_type basis_set, tensor_type S, tensor_type C) :
      basis_set_(std::make_shared<basis_type>(basis_set)),
      S_(std::make_shared<tensor_type>(S)),
      C_(C) {}

    OrbitalSpacePIMPL(const OrbitalSpacePIMPL&) = delete;

    /** @brief Creates a clone of the OrbitalSpacePIMPL.
     *
     * @return A clone of the current OrbitalSpacePIMPL
     */
    virtual std::unique_ptr<OrbitalSpacePIMPL> clone() const {
        return std::make_unique<OrbitalSpacePIMPL>(*basis_set_, *S_, C_,
                                                   Cdagger_, D_);
    }

    /** @brief Return a reference to the basis set of the space
     *
     * @return A reference to the basis set
     */
    const basis_type& basis_set() const { return *(basis_set_); }

    /** @brief Return a reference to the overlap matrix of the space
     *
     * @return A reference to the overlap matrix
     */
    const tensor_type& S() const { return *(S_); }

    /** @brief Return a reference to the coefficients of the space
     *
     * @return A reference to the coefficients
     */
    const tensor_type& C() const { return C_; }

    /** @brief Return a reference to the conjugate coefficients of the space
     *
     * @return A reference to the conjugate coefficients
     */
    const tensor_type& Cdagger() const {
        if(!Cdagger_.is_initialized()) init_Cdagger_();
        return Cdagger_;
    }

    /** @brief Return a reference to the density of the space
     *
     * @return A reference to the density
     */
    const tensor_type& density() const {
        if(!D_.is_initialized()) init_D_();
        return D_;
    }

    /** @brief Return the provided matrix with the specified modes transformed
     * from the AO space
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    tensor_type transform_from_ao(const tensor_type X,
                                  const size_vec& modes) const {
        tensor_type rv(X);
        auto n_modes = X.range().rank();
        for(const auto& i : modes) {
            auto [start, finish, change] =
              ta_helpers::detail_::contraction_dummy_annotations(
                X.trange().rank(), i);
            rv(finish) = rv(start) * C_(change);
        }
        return rv;
    }

    /** @brief Return the provided matrix with the specified modes transformed
     * to the AO space
     *
     * @param X The tensor to be transformed
     * @param modes The list of modes of X that should be transformed
     *
     * @return The transformed matrix
     */
    tensor_type transform_to_ao(const tensor_type X,
                                const size_vec& modes) const {
        tensor_type rv(X);
        auto n_modes = X.range().rank();
        for(const auto& i : modes) {
            auto [start, finish, change] =
              ta_helpers::detail_::contraction_dummy_annotations(
                X.trange().rank(), i);
            rv(finish) = rv(start) * Cdagger_(change);
        }
        return rv;
    }

}; // class OrbitalSpacePIMPL

} // namespace chemist::detail_

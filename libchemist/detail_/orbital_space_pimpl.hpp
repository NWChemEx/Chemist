#pragma once
#include "libchemist/orbital_space.hpp"
#include <libchemist/basis_set/ao_basis_set/ao_basis_set.hpp>

namespace libchemist::detail_ {

    // Class that implements OrbitalSpace
    template<typename basis_type, typename tensor_type>
    class OrbitalSpacePIMPL {
        using size_vec = std::vector<std::size_t>; // Type of list of modes for conversion
        using basis_pointer = std::shared_ptr<basis_type>; // shared pointer to a basis set
        using tensor_pointer = std::shared_ptr<tensor_type>; // shared pointer to a tensor

        basis_pointer basis_set_; // basis set of the orbital space
        tensor_pointer S_; // overlap matrix of the basis set
        tensor_type C_; // conversion from AO basis to current basis
        tensor_type Cdagger_; // conversion from current orbital basis to AO basis
        tensor_type D_; // density of the orbitals

        /** @brief Initialize Cdagger as the transpose of C */
        void init_Cdagger_() {
            if (C_.is_initialized()) Cdagger_("a, b") = C_("b, a");
        }

        /** @brief Initialize density from C and Cdagger */
        void init_D_() {
            if (!Cdagger_.is_initialized()) init_Cdagger_();
            if (C_.is_initialized()) D_("mu, nu") = C_("mu, i") * Cdagger_("i, nu");
        }

        /** @brief Make the indices to transform the selected mode of a tensor
         *
         * @param n_modes The number of modes in the tensor
         * @param target_mode The mode that will be transformed
         *
         * @return The indices of the tensor before and after transformation
         */
        std::pair<std::string, std::string> make_indices(std::size_t n_modes, std::size_t target_mode) {
            std::string initial;
            std::string transformed;

            for (int i = 0; i < n_modes; ++i) {
                if (i > 0) {
                    initial.append(",");
                    transformed.append(",");
                }
                if (i != target_mode) {
                    initial.append(std::to_string(i));
                    transformed.append(std::to_string(i));
                } else {
                    initial.append("A");
                    transformed.append("B");
                }
            }

            return std::pair<std::string, std::string>(transformed, initial);
        }

    public:
        /** @brief Creates a new OrbitalSpacePIMPL instance containing empty members. */
        OrbitalSpacePIMPL() : basis_set_(std::make_shared<basis_type>(basis_type())),
            S_(std::make_shared<tensor_type>(tensor_type())) {}

        /** @brief Creates a new OrbitalSpacePIMPL instance with the provided values.
         *
         * @param bs The basis set of the space
         * @param S The overlap matrix of the basis set
         * @param C The coefficients going from the AO space to the MO space
         * @param Cdagger The coefficients going from the MO space to the AO space
         * @param D The denisty of the MOs
         */
        OrbitalSpacePIMPL(basis_type basis_set, tensor_type S, tensor_type C, tensor_type Cdagger, tensor_type D) :
            basis_set_(std::make_shared<basis_type>(basis_set)), S_(std::make_shared<tensor_type>(S)),
            C_(C), Cdagger_(Cdagger), D_(D) {}

        /** @brief Creates a new OrbitalSpacePIMPL instance with the provided values.
         *
         * The density of calculated from C and Cdagger
         *
         * @param bs The basis set of the space
         * @param S The overlap matrix of the basis set
         * @param C The coefficients going from the AO space to the MO space
         * @param Cdagger The coefficients going from the MO space to the AO space
         */
        OrbitalSpacePIMPL(basis_type basis_set, tensor_type S, tensor_type C, tensor_type Cdagger) :
            basis_set_(std::make_shared<basis_type>(basis_set)), S_(std::make_shared<tensor_type>(S)),
            C_(C), Cdagger_(Cdagger){}

        /** @brief Creates a new OrbitalSpacePIMPL instance with the provided values.
         *
         * Cdagger is assumed to be the transpose of C, and the density calculated as such.
         *
         * @param bs The basis set of the space
         * @param S The overlap matrix of the basis set
         * @param C The coefficients going from the AO space to the MO space
         */
        OrbitalSpacePIMPL(basis_type basis_set, tensor_type S, tensor_type C) :
            basis_set_(std::make_shared<basis_type>(basis_set)), S_(std::make_shared<tensor_type>(S)), C_(C) {}

        /** @brief Return a reference to the basis set of the space
         *
         * @return A reference to the basis set
         */
        basis_type& basis_set() { return *(basis_set_); }

        /** @brief Return a reference to the overlap matrix of the space
         *
         * @return A reference to the overlap matrix
         */
        tensor_type& S() { return *(S_); }

        /** @brief Return a reference to the coefficients of the space
         *
         * @return A reference to the coefficients
         */
        tensor_type& C() { return C_; }

        /** @brief Return a reference to the conjugate coefficients of the space
         *
         * @return A reference to the conjugate coefficients
         */
        tensor_type& Cdagger() {
            if (!Cdagger_.is_initialized()) init_Cdagger_();
            return Cdagger_;
        }

        /** @brief Return a reference to the density of the space
         *
         * @return A reference to the density
         */
        tensor_type& density() {
            if (!D_.is_initialized()) init_D_();
            return D_;
        }

        /** @brief Return the provided matrix with the specified modes transformed from the AO space
         *
         * @param X The tensor to be transformed
         * @param modes The list of modes of X that should be transformed
         *
         * @return The transformed matrix
         */
        tensor_type transform_from_ao(const tensor_type X, const size_vec& modes) {
            tensor_type rv(X);
            auto n_modes = X.range().rank();
            for (const auto& i : modes) {
                auto indices = make_indices(n_modes, i);
                rv(indices.first) = rv(indices.second) * C_("A,B");
            }
            return rv;
        }

        /** @brief Return the provided matrix with the specified modes transformed to the AO space
         *
         * @param X The tensor to be transformed
         * @param modes The list of modes of X that should be transformed
         *
         * @return The transformed matrix
         */
        tensor_type transform_to_ao(const tensor_type X, const size_vec& modes) {
            tensor_type rv(X);
            auto n_modes = X.range().rank();
            for (const auto& i : modes) {
                auto indices = make_indices(n_modes, i);
                rv(indices.first) = rv(indices.second) * Cdagger_("A,B");
            }
            return rv;
        }

    }; // class OrbitalSpacePIMPL

} //namespace libchemist::detail_
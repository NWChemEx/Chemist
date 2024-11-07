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

#pragma once
#include <chemist/quantum_mechanics/wavefunction/transformed.hpp>
#include <tensorwrapper/tensorwrapper.hpp>

namespace chemist::wavefunction {

/** @brief A space whose transformation is chosen such that it diagonalizes a
 *         matrix.
 *
 *  @tparam FromSpaceType The type of the space this is a transformation from.
 *                        Expected to be a class
 *
 *  Spaces like the canonical molecular orbitals are defined in terms of a
 *  transformation from a set of atomic orbitals to a set of orbitals which are
 *  eigenfunctions of the Fock matrix. In this sense, they are a set of
 *  transformed orbitals, but they are also tied to the matrix they diagonalize.
 *
 *  The Natural class extends the Transformed class so that it additionally
 *  stores the matrix the orbitals diagonalize. Since that matrix is diagonal
 *  it is easily stored.
 */
template<typename FromSpaceType>
class Natural : public Transformed<FromSpaceType> {
private:
    /// Type of *this
    using my_type = Natural<FromSpaceType>;

public:
    /// Type of *this when viewed as a Transformed object
    using transformed_type = Transformed<FromSpaceType>;

    /// Type of the diagonal matrix associated with *this
    using tensor_type = typename transformed_type::transform_type;

    /// Type of a mutable reference to an object of type tensor_type
    using tensor_reference = typename transformed_type::transform_reference;

    /// Type of a read-only reference to an object of type tensor_type
    using const_tensor_reference =
      typename transformed_type::const_transform_reference;

    /// Pull in types from the base class
    ///@{
    using typename transformed_type::base_pointer;
    using typename transformed_type::from_space_type;
    using typename transformed_type::transform_type;
    ///@}

    Natural() = default;

    Natural(tensor_type diagonalized_matrix, from_space_type from_space,
            transform_type c) :
      transformed_type(std::move(from_space), std::move(c)),
      m_matrix_(std::move(diagonalized_matrix)) {}

    /** @brief The diagonal of the matrix *this diagonalizes.
     *
     * @return The diagonal of the matrix *this diagonalizes.
     *
     * @throw None No throw guarantee.
     */
    tensor_reference diagonalized_matrix() noexcept { return m_matrix_; }

    const_tensor_reference diagonalized_matrix() const noexcept {
        return m_matrix_;
    }

    bool operator==(const Natural& rhs) const noexcept {
        if(m_matrix_ != rhs.m_matrix_) return false;
        return transformed_type::operator==(rhs);
    }

    bool operator!=(const Natural& rhs) const noexcept {
        return !((*this) == rhs);
    }

protected:
    /// Implements clone by calling copy ctor
    base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    /// Implements are_equal by calling are_equal_impl_
    bool are_equal_(const VectorSpace& rhs) const noexcept override {
        return this->template are_equal_impl_<my_type>(rhs);
    }

private:
    /// The matrix *this diagonalizes
    tensor_type m_matrix_;
};

} // namespace chemist::wavefunction
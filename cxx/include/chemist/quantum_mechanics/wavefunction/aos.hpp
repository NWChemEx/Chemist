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
#include <chemist/basis_set/ao_basis_set.hpp>
#include <chemist/quantum_mechanics/wavefunction/vector_space.hpp>

namespace chemist::wavefunction {

/** @brief Attempts to span the problem space using atomic orbitals (AOs)
 *
 *  This class represents the vector space which is spanned by a set of atomic
 *  orbitals (AOs).
 */
class AOs : public VectorSpace {
public:
    /// Type *this derives from
    using base_type = VectorSpace;

    /// Type of a pointer to *this through the base class
    using base_type::base_pointer;

    /// Type used for indexing and offsets
    using base_type::size_type;

    /// Type of the parameters defining the AOs
    using ao_basis_type = basis_set::AOBasisSetD;

    /// Type of a mutable reference to an ao_basis_type object
    using ao_basis_reference = ao_basis_type&;

    /// Type of a read-only reference to an ao_basis_type object
    using const_ao_basis_reference = const ao_basis_type&;

    // -------------------------------------------------------------------------
    // -- Ctors, Assignment, and Dtor
    // -------------------------------------------------------------------------

    /** @brief Creates an empty AO space.
     *
     *  The space resulting from the default ctor will have no AOs in it.
     *  To add AOs either assign to *this or assign to the result of
     *  calling `ao_basis_set()`.
     *
     *  @throw None No throw guarantee.
     */
    AOs() noexcept = default;

    /** @brief Creates an AO space spanned by @p aos.
     *
     *  This ctor takes as input the parameters for a series of Gaussian
     *  atomic orbitals. The resulting space is spanned by the input orbitals.
     *
     *  @param[in] aos The parameters for the AOs which span the space
     *
     *  @throw None No throw guarantee.
     */
    explicit AOs(ao_basis_type aos) noexcept : m_aos_(std::move(aos)) {}

    /** @brief Initializes *this to a deep copy of @p other.
     *
     *  This ctor creates a new AOs instance by deep copying @p other.
     *
     *  @param[in] other The set of AOs to copy.
     *
     *  @throw std::bad_alloc if there is a problem copying @p other. Strong
     *                        throw guarantee.
     */
    AOs(const AOs& other) = default;

    /** @brief Initializes *this by taking ownership of the AOs in @p other.
     *
     *  This ctor creates a new AOs object which will own the AOs that were
     *  previously owned by @p other.
     *
     *  @param[in,out] other The object to take the orbitals from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined state.
     *
     *  @throw None No throw guarantee.
     */
    AOs(AOs&& other) noexcept = default;

    /** @brief Overwrites the state of *this with a deep copy of @p rhs.
     *
     *  This method will deep copy @p rhs. Then, and only if the copy succeeded,
     *  this method will set *this equal to the deep copy and free the state
     *  which was in *this.
     *
     *  @param[in] rhs The object to copy.
     *
     *  @return *this after overwriting its state with a deep copy of the state
     *          in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem copying @p rhs. Strong
     *                        throw guarantee.
     */
    AOs& operator=(const AOs& rhs) {
        if(this != &rhs) AOs(rhs).swap(*this);
        return *this;
    }

    /** @brief Overwrites the state of *this with the state in @p rhs.
     *
     *  This method will free the state held by *this and replace it with the
     *  state in @p rhs.
     *
     *  @param[in,out] rhs The object to take the state from. After this
     *                     operation @p rhs will be in a valid, but otherwise
     *                     undefined state.
     *
     *  @return *this after taking the state of @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    AOs& operator=(AOs&& rhs) noexcept {
        m_aos_ = std::move(rhs.m_aos_);
        return *this;
    }

    // -------------------------------------------------------------------------
    // -- Getters and Setters
    // -------------------------------------------------------------------------

    // No throw read/write access to the AO parameters
    ao_basis_reference ao_basis_set() noexcept { return m_aos_; }

    // No throw read-only access to the AO parameters
    const_ao_basis_reference ao_basis_set() const noexcept { return m_aos_; }

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /// No throw exchanges the state of *this with that of @p other
    void swap(AOs& other) noexcept { m_aos_.swap(other.m_aos_); }

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two AO objects are value equal if they contain the same parameters.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool operator==(const AOs& rhs) const noexcept {
        return m_aos_ == rhs.m_aos_;
    }

    /** @brief Determines if *this is different from @p rhs.
     *
     *  Two AO objects are different if they are not value equal. See
     *  operator== for the definition of value equal.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return False if *this and @p rhs are value equal and true otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool operator!=(const AOs& rhs) const noexcept { return !(*this == rhs); }

protected:
    /// Implements clone by calling copy ctor
    base_pointer clone_() const override {
        return std::make_unique<AOs>(*this);
    }

    /// Implements size by calling n_aos() on the parameters
    size_type size_() const noexcept override { return m_aos_.n_aos(); }

    /// Implements are_equal by calling are_equal_impl_
    bool are_equal_(const VectorSpace& rhs) const noexcept override {
        return are_equal_impl_<AOs>(rhs);
    }

private:
    /// The parameters defining the AOs
    ao_basis_type m_aos_;
};

} // namespace chemist::wavefunction

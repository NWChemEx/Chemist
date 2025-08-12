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
#include <chemist/quantum_mechanics/wavefunction/vector_space.hpp>
#include <tensorwrapper/tensorwrapper.hpp>

namespace chemist::wavefunction {

/** @brief Represents a space that is a linear transformation of another space.
 *
 *  @tparam FromSpaceType The type of the vector space which is transformed to
 *                        make the space represented by *this.
 *
 *  Given an initial vector space, which we term the "from space", we can obtain
 *  a new vector space, which we term the "to space", by applying a linear
 *  transformation to the "from space". The "to space" will actually be the same
 *  vector space, but with different basis functions, if the transformation is
 *  unitary. This class represents not just the "to space", but also its
 *  relationship to the "from space".
 */
template<typename FromSpaceType>
class Transformed : public VectorSpace {
private:
    /// Type of *this
    using my_type = Transformed<FromSpaceType>;

public:
    /// Pull in types from the base class
    ///@{
    using VectorSpace::base_pointer;
    using VectorSpace::size_type;
    ///@}

    /// Type of the linear transformation
    using transform_type = tensorwrapper::Tensor;

    /// Type of a mutable reference to an object of type transform_type
    using transform_reference = transform_type&;

    /// Type of a read-only reference to an object of type transform_type
    using const_transform_reference = const transform_type&;

    /// Type of the "from space"
    using from_space_type = FromSpaceType;

    /// Type of a mutable reference to an object of type from_space_type
    using from_space_reference = from_space_type&;

    /// Type of a read-only reference to an object of type from_space_type
    using const_from_space_reference = const from_space_type&;

    /** @brief A null transformation from the empty space.
     *
     *  This default constructs both the "from space" and the linear
     *  transformation. For most choices of @p FromSpaceType the resulting
     *  vector space will be an empty space and the transformation will be a
     *  null tensor with no state. State can be assigned to the resulting
     *  Transformed object via the from_space and transform methods.
     *
     *  @throw None No throw guarantee.
     */
    Transformed() = default;

    /** @brief Represents the space obtained by applying @p c to @p from_space.
     *
     *  This ctor creates the space represented by taking linear combinations
     *  of the vectors in @p from_space according to the columns of @p c. In
     *  other words the @f$i@f$-th column of @p c is the @f$i@f$-th vector of
     *  the "to space" given as a linear combination of vectors in
     *  @p from_space.
     *
     *  @param[in] from_space The space which is transformed to form *this.
     *  @param[in] c          The linear transformation to apply.
     *
     *  @throw None No throw guarantee.
     */
    Transformed(from_space_type from_space, transform_type c) :
      m_space_(std::move(from_space)), m_c_(std::move(c)) {}

    /** @brief Initializes *this to a deep copy of @p other.
     *
     *  This ctor will deep copy both the "from space" and the transform in
     *  @p other and use the deep copies to initialize *this.
     *
     *  @param[in] other The object to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    Transformed(const Transformed& other) = default;

    /** @brief Initializes *this by taking the state from @p other.
     *
     *  This ctor will take the "from space" and transform in @p other and move
     *  them into *this.
     *
     *  @param[in,out] other The object to take the state from. After this
     *                       operation @p other will be in a valid but otherwise
     *                       undefined state.
     *
     *  @throw None No throw guarantee.
     */
    Transformed(Transformed&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p rhs.
     *
     *  This method will release the state currently held by *this and replace
     *  it with a deep copy of the state found in @p rhs.
     *
     *  @param[in] rhs The object to copy.
     *
     *  @return *this after overwriting its state with a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    Transformed& operator=(const Transformed& rhs) = default;

    /** @brief Overwrites the state in *this with the state in @p rhs.
     *
     *  This method will release the state in *this and then replace it with
     *  the state in @p rhs.
     *
     *  @param[in,out] rhs The object to take the state from. After this
     *                     operation @p rhs is in a valid, but otherwise
     *                     undefined state.
     *
     *  @return *this after overwriting the state in *this with the state
     *          that was originally in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    Transformed& operator=(Transformed&& rhs) noexcept = default;

    /** @brief Provides mutable access to the from space.
     *
     *  @note This class will not check that the modified from space is
     *        consistent with the transformation.
     *
     *  @return Mutable access to the from space.
     *
     *  @throw None No throw guarantee.
     */
    from_space_reference from_space() { return m_space_; }

    /** @brief Provides read-only access to the from space.
     *
     *  This method is identical to the non-const version except that the
     *  resulting reference is read-only. See the description for the non-const
     *  version for more details.
     *
     *  @return Read-only access to the from space.
     *
     *  @throw None No throw guarantee.
     */
    const_from_space_reference from_space() const { return m_space_; }

    /** @brief Returns a mutable reference to the linear transformation.
     *
     *  This method returns a reference to the linear transformation stored as
     *  a matrix. By convention, the column vectors of the resulting matrix are
     *  the basis functions for the "to space" expressed in terms of the basis
     *  functions of the "from space". If this method is used to modify the
     *  linear transformation, the user is responsible for ensuring that the
     *  new matrix obeys this convention.
     *
     *  @return Mutable access to the linear transformation.
     *
     *  @throw None No throw guarantee.
     */
    transform_reference transform() { return m_c_; }

    /** @brief Returns a read-only reference to the linear transformation.
     *
     *  This method behaves the same as the non-const version except that the
     *  resulting reference is read-only. See the description for the non-const
     *  version for more details.
     *
     *  @return A read-only reference to the linear transformation.
     *
     *  @throw None No throw guarantee.
     */
    const_transform_reference transform() const { return m_c_; }

    /** @brief Is *this value equal to @p rhs?
     *
     *  Two Transformed spaces are value equal if they both originate from the
     *  same "to space" and if the linear transformations to go from the
     *  "to space" to the "from space" are value equal also.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return True if *this and @p rhs are value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Transformed& rhs) const noexcept {
        return std::tie(m_space_, m_c_) == std::tie(rhs.m_space_, rhs.m_c_);
    }

    /** @brief Determines if *this is different from @p rhs.
     *
     *  Two transformed spaces are defined as different if they are not value
     *  equal. This method simply negates the result of operator==. See the
     *  description for operator== for the definition of value equal.
     *
     *  @param[in] rhs The object to compare against.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Transformed& rhs) const noexcept {
        return !((*this) == rhs);
    }

protected:
    /// Implements clone by calling copy ctor
    base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    /// Implements size by returning number of columns
    size_type size_() const noexcept override {
        if(m_c_ == transform_type{}) return 0;
        return m_c_.logical_layout().shape().as_smooth().extent(1);
    }

    /// Implements are_equal by calling are_equal_impl_
    bool are_equal_(const VectorSpace& rhs) const noexcept override {
        return are_equal_impl_<my_type>(rhs);
    }

private:
    /// The space we are transforming from
    from_space_type m_space_;

    /// The basis vectors of the space we are transforming to, as a matrix
    transform_type m_c_;
};

} // namespace chemist::wavefunction

/*
 * Copyright 2022 NWChemEx-Project
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
#include <chemist/vector_space/base_space.hpp>
#include <string>
#include <vector>

namespace chemist::vector_space {

/** @brief Models a space spanned by cartesian axes (R^N).
 *
 *  R^N is the N-dimensional real space spanned by N orthogonal axes.
 *  In practice, N is often 3, in which case the basis functions are usually
 *  labeled "x", "y", and "z". However, the user may customize the number of
 *  basis functions and their labels.
 */
class CartesianSpace : public BaseSpace {
private:
    /// String-like type used for labeling basis functions
    using label_type            = std::string;
    using label_container       = std::vector<label_type>;
    using const_label_reference = const label_container&;

    /// dimension of the space
    size_type m_N_;
    /// vector of axis labels
    label_container m_axis_vec_;

public:
    /// Type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;

    /** @brief Creates an N-dimensinal CartesianSpace with axes set as "None".
     *
     *  @param[in] N The dimension of the space.
     *
     *  @throw std::bad_alloc if changing of the capacity of the vector fails.
     */
    CartesianSpace(size_type N) : m_N_(N) {
        m_axis_vec_.reserve(m_N_);
        m_axis_vec_.assign(m_N_, "None");
    }

    /** @brief Creates an N-dimensinal CartesianSpace with axis labels being
     * given.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @param[in] ItType The type of the begin and end iterator of the label
     * vector to set up the axes. Default to be
     * std::vector<std::string>::iterator>.
     *
     *  @param[in] beginIt, endIt The begin and end iterator of the label vector
     *             to set up the axes.
     *
     *  @throw Throws if the length of string vector is not equal to the
     *         dimension of the space (invalid_argument) or initialization of
     * the string vector throws.
     */
    template<typename ItType = std::vector<std::string>::iterator>
    CartesianSpace(const size_type& N, ItType&& beginIt, ItType&& endIt) :
      m_N_(N),
      m_axis_vec_(std::forward<ItType>(beginIt), std::forward<ItType>(endIt)) {
        if(m_axis_vec_.size() != m_N_)
            throw std::invalid_argument("Label vector length not equal to the"
                                        "dimension of the space!");
    }

    /** @brief Copy constructor. Copy another CartesianSpace.
     *
     *  @param[in] rhs The CartesianSpace to be copied.
     *
     *  @throw None No throw gurantee.
     */
    CartesianSpace(const CartesianSpace& rhs) = default;

    /** @brief Initializes this instance by taking the state of @p other.
     *
     *  The move ctor is a standard move ctor. After this call the new instance
     *  will have the same dimension and contain the same coordinate labels
     *  as those of @p other.
     *
     *  @param[in,out] other The CartesianSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *  @throw None No throws guarantee.
     */
    CartesianSpace(CartesianSpace&& other) = default;

    /** @brief Replaces the internal state with a copy of another CartesianSpace
     *         instance's state.
     *
     *  @note This operator is not polymorphic, i.e. the resulting instance
     *        may slice @p other.
     *
     *  @param[in] rhs The CartesianSpace instance we are copying
     *
     *  @return The current instance, after replacing its state with a copy of
     *          the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new
     *                        axis labels. Strong throw guarantee.
     */
    CartesianSpace& operator=(const CartesianSpace& rhs) = default;

    /** @brief Replaces this instance's state with that of @p other.
     *
     *  This move assignment is the default move assignment. After this call the
     *  new instance will contain the same axis labels as @p other.
     *
     *  @param[in,out] other The CartesianSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *
     *  @return The current instance, after taking ownership of the state in
     *          @p rhs.
     *
     *  @throw None No throws guarantee.
     */
    CartesianSpace& operator=(CartesianSpace&& rhs) = default;

    /** @brief Function to access the entire axis label vector.
     *
     *  @return A reference of the axis label vector.
     *
     *  @throw None No throws guarantee.
     */
    const_label_reference get_axis_labels() const { return m_axis_vec_; }

    /** @brief Function to access the an axis label.
     *         With this function one may be able to set the label.
     *
     *  @param[in] i The index of the axis lable to be accessed.
     *
     *  @return The i-th axis label.
     *
     *  @throw std::out_of_range if the index is out of the range of the
     *         label vector.
     */
    label_type get_set_label(size_type i) { return m_axis_vec_.at(i); }

    /** @brief Function to access the an axis label.
     *         With this function one cannot set the label.
     *
     *  @param[in] i The index of the axis lable to be accessed.
     *
     *  @return The i-th axis label.
     *
     *  @throw std::out_of_range if the index is out of the range of the
     *         label vector.
     */
    label_type get_label(size_type i) const { return m_axis_vec_.at(i); }

protected:
    /** @brief Dimension of the cartesian space
     *
     *  @return The dimension.
     */
    size_type size_() const noexcept override { return m_N_; }

    bool equal_(const BaseSpace& rhs) const noexcept override {
        return this->equal_common(*this, rhs);
    }

    /** @brief To  clone the entire object.
     *
     *  @return A unique pointer of the cloned object.
     *
     *  @throw Throw if the clone action fail.
     */
    clone_type clone_() const override {
        return std::make_unique<CartesianSpace>(*this);
    }
};

/** @brief Comapres two CartesianSpace instances for equality.
 *
 *  Two CartesianSpace instances are equal if they have the same dimension.
 *
 *  @param[in] lhs The instance on the left of the equality.
 *  @param[in] rhs The instance on the right of the equality.
 *
 *  @return True if the CartesianSpace part of @p lhs compares equal to the
 * CartesianSpace part of @p rhs. False otherwise.
 *
 *  @throw Throws if comparing the base classes throws. Same throw
 *             guarantee.
 */
inline bool operator==(const CartesianSpace& lhs, const CartesianSpace& rhs) {
    if(lhs.get_axis_labels() != rhs.get_axis_labels())
        return false;
    else {
        const BaseSpace& lhs_base = lhs;
        const BaseSpace& rhs_base = rhs;
        return (lhs_base == rhs_base);
    }
}

} // namespace chemist::vector_space

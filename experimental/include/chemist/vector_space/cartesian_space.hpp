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
public:
    /// Type used for indexing and offsets
    using size_type  = typename BaseSpace::size_type;
    using label_type = std::string;

    /** @brief Default constructor to create R^0 with 0 dimension and no labels.
     *
     *  @throw None No throws guarantee.
     */
    CartesianSpace() = default;

    /** @brief Creates an N-dimensinal CartesianSpace with all the N axes set as
     * null. Users can set the labels by call the function label(i).
     *
     *  @param[in] N The dimension of the space.
     *
     *  @throw std::bad_alloc if changing of the capacity of the vector fails.
     */
    explicit CartesianSpace(size_type N) : BaseSpace(N) {}

    /** @brief Creates a CartesianSpace with axis labels being given. The
     * dimension is N and the labels are stored in a N-element vector of
     * strings.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @taram ItType The type of the begin and end iterator of the label
     *                vector to set up the axes. Default to be
     *                std::vector<std::string>::iterator>.
     *
     *  @param[in] begin_it The begin iterator of the label vector
     *             to set up the axes.
     *
     *  @param[in] end_it The end iterator of the label vector
     *             to set up the axes.
     *
     *  @throw std::invalid_argument if the length of string vector is not equal
     *                               to the dimension of the space.
     *
     *  @throw std::bad_alloc if the initialization of the string vector fails.
     */
    template<typename ItType = std::vector<std::string>::iterator>
    CartesianSpace(const size_type& N, ItType&& begin_it, ItType&& end_it) :
      BaseSpace(N, std::forward<ItType>(begin_it),
                std::forward<ItType>(end_it)) {}

    /** @brief Copy constructor. Copy another CartesianSpace.
     *
     *  @param[in] rhs The CartesianSpace to be copied.
     *
     *  @throw std::bad_alloc if the creation of the new object fails.
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
     *        may slice @p rhs.
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

protected:
    bool equal_(const BaseSpace& rhs) const noexcept override {
        return this->equal_common(*this, rhs);
    }

    /** @brief To  clone the entire object.
     *
     *  @return A unique pointer of the cloned object.
     *
     *  @throw Throw if the clone action fail.
     */
    base_pointer clone_() const override {
        return std::make_unique<CartesianSpace>(*this);
    }
};

/** @brief Comapres two CartesianSpace instances for equality.
 *
 *  Two CartesianSpace instances are equal if they have the same dimension and
 *  their labels are identical.
 *
 *  @param[in] lhs The instance on the left of the equality.
 *  @param[in] rhs The instance on the right of the equality.
 *
 *  @return True if the CartesianSpace part of @p lhs compares equal to the
 *          CartesianSpace part of @p rhs. False otherwise.
 *
 *  @throw Throws if comparing the base classes throws. Same throw
 *         guarantee.
 */
inline bool operator==(const CartesianSpace& lhs, const CartesianSpace& rhs) {
    const BaseSpace& lhs_base = lhs;
    const BaseSpace& rhs_base = rhs;
    return (lhs_base == rhs_base);
}

inline bool operator!=(const CartesianSpace& lhs, const CartesianSpace& rhs) {
    return !(lhs == rhs);
}

} // namespace chemist::vector_space

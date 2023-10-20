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
#include "chemist/orbital_space/base_space.hpp"

namespace chemist::orbital_space {

/** @brief Models a space spanned by cartesian axes (R^N).
 *
 *  R^N represents an N-dimensional space of cartesian axes x, y,
 *  and z. It can be used in calculating the components of properties
 *  depending on multiple positions, e. g., energy gradients.
 *
 *  @tparam N Dimension of the space.
 */
class CartesianSpace : public BaseSpace {
private:
    /// enum variable to label the x, y, z axes
    enum axes_type { 0, 1, 2 };
    axes_type axes;
    /// dimension of the space
    int m_N_;

public:
    /// Type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;

    /// array of enums representing the space
    axes_type axis_arr[m_N_];

    /** @brief Creates an N-dimensinal CartesianSpace.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @throw ??? Throws if initialization of the enum array throws.
     */
    CartesianSpace(int N) : m_N_(N){};

protected:
    /** @brief Dimension of the cartesian space
     *
     *  @return The dimension.
     */

    size_type size_() const noexcept override { return m_N_; }
    bool equal_(const BaseSpace& rhs) const noexcept override {
        return this->equal_common(*this, rhs);
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
 *  @throw ??? Throws if comparing the base classes throws. Same throw
 *             guarantee.
 */
inline bool operator==(const CartesianSpace& lhs, const CartesianSpace& rhs) {
    // Must have the same dimension
    return lhs.size() == rhs.size();
}

} // namespace chemist::orbital_space

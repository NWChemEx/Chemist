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
    enum axis_type { x, y, z };
    // axis_type axes;
    /// dimension of the space
    int m_N_;
    std::vector<int> m_val_;

public:
    /// Type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;

    /// array of enums representing the space
    std::vector<axis_type> axis_arr;

    /** @brief Creates an N-dimensinal CartesianSpace with axes not given.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @throw ??? Throws if initialization of the enum array throws.
     */
    CartesianSpace(const int N) : m_N_(N){};

    /** @brief Creates an N-dimensinal CartesianSpace with axes being given.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @param[in] val[] The label (0 -> x, 1 -> y and 2 -> z) array
     *                   to set up the axes
     *
     *  @throw ??? Throws if initialization of the enum array throws.
     */
    CartesianSpace(const int N, const std::vector<int> val) :
      m_N_(N), m_val_(val) {
        for(int i = 0; i < m_N_; i++) {
            axis_arr.push_back(static_cast<axis_type>(m_val_[i]));
        }
    };

    const char* EnumToStr(axis_type type) {
        switch(type) {
            case axis_type::x: return "x";
            case axis_type::y: return "y";
            case axis_type::z: return "z";
        }
    }

    // auto AxisType(int i) { return EnumToStr(static_cast<axis_type>(i));

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

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
#include "chemist/vector_space/base_space.hpp"
#include <string>

namespace chemist::vector_space {

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
    /// dimension of the space
    int m_N_;
    /// string vector to label the axes
    std::vector<std::string> m_val_;

public:
    /// Type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;

    /// array of enums representing the space
    std::vector<std::string> axis_arr;

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
     *  @throw ??? Throws if the length of string vector is not equal to the
     *             dimension of the space or initialization of the string vector
     *             throws.
     */
    CartesianSpace(const int N, const std::vector<std::string> val) :
      m_N_(N), m_val_(val) {
	if (m_val_.size() != m_N_) 
	   throw "Label vector length not equal to the dimension of the space!";
        for(int i = 0; i < m_N_; i++) {
            axis_arr.push_back(m_val_[i]);
        }
    };


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
    if (lhs.size() != rhs.size()) return false;
    else {
	if (lhs.axis_arr.empty() == true) {
		if (rhs.axis_arr.empty() == true) return true;
		else return false;
	}
	else if (lhs.axis_arr.empty() == true) return false;
	else {
		int len_l = lhs.axis_arr.size();
		int len_r = rhs.axis_arr.size();
		if (len_l != len_r) return false;
		else {
			for(int i=0;i<len_l;i++) {
				if (lhs.axis_arr[i] != rhs.axis_arr[i]) return false;
			}
			return true;
		}
	}		
    }
}

} // namespace chemist::vector_space


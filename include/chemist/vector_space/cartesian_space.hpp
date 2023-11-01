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
#include <vector>

namespace chemist::vector_space {

/** @brief Models a space spanned by cartesian axes (R^N).
 *
 *  R^N is the N-dimensional real space spanned by N orthogonal axes. 
 *  In practice, N is often 3, in which case the basis functions are usually 
 *  labeled "x", "y", and "z". However, the user may customize the number of 
 *  basis functions and their labels.
 *
 *  @tparam N Dimension of the space.
 */
class CartesianSpace : public BaseSpace {
private:
    /// dimension of the space
    BaseSpace::size_type m_N_;

public:
    /// Type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;

    /// array of enums representing the space
    using axis_label = std::vector<std::string>;
    axis_label axis_vec;

    /** @brief Creates an N-dimensinal CartesianSpace with axes not given.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @throw ??? No throws guarantee.
     */
    CartesianSpace(const unsigned int& N) : m_N_(N){};

    /** @brief Creates an N-dimensinal CartesianSpace with axes being given.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @param[in] val[] The label vector to set up the axes.
     *
     *  @throw ??? Throws if the length of string vector is not equal to the
     *             dimension of the space or initialization of the string vector
     *             throws.
     */
    CartesianSpace(const unsigned int& N, axis_label& val) :
      m_N_(N) {
	if (val.size() != m_N_) 
	   throw std::invalid_argument("Label vector length not equal to the" 
                 "dimension of the space!");
	for (std::vector<std::string>::iterator it=val.begin(); it<val.end(); it++) {
            axis_vec.push_back(*it);
        }
    };

    /** @brief Copy constructor. Copy another CartesianSpace.
     *
     *  @param[in] rhs The CartesianSpace to be copied.
     *
     *  @throw None No throw gurantee.
     */
    CartesianSpace(const CartesianSpace& rhs) : BaseSpace(rhs) {
    	this -> m_N_ = rhs.m_N_;
	this -> axis_vec = rhs.axis_vec;
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
         return (lhs.axis_vec.size() == rhs.axis_vec.size() && 
	         std::equal(lhs.axis_vec.begin(), lhs.axis_vec.end(), rhs.axis_vec.begin()));		
    }
}

} // namespace chemist::vector_space


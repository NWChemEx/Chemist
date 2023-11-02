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
 *
 *  @tparam N Dimension of the space.
 */
class CartesianSpace : public BaseSpace {
public:
    /// Type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;
    using label_type = std::string;
    using label_container = std::vector<label_type>;

    /** @brief Creates an N-dimensinal CartesianSpace with axes not given.
     *
     *  @param[in] N The dimension of the space.
     *
     *  @throw ??? No throws guarantee.
     */
    CartesianSpace(const unsigned int& N) : m_N_(N){}

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
template <typename ItType = std::vector<std::string>::iterator>
    CartesianSpace(const size_type& N, ItType& beginIt, ItType& endIt) : m_N_(N),
                   axis_vec(beginIt, endIt) {
       if(axis_vec.size() != m_N_)
            throw std::invalid_argument("Label vector length not equal to the"
                                        "dimension of the space!");
    }

template <typename ItType = std::vector<std::string>::iterator>    
    CartesianSpace(const size_type& N, ItType&& beginIt, ItType&& endIt) : m_N_(N), 
	           axis_vec(beginIt, endIt) {
       if(axis_vec.size() != m_N_)
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

    /** @brief Function to access the axis labels.
     *
     *  @param[in/out] labels The vector of the axis labels.
     *
     *  @throw Throws if vector asignment fails.
     */
     label_container* get_axis_label() const {
	label_container* labels = new label_container;
        (*labels).assign(axis_vec.begin(), axis_vec.end());
	return labels;
    }

protected:
    /** @brief Dimension of the cartesian space
     *
     *  @return The dimension.
     */

    size_type size_() const noexcept override { return m_N_; }
    bool equal_(const BaseSpace& rhs) const noexcept override {
        return this->equal_common(*this, rhs);
    }

private:
    /// dimension of the space
    size_type m_N_;
    /// vector of axis labels
    label_container axis_vec;
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
    if (*(lhs.get_axis_label())!= (*(rhs.get_axis_label()))) return false;
    else {
       const BaseSpace& lhs_base = lhs;
       const BaseSpace& rhs_base = rhs;
       return (lhs_base == rhs_base);
    }
}

// -----------------------------------------------------------------------------
// ----------------------------- Template Instantiations -----------------------
// -----------------------------------------------------------------------------
using label_itor = std::vector<std::string>::iterator;
template CartesianSpace::CartesianSpace<label_itor>(const size_type&, label_itor&, label_itor&);
template CartesianSpace::CartesianSpace<label_itor>(const size_type&, label_itor&&, label_itor&&);

} // namespace chemist::vector_space

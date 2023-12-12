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
#include <chemist/types.hpp>

namespace chemist::vector_space {

/** @brief Common base class for all vector spaces.
 *
 *  BaseSpace provides a generic API for working with a vector space. In
 *  particular this API provides members for:
 *  - the number of basis function via `size()`
 *  - the label of the i-th basis function via `label(i)`. This method would
 *    be implemented polymorphically in the derived classes.
 *  - polymorphically comparing the vector spaces `equal()` and `not_equal()`
 */
class BaseSpace {
public:
    /// Type used for indexing and offsets
    using size_type    = type::size;
    using base_pointer = std::unique_ptr<BaseSpace>;
    using label_type   = std::string;

    /// Default polymorphic dtor
    virtual ~BaseSpace() noexcept = default;

    /** @brief Returns the number of basis functions in this space.
     *
     *  The size of a vector space is the total number of basis functions in
     *  that space.
     *
     *  @return The number of basis functions in *this.
     *
     *  @throw None No throw guarantee.
     */
    auto size() const { return size_(); }

    /** @brief Clone the current object.
     *
     *  @return A deep (polymorphic) copy of *this.
     *
     *  @throw std::bad_alloc when making the unique pointer of the object
     * fails.
     */
    auto clone() const { return clone_(); }

    /** @brief Polymorphically compares two vector spaces to determine if they
     *         are equal.
     *
     *  Exactly what it means for two vector spaces to be equal depends on what
     *  the most derived classes of the spaces are. In general two vector
     *  spaces are equal if they:
     *  - have the same most derived class
     *    - e.g., a `DerivedSpace` instance and a `CanonicalSpace` compared
     *      through a common base class are not equal.
     *  - contain the same number of basis functions
     *  - the basis functions are ordered the same
     *  - the parameters associated with the basis functions are the same
     *
     *  Note that all floating-point comparisons are exact with no threshold
     *  tolerance. Meaning a parameter of 1.000000 is different than 1.000001.
     *
     *  @param[in] rhs The vector space we are comparing to.
     *
     *  @return True if this vector space is equivalent to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool equal(const BaseSpace& rhs) const noexcept;

    /** @brief Polymorphically compares two vector spaces to determine if they
     *         are different.
     *
     *  This function simply negates the result of `this->equal`. See `equal`
     *  for a description of how equality is defined.
     *
     *  @param[in] rhs The vector space we are comparing to.
     *
     *  @return False if *this is equivalent to @p rhs and true
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool not_equal(const BaseSpace& rhs) const noexcept { return !equal(rhs); }

    /** @brief Function to access the a basis funciton label.
     *         With this function one is able to set the label.
     *
     *  @param[in] i The index of the axis label to be accessed.
     *
     *  @return The i-th basis function label.
     *
     *  @throw std::out_of_range if the index is out of the range of the
     *         label vector.
     */
    label_type& label(size_type i) {
        if(i >= size())
            throw std::out_of_range("Index out of the range of"
                                    " the label vector.");
        else
            return m_labels_.at(i);
    }

    /** @brief Function to access the a basis funciton label.
     *         With this function one is NOT able to set the label.
     *
     *  @param[in] i The index of the axis label to be accessed.
     *
     *  @return The i-th basis function label.
     *
     *  @throw std::out_of_range if the index is out of the range of the
     *         label vector.
     */
    label_type label(size_type i) const {
        return (const_cast<BaseSpace&>(*this)).label(i);
    }

protected:
    /// Type of a container of mode indices
    using mode_container = std::vector<type::size>;

    /** @brief Creates a BaseSpace
     *
     *  Users will never directly create a BaseSpace instance because it is an
     *  abstract class. The ctor is made protected to make this more clear.
     *  BaseSpace has no state so this ctor is a no-op.
     *
     *  @throw None no throw guarantee.
     */
    BaseSpace() = default;

    /** @brief Creates an N-dimensinal BaseSpace with all basis function labels
     *  set as null. Users can set the labels by call the function label(i).
     *
     *  @param[in] N The dimension of the space.
     *
     *  @throw std::bad_alloc if changing of the capacity of the vector fails.
     */
    BaseSpace(size_type N) : m_size_(N), m_labels_(N, "") {}

    /** @brief Creates a BaseSpace with basis function labels being given. The
     *  dimension is N and the labels are stored in a N-element vector of
     *  strings.
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
    BaseSpace(const size_type& N, ItType&& begin_it, ItType&& end_it) :
      m_size_(N),
      m_labels_(std::forward<ItType>(begin_it), std::forward<ItType>(end_it)) {
        if(m_labels_.size() != m_size_)
            throw std::invalid_argument("Label vector length not equal to the"
                                        "dimension of the space!");
    }

    /** @brief Makes this `BaseSpace` instance by copying @p rhs.
     *
     *  The copy ctor is protected to avoid accidental slicing. `BaseSpace` has
     *  no state so the copy ctor is a no-op.
     *
     *  @param[in] rhs The instance being copied.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace(const BaseSpace& rhs) = default;

    /** @brief Makes this `BaseSpace` instance by taking state from @p rhs.
     *
     *  The move ctor is protected to avoid accidental slicing. `BaseSpace` has
     *  no state so the move ctor is a no-op.
     *
     *  @param[in,out] rhs The instance being moved from.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace(BaseSpace&& rhs) = default;

    /** @brief Replaces this instance's state with a copy of the state in
     *         @p rhs.
     *
     *  The copy assignment operator is protected to avoid accidental slicing.
     *  `BaseSpace` has no state so the move assignment is a no-op.
     *
     *  @param[in] rhs The instance being copied from.
     *
     *  @return The current instance, unchanged.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace& operator=(const BaseSpace& rhs) = default;

    /** @brief Replaces this instance's state by taking state from @p rhs.
     *
     *  The move assignment operator is protected to avoid accidental slicing.
     *  `BaseSpace` has no state so the move assignment is a no-op.
     *
     *  @param[in,out] rhs The instance being moved from.
     *
     *  @return The current instance, unchanged.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace& operator=(BaseSpace&& rhs) = default;

    /** @brief Implements polymorphic comparisons of vector spaces.
     *
     *  `equal_common` takes care of:
     *  - ensuring that this instance's derived type appears in @p rhs's class
     *    hierarchy (ensuring that it is also @p rhs's most derived type is
     *    accomplished by requiring equal to commute).
     *  - calling `DerivedType::operator==` to actually compare the
     *    `DerivedType` instances.
     *
     *  @tparam DerivedType The type of the class being compared to @p rhs.
     *          Assumed to be derived from `BaseSpace`.
     *
     *  @param[in] lhs When called from the derived class should be `*this`.
     *  @param[in] rhs The vector space we are compring to @p lhs.
     *
     *  @return True if @p rhs can be downcast to `DerivedType` and if the
     *          resulting `DerivedType` instance compares equal to @p lhs; false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename DerivedType>
    bool equal_common(const DerivedType& lhs,
                      const BaseSpace& rhs) const noexcept;

    /** @brief To be overridden by the derived class to implement size().
     *
     *  The derived class is responsible for ensuring this function returns the
     *  correct size for its respective space.
     *  - For AOSpace this is the number of AOs (radial plus angular, not just
     *    radial).
     *  - For DependentSpace this is the sum of the sizes of the vector spaces
     *    for each independent index.
     *  - For DerivedSpace derived directly from BaseSpace (as opposed to
     *    derived from `DependentSpace`) this is simply the number of orbitals.
     *  - For CartesianSpace this is just the dimension of the space.
     *  - For SpinSpace this is 2 (alpha and beta spin channel).
     *  - For ProductSpace this is the product of the sizes of the two spaces
     * which make the product.
     *
     *  @return The number of basis functions in the space.
     *
     *  @throw None No throw guarantee.
     */
    virtual size_type size_() const noexcept { return m_size_; };

    /** @brief To be overridden by the derived class to implement `equal`
     *
     *  Assuming that each derived class implements an appropriate `operator==`,
     *  `equal_` is implemented simply by calling the `equal_common` member:
     *
     *  @code
     *  bool equal_(const BaseType& rhs) const noexcept {
     *      return equal_common(*this, rhs);
     *  }
     *  @endcode
     *
     *  @note This little bit of boilerplate is required because otherwise, and
     *        without resorting to CRTP (thereby eliminating the common base
     *        class), BaseSpace does not know the type of the derived class.
     *
     *  @param[in] rhs The vector space we are comparing to.
     *
     *  @return True if this vector space is equivalent to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    virtual bool equal_(const BaseSpace& rhs) const noexcept = 0;

    /** @brief To be overridden in the derived vector space to implement the
     * clone method.
     *
     *  @return A unique pointer of the cloned object.
     *
     *  @throw  std::bad_alloc if the clone action fail in the derived vector
     * space.
     */
    virtual base_pointer clone_() const = 0;

    size_type m_size_;
    using label_container = std::vector<std::string>;
    label_container m_labels_; // vector of basis function labels
};

/** @brief Compares two BaseSpace instances for equality.
 *
 *  @relates BaseSpace
 *
 *  This operator compares all accessible state, even if that state isn't stored
 * in the base class. Point being this operator should account for the size.
 *
 *  @note This function is defined so that derived classes do not need to worry
 *        about whether they derive directly from BaseSpace.
 *
 *  @param[in] lhs The instance on the left of the equality operator.
 *  @param[in] rhs The instance on the right of the equality operator.
 *
 *  @return True if the BaseSpace instances are equal.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator==(const BaseSpace& lhs, const BaseSpace& rhs) {
    if(lhs.size() != rhs.size())
        return false;
    else {
        for(BaseSpace::size_type i = 0; i < lhs.size(); i++) {
            if(lhs.label(i) != rhs.label(i)) return false;
        }
        return true;
    }
}

/** @brief Determines if two BaseSpace instances are different.
 *
 *  @relates BaseSpace
 *
 *  This operator simply negates the result of `operator==`. See
 *  the documentation for `operator==` for the definition of
 *  equality.
 *
 *  @param[in] lhs The instance on the left of the inequality operator.
 *  @param[in] rhs The instance on the right of the inequality operator.
 *
 *  @return False If the two BaseSpace instances are different.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const BaseSpace& lhs, const BaseSpace& rhs) {
    return !(lhs == rhs);
}

//------------------------- Implementations -----------------------------------

inline bool BaseSpace::equal(const BaseSpace& rhs) const noexcept {
    return equal_(rhs) && rhs.equal_(*this);
}

template<typename DerivedType>
bool BaseSpace::equal_common(const DerivedType& lhs,
                             const BaseSpace& rhs) const noexcept {
    auto prhs = dynamic_cast<const DerivedType*>(&rhs);
    return prhs == nullptr ? false : lhs == *(prhs);
}

} // namespace chemist::vector_space

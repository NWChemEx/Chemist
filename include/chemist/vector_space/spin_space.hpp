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

namespace chemist::vector_space {

/** @brief Models a space spanned by the spin states of a system.
 *
 *  The common case is the space representing the alpha and beta spin channels
 *  of an electron. In general, if the total spin of the system is S, the
 *  dimension of the spin space is (2*S + 1) (multiplicity).
 *
 */
class SpinSpace : public BaseSpace {
public:
    /// Type used for indexing and offsets
    using size_type  = typename BaseSpace::size_type;
    using spin_type  = double;
    using label_type = typename BaseSpace::label_type;

    /** @brief Default ctor. Creates an empty SpinSpace with no spin channel (a
     * null object).
     *
     *  @throw None No throws guarantee.
     */
    SpinSpace() = default;

    /** @brief Ctor to create a SpinSpace to represent the spin channels of a
     *  system with the multiplicity as an input. Spin channel labels are
     * initialized as blank strings. For the case of 1-electron spin system, the
     * labels are defaulted as "alpha" and "beta". Users can set the labels by
     * call the function label(i).
     *
     *  @param[in] mult The multiplicity of the system (2S + 1).
     *
     *  @throw std::bad_alloc if changing of the capacity of the label vector
     * fails.
     */
    explicit SpinSpace(size_type mult) : BaseSpace(mult) {
        if(mult == 2) {
            m_labels_[0] = "alpha";
            m_labels_[1] = "beta";
        }
    }

    /** @brief Creates a SpinSpace with spin channel labels being given. The
     * dimension is mult and the labels are stored in a mult-element vector of
     * strings.
     *
     *  @param[in] mult The dimension of the space.
     *
     *  @taram ItType The type of the begin and end iterator of the
     *                vector to label the spin channels. Default to be
     *                std::vector<std::string>::iterator>.
     *
     *  @param[in] begin_it The begin iterator of the vector
     *             to label the spin channels.
     *
     *  @param[in] end_it The end iterator of the vector
     *             to label the spin channels.
     *
     *  @throw std::invalid_argument if the length of string vector is not equal
     *                               to the dimension of the space.
     *
     *  @throw std::bad_alloc if the initialization of the string vector fails.
     */
    template<typename ItType = std::vector<std::string>::iterator>
    SpinSpace(const size_type& mult, ItType&& begin_it, ItType&& end_it) :
      BaseSpace(mult, begin_it, end_it) {}

    /** @brief Copy constructor. Copy another SpinSpace.
     *
     *  @param[in] rhs The SpinSpace to be copied.
     *
     *  @throw None No throws guarantee.
     */
    SpinSpace(const SpinSpace& rhs) = default;

    /** @brief Initializes this instance by taking the state of @p other.
     *
     *  The move ctor is a standard move ctor. After this call the new instance
     *  will have the same dimension and contain the same spin channels
     *  as those of @p other.
     *
     *  @param[in,out] other The SpinSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *  @throw None No throws guarantee.
     */
    SpinSpace(SpinSpace&& other) = default;

    /** @brief Replaces the internal state with a copy of another SpinSpace
     *         instance's state.
     *
     *  @note This operator is not polymorphic, i.e. the resulting instance
     *        may slice @p rhs.
     *
     *  @param[in] rhs The SpinSpace instance we are copying
     *
     *  @return The current instance, after replacing its state with a copy of
     *          the state in @p rhs.
     *
     *  @throw None No throws guarantee.
     */
    SpinSpace& operator=(const SpinSpace& rhs) = default;

    /** @brief Replaces this instance's state with that of @p other.
     *
     *  This move assignment is the default move assignment. After this call the
     *  new instance will contain the same spin channels as @p other.
     *
     *  @param[in,out] other The SpinSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *
     *  @return The current instance, after taking ownership of the state in
     *          @p rhs.
     *
     *  @throw None No throws guarantee.
     */
    SpinSpace& operator=(SpinSpace&& rhs) = default;

    /** @brief Function to obtain the spin value of the i-th spin channel.
     *
     *  @param[in] i The index of the spin channel label.
     *
     *  @return The spin value of the i-th spin channel.
     *
     *  @throw std::out_of_range if the index is out of the range of the
     *         spin channels.
     */
    spin_type spin_channel(size_type i) const {
        if((i + 1) > size())
            throw std::out_of_range("Index out of the range of"
                                    " the spin channels.");
        else
            return (total_spin() - i);
    }

    /** @brief Get the total spin of the space.
     *
     *  @return The total spin.
     *
     *  @throw std::invalid_argument if multiplicity = 0.
     *
     */
    spin_type total_spin() const {
        if(m_size_ == 0)
            throw std::invalid_argument("No spin!");
        else
            return (spin_type(m_size_) - 1.0) / 2.0;
    }

protected:
    /** @brief Dimension of the spin space.
     *
     *  @return The dimension.
     *
     *  @throw None No throws guarantee.
     */
    size_type size_() const noexcept override { return m_size_; }

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
        return std::make_unique<SpinSpace>(*this);
    }
};

/** @brief Comapres two SpinSpace instances for equality.
 *
 *  Two SpinSpace instances are equal if they have the same dimension and
 *  identical spin channel labels.
 *
 *  @param[in] lhs The instance on the left of the equality.
 *  @param[in] rhs The instance on the right of the equality.
 *
 *  @return True if the SpinSpace part of @p lhs compares equal to the
 *          SpinSpace part of @p rhs. False otherwise.
 *
 *  @throw Throws if comparing the base classes throws. Same throw
 *         guarantee.
 */
inline bool operator==(const SpinSpace& lhs, const SpinSpace& rhs) {
    const BaseSpace& lhs_base = lhs;
    const BaseSpace& rhs_base = rhs;
    return (lhs_base == rhs_base);
}

inline bool operator!=(const SpinSpace& lhs, const SpinSpace& rhs) {
    return !(lhs == rhs);
}

} // namespace chemist::vector_space

#pragma once
#include "libchemist/types.hpp"
#include <utilities/iterators/random_access_iterator_base.hpp>
#include <utilities/mathematician/integer_utils.hpp>

namespace libchemist::basis_set::detail_ {

/** @brief Code factorization for making an iterator over a center, shell, etc.
 *
 *  This class implements an iterator that should work with any container that
 *  defines an `operator[](std::size_t)`. It works by holding an index and
 *  calling the subscript operator.
 *
 * @tparam ValueType The type returned by the subscript operator
 * @tparam ParentType The type of the container this will be an iterator for.
 *                    Assumed to be the `this` pointer of the class.
 */
template<typename ValueType, typename ParentType>
class BSIterator : public utilities::iterators::RandomAccessIteratorBase<
                     BSIterator<ValueType, ParentType>, ValueType> {
public:
    /// The type of this iterator
    using my_type = BSIterator<ValueType, ParentType>;
    /// The type of a difference between two iterators
    using difference_type = typename my_type::difference_type;

    /** @brief Makes an iterator that points at the specified element and is
     *         associated with the provided instance.
     *
     * @param[in] me The index this iterator is currently pointing at. Should
     *               be in range [0, size()] of the parent container.
     * @param[in] parent The container we are an iterator for. Should be the
     *                   this pointer of the container.
     *
     * @throw none no throw guarantee.
     */
    BSIterator(type::size me, ParentType parent) noexcept :
      m_index_(me),
      m_parent_(parent) {}

    /** @brief Advances the iterator by @p i iterations.
     *
     *  @param i The number of iterations to advance the iterator by. If
     * negative the iterator will move backwards.
     *  @return The current iterator advanced to the new position.
     *  @throw ??? If the parent's subscript operator throws at the new index.
     *             Strong throw guarantee.
     */
    my_type& advance(difference_type i) override {
        m_value_.emplace((*m_parent_)[m_index_ + i]);
        m_index_ += i;
        return *this;
    }

    /** @brief Computes the number of iterations to reach @p rhs.
     *
     *  @param[in] rhs The iterator you want the distance to.
     *  @return The number of iterations required to make this iterator equal
     *          to @p rhs. A negative value indicates that the present iterator
     *          is ahead of @p rhs.
     *  @throw none No throw guarantee.
     */
    difference_type distance_to(const my_type& rhs) const noexcept override {
        return utilities::UnsignedSubtract(rhs.m_index_, m_index_);
    }

    /** @brief Dreferences the iterator.
     *
     *
     *  @return the value currently being pointed at
     *  @throw ??? If dereferencing the current index throws. Strong throw
     *             guarantee.
     */
    ValueType& dereference() override {
        m_value_.emplace((*m_parent_)[m_index_]);
        return *m_value_;
    }

    /** @brief Determines if two iterators are the same.
     *
     *  Two iterators are defined as equal if they are pointing at the same
     *  object and associated with equal containers.
     *
     * @param[in] rhs The iterator to compare to.
     * @return True if the iterators are equal and false otherwise.
     * @throw none no throw guarantee.
     */
    bool are_equal(const my_type& rhs) const noexcept override {
        return m_index_ == rhs.m_index_ && (*m_parent_) == (*rhs.m_parent_);
    }

private:
    /// The index of the current value
    type::size m_index_;
    /// The container that owns this iterator
    ParentType m_parent_;
    /// The value currently being pointed at
    std::optional<ValueType> m_value_;
};

/** @brief Convenience function for making a bs_iterator
 *
 *  This function will deduce the `value_type` for the resulting iterator given
 *  the `this` pointer and the starting index.
 *
 * @tparam ParentType The type of the this pointer.
 * @param index Where the iterator starts at.
 * @param parent The container associated with this iterator.
 * @return The iterator consistent with pointing at element @p index of
 *         @p parent.
 * @throw none No throw guarantee.
 */
template<typename ParentType>
auto make_bs_iterator(type::size index, ParentType&& parent) noexcept {
    using value_type  = decltype(parent->operator[](index));
    using clean_value = std::remove_reference_t<value_type>;
    using parent_type = std::remove_reference_t<ParentType>;
    return BSIterator<clean_value, parent_type>(
      index, std::forward<ParentType>(parent));
}

} // namespace libchemist::basis_set::detail_

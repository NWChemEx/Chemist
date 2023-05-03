/*
 * Copyright 2023 NWChemEx-Project
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
#include <chemist/point/point_view2.hpp>
#include <memory>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename T>
class PointSetPIMPL;
}

/** @brief A container filled with Point objects
 *
 *  Conceptually this class behaves like a std::vector<Point<T>>; however, the
 *  backend is more complicated than that, which is why we wrote a new class.
 *
 *  @note Despite the name, this class will allow you to add the same point
 *        multiple times.
 *
 *  @tparam T The floating-point type used to store the coordinates of the
 *            points. Assumed to be either float or double.
 */
template<typename T>
class PointSet : public utilities::IndexableContainerBase<PointSet<T>> {
private:
    /// The type of the base class implementing the container API
    using base_type = utilities::IndexableContainerBase<PointSet<T>>;

public:
    /// The type of the PIMPL used to implement *this
    using pimpl_type = detail_::PointSetPIMPL<T>;

    /// The type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// The elements in the container
    using value_type = Point<T>;

    /// Read/write reference to an element
    using reference = PointView2<value_type>;

    /// Read-only reference to an element
    using const_reference = PointView2<const value_type>;

    /// Integral type used for indexing
    using size_type = typename base_type::size_type;

    /** @brief Creates an empty PointSet.
     *
     *  A default constructed PointSet behaves like an empty container (e.g.,
     *  think of an empty std::vector).
     *
     *  @throw None No throw guarantee.
     */
    PointSet() noexcept;

    /** @brief Creates a PointSet filled with provided points.
     *
     *  This ctor is intended primarily for testing and tutorials. It allows
     *  the user to initialize *this with the specified points.
     *
     *  @param[in] points A series of value_type instances to initialize the
     *                    PointSet with.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     *
     */
    explicit PointSet(std::initializer_list<value_type> points);

    /** @brief Creates a PointSet which is a deep copy of @p other.
     *
     *  @param[in] other The PointSet being copied.
     *
     *  @throw std::bad_alloc if there is a problem copying @p other. Strong
     *                        throw guarantee.
     */
    PointSet(const PointSet& other);

    /** @brief Creates a PointSet which owns @p other 's state
     *
     *  This ctor will create a new PointSet which will take ownership of the
     *  state in @p other. After construction all references to the state of
     *  @p other remain valid, but will now refer to state owned by *this.
     *
     *  @param[in,out] other The PointSet we are taking the state of. After
     *                       this operation @p other will be in a state
     *                       consistent with that of an empty PointSet.
     *
     *  @throw None No throw guarantee.
     */
    PointSet(PointSet&& other) noexcept;

    /** @brief Overrides *this's state with a deep copy of @p rhs.
     *
     *  This method will override *this's state (releasing it in the process)
     *  with a deep copy of @p rhs's state.
     *
     *  @param[in] rhs The PointSet we are copying.
     *
     *  @return *this after setting its state to a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state.
     */
    PointSet& operator=(const PointSet& rhs);

    /** @brief Overrides *this's sate with that of @p rhs.
     *
     *  This method will release that state in *this, and then replace it with
     *  the state in @p rhs. After this operation, all pointers and references
     *  to the state in @p rhs remain valid, except that they now refer to
     *  state inside *this.
     *
     *  @param[in,out] rhs The PointSet we are taking the state of. After this
     *                     operator @p rhs will be in a state consistent with
     *                     that of an empty PointSet.
     *
     *  @return *this after taking ownership of @p rhs's state.
     *
     *  @throw None No throw guarantee.
     */
    PointSet& operator=(PointSet&& rhs) noexcept;

    /// Defaulted dtor
    ~PointSet() noexcept;

    /** @brief Adds @p r to *this.
     *
     *  This method will set `(*this)[size()]` to @p r. In other words, if
     *  before calling this method *this had @f$n@f$ elements, then after this
     *  method *this will have @f$n + 1@f$ elements and the @f$n@f$-th element
     *  (0-based) will be @p r.
     *
     *  @param[in] r The point we are adding.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Weak
     *                        throw guarantee.
     */
    void push_back(value_type r);

    /** @brief Serialize PointSet instance
     *
     *  @param[in,out] ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserialize for PointSet instance
     *
     * @param[in,out] ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar);

private:
    friend base_type;

    /** @brief Creates a PointSet which uses an already created PIMPL.
     *
     *  This ctor is used internally to initialize the PointSet after
     *  creating a PIMPL.
     *
     *  @param[in] pimpl The PIMPL which will power *this.
     *
     *  @throw None No throw guarantee.
     */
    explicit PointSet(pimpl_pointer pimpl) noexcept;

    /// Used by base to implement non-const versions of at/operator[]
    reference at_(size_type i);

    /// Used by base to implement const versions of at/operator[]
    const_reference at_(size_type i) const;

    /// Used by base to implement size()
    size_type size_() const noexcept;

    /// Code factorization for determining if *this has a PIMPL already
    bool has_pimpl_() const noexcept;

    /// Actually implements the class
    pimpl_pointer m_pimpl_;
};

template<typename T>
template<typename Archive>
void PointSet<T>::save(Archive& ar) const {
    ar& this->size();
    if(this->size()) {
        for(const auto& x : *this) ar& x.as_point();
    }
}

template<typename T>
template<typename Archive>
void PointSet<T>::load(Archive& ar) {
    size_type size;
    ar& size;
    if(size) {
        value_type p;
        for(size_type _ = 0; _ < size; ++_) {
            ar& p;
            push_back(p);
        }
    }
}

extern template class PointSet<float>;
extern template class PointSet<double>;

} // namespace chemist

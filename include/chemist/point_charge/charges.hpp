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
#include <chemist/point/point_set.hpp>
#include <chemist/point_charge/point_charge_view.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename T>
class ChargesPIMPL;
}

/** @brief Represents a container of PointCharge objects.
 *
 *  Conceptually the Charges class behaves like a std::vector<PointCharge<T>>;
 *  however, the backend involves a fair amount of redirection for performance
 *  reasons.
 *
 *  @tparam T The floating-point type used to hold each PointCharge's charge.
 *            @p T will also be the type of the point charge's coordinates.
 */
template<typename T>
class Charges : public utilities::IndexableContainerBase<Charges<T>> {
private:
    /// Type implementing the container API of *this
    using base_type = utilities::IndexableContainerBase<Charges<T>>;

public:
    /// The type of the PIMPL
    using pimpl_type = detail_::ChargesPIMPL<T>;

    /// The type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    // -- PointCharge types ----------------------------------------------------

    /// The elements in the container
    using value_type = PointCharge<T>;

    /// Read/write reference to an element
    using reference = PointChargeView<value_type>;

    /// Read-only reference to an element
    using const_reference = PointChargeView<const value_type>;

    // -- Point types ----------------------------------------------------------

    /// The type used to store the coordinates
    using coord_type = typename value_type::coord_type;

    /// The type of the PointSet *this conceptually derives from
    using point_set_type = PointSet<coord_type>;

    /// Type of a read-only reference to the conceptual base class
    using const_point_set_reference = const point_set_type&;

    /// Integral type used for indexing
    using size_type = typename base_type::size_type;

    /** @brief Creates an empty Charges object.
     *
     *  The Charges object resulting from this ctor will function like an
     *  empty container.
     *
     *  @throw None No throw guarantee.
     */
    Charges() noexcept;

    /** @brief Initializes *this with the provided PointCharge objects.
     *
     *  This ctor is used primarily for unit testing and tutorials. It allows
     *  the user to initialize *this with a compile-time-known number of
     *  point charges.
     *
     *  @param[in] qs The PointCharges to initialize *this with.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory to
     *                        store the charges. Strong throw guarantee.
     */
    explicit Charges(std::initializer_list<value_type> qs);

    /** @brief Creates a new Charges by deep copying @p rhs.
     *
     *  @param[in] other the object we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        copy.
     */
    Charges(const Charges& other);

    /** @brief Creates a new Charges by taking the state of @p other.
     *
     *  This ctor creates a new Charges object which owns the state that was
     *  previously in @p other. All references to the state previously owned
     *  by @p other remain valid, except that they now point to state owned
     *  by *this.
     *
     *  @param[in,out] other The instance we are taking the state from. After
     *                       this operation @p other will be in a state
     *                       consistent with default initialization.
     *
     *  @throw None No throw guarantee
     */
    Charges(Charges&& other) noexcept;

    /** @brief Overrides the state of *this with a deep copy of @p rhs.
     *
     *  This method will release the state held by *this (invalidating any
     *  references to it in the process) and replace the internal state by a
     *  deep copy of the state in *this.
     *
     *  @param[in] rhs The object we are copying.
     *
     *  @return *this after setting its state to a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem copying @p rhs. Strong throw
     *                        guarantee.
     */
    Charges& operator=(const Charges& rhs);

    /** @brief Sets the state of *this to that in @p rhs.
     *
     *  This method will overwrite the state of *this (releasing it in the
     *  process) with the state found in @p rhs. After this operation all
     *  references to the state which was previously in @p rhs remain valid,
     *  but now refer to state found in *this.
     *
     *  @param[in,out] rhs The Charges we are taking the state of. After this
     *                     call @p rhs is in a state consistent with default
     *                     initialization.
     *
     *  @return *this after taking ownership of the state in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    Charges& operator=(Charges&& rhs) noexcept;

    /// Default dtor
    ~Charges() noexcept;

    /** @brief Adds @p q to *this
     *
     *  This method is used to add point charges to *this. If prior to this
     *  call *this was size @f$n@f$, after this call *this will be of size
     *  @f$n + 1 @f$ and the last, @f$n@f$-th element will be @p q.
     *
     *  @param[in] q The point charge to add to *this.
     *
     *  @throw std::bad_alloc if there is a problem copying @p q. Weak throw
     *                        guarantee.
     */
    void push_back(value_type q);

    /** @brief Serialize Charges instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserialize for Charges instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar);

private:
    /// Allows the base class to access at_ and size_
    friend base_type;

    /// Used by base to implement retrieving elements by read/write reference
    reference at_(size_type i);

    /// Used by base to implement retrieving elements by read-only reference
    const_reference at_(size_type i) const;

    /// Used by base to determine the number of elements in *this
    size_type size_() const noexcept;

    /// Wraps determining if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// The object actually implementing *this
    pimpl_pointer m_pimpl_;
};

template<typename T>
template<typename Archive>
void Charges<T>::save(Archive& ar) const {
    ar& this->size();
    if(this->size()) {
        for(const auto& x : *this) ar& x.as_point_charge();
    }
}

template<typename T>
template<typename Archive>
void Charges<T>::load(Archive& ar) {
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

extern template class Charges<float>;
extern template class Charges<double>;

} // namespace chemist

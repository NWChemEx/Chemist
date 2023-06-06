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
#include <chemist/nucleus/nucleus_view.hpp>
#include <chemist/point_charge/charges.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
class NucleiPIMPL;
} // namespace detail_

/** @brief Represents a container of Nucleus objects.
 *
 *  Conceptually the Nuclei class behaves like a std::vector<Nucleus<T>>;
 *  however, the backend involves a fair amount of redirection for performance
 *  reasons.
 */
class Nuclei : public utilities::IndexableContainerBase<Nuclei> {
private:
    /// Type implementing the container API of *this
    using base_type = utilities::IndexableContainerBase<Nuclei>;

public:
    /// The type of the PIMPL
    using pimpl_type = detail_::NucleiPIMPL;

    /// The type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    // -- Nucleus types ----------------------------------------------------

    /// The elements in the container
    using value_type = Nucleus;

    /// Read/write reference to an element
    using reference = NucleusView<value_type>;

    /// Read-only reference to an element
    using const_reference = NucleusView<const value_type>;

    /// -- PointCharge types ---------------------------------------------------

    /// The type used to store the charge
    using charge_type = typename value_type::charge_type;

    /// The type of the Charges *this conceptually derives from
    using charge_set_type = Charges<charge_type>;

    /// Type of a read-only reference to the conceptual base class
    using const_charge_set_reference = const charge_set_type&;

    /// Integral type used for indexing
    using size_type = typename base_type::size_type;

    /** @brief Creates an empty Nuclei object.
     *
     *  The Nuclei object resulting from this ctor will function like an
     *  empty container.
     *
     *  @throw None No throw guarantee.
     */
    Nuclei() noexcept;

    /** @brief Initializes *this with the provided Nucleus objects.
     *
     *  This ctor is used primarily for unit testing and tutorials. It allows
     *  the user to initialize *this with a compile-time-known number of
     *  point Nuclei.
     *
     *  @param[in] qs The nuclei to initialize *this with.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory to
     *                        store the Nucleus objects. Strong throw guarantee.
     */
    explicit Nuclei(std::initializer_list<value_type> qs);

    /** @brief Creates a new Nuclei by deep copying @p rhs.
     *
     *  @param[in] other the object we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        copy.
     */
    Nuclei(const Nuclei& other);

    /** @brief Creates a new Nuclei by taking the state of @p other.
     *
     *  This ctor creates a new Nuclei object which owns the state that was
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
    Nuclei(Nuclei&& other) noexcept;

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
    Nuclei& operator=(const Nuclei& rhs);

    /** @brief Sets the state of *this to that in @p rhs.
     *
     *  This method will overwrite the state of *this (releasing it in the
     *  process) with the state found in @p rhs. After this operation all
     *  references to the state which was previously in @p rhs remain valid,
     *  but now refer to state found in *this.
     *
     *  @param[in,out] rhs The Nuclei we are taking the state of. After this
     *                     call @p rhs is in a state consistent with default
     *                     initialization.
     *
     *  @return *this after taking ownership of the state in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    Nuclei& operator=(Nuclei&& rhs) noexcept;

    /// Default dtor
    ~Nuclei() noexcept;

    /** @brief Adds @p q to *this
     *
     *  This method is used to add point nuclei to *this. If prior to this
     *  call *this was size @f$n@f$, after this call *this will be of size
     *  @f$n + 1 @f$ and the last, @f$n@f$-th element will be @p q.
     *
     *  @param[in] q The point nucleus to add to *this.
     *
     *  @throw std::bad_alloc if there is a problem copying @p q. Weak throw
     *                        guarantee.
     */
    void push_back(value_type q);

    /** @brief Serialize Nuclei instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserialize for Nuclei instance
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

template<typename Archive>
void Nuclei::save(Archive& ar) const {
    ar& size();
    if(size()) {
        for(const auto& x : *this) ar& x.as_nucleus();
    }
}

template<typename Archive>
void Nuclei::load(Archive& ar) {
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

} // namespace chemist

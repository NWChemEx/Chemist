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
#include <chemist/chemical_system/nucleus/nucleus_view.hpp>
#include <chemist/chemical_system/point_charge/charges.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
class NucleiPIMPL;
} // namespace detail_

/** @brief Represents a container of Nucleus objects.
 *
 *  Conceptually the Nuclei class behaves like a std::vector<Nucleus<T>>;
 *  however, the backend can involve a fair amount of redirection for
 *  performance reasons (e.g., ensuring contiguous storage).
 */
class Nuclei : public utilities::IndexableContainerBase<Nuclei> {
private:
    /// Type of *this
    using my_type = Nuclei;

    /// Type implementing the container API of *this
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    /// The type of the PIMPL
    using pimpl_type = detail_::NucleiPIMPL;

    /// The type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Class defining types associated with *this
    using nuclei_traits = ChemistClassTraits<my_type>;

    // -- Nucleus types ----------------------------------------------------

    /// Class defining the types of the elements in *this
    using nucleus_traits = typename nuclei_traits::nucleus_traits;

    /// The elements in the container
    using value_type = typename nucleus_traits::value_type;

    /// Read/write reference to an element
    using reference = typename nucleus_traits::view_type;

    /// Read-only reference to an element
    using const_reference = typename nucleus_traits::const_view_type;

    /// Type of a mutable pointer to a Nucleus's name
    using name_pointer = typename nucleus_traits::name_pointer;

    // -- PointCharge types

    // Class defining the types of the Charges subset of *this
    using charges_traits = typename nuclei_traits::charges_traits;

    // Class defining the types of PointCharge piece of each Nucleus
    using point_charge_traits = typename nucleus_traits::point_charge_traits;

    /// The type used to store the charge
    using charge_type = typename point_charge_traits::charge_;

    /// The type of the Charges *this conceptually derives from
    using charge_set_type = typename charges_traits::value_type;

    /// Type of a mutable reference to the Charges piece of *this
    using charge_set_reference = typename charges_traits::view_type;

    /// Type of a read-only reference to the Charges piece of *this
    using const_charge_set_reference = typename charges_traits::const_view_type;

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

    /** @brief Creates a Nuclei object from a pair of iterators.
     *
     *  @tparam BeginItr The type of the iterator pointing to the first Nucleus.
     *  @tparam EndItr   The type of the iterator pointing to just past the
     *                   last Nucleus.
     *
     *  This constructor will add all of the Nucleus objects in the range
     *  [begin, end()) to *this.
     *
     *  @param[in] begin An iterator pointing to the first Nucleus.
     *  @param[in] end   An iterator pointing to just past the last Nucleus
     *
     *  @throw std::bad_alloc if there is a problem allocating the internal
     *                        state of *this. Strong throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    Nuclei(BeginItr&& begin, EndItr&& end) : Nuclei() {
        while(begin != end) {
            push_back(*begin);
            ++begin;
        }
    }

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

    charge_set_reference charges();

    const_charge_set_reference charges() const;

    name_pointer names_data() noexcept;

    const_name_pointer names_data() const noexcept;

    atomic_number_pointer atomic_number_data() noexcept;

    const_atomic_number_pointer atomic_number_data() const noexcept;

    mass_pointer mass_data() noexcept;

    const_mass_pointer mass_data() const noexcept;

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

/**
 * @relates Nuclei
 * @brief Overloads the stream insertion operator so that nuclei can be
 *        printed.
 *
 * @param os The ostream instance to write to.
 * @param nuclei The Nuclei instance to write to the stream.
 * @return The ostream instance containing the text representation of the
 *         nuclei.
 * @throws std::ios_base::failure if anything goes wrong while writing.  Weak
 *         throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const Nuclei& nuclei);

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

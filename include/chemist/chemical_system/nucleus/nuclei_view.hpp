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
#include <chemist/chemical_system/nucleus/nuclei.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename NucleiType>
class NucleiViewPIMPL;

} // namespace detail_

/** @brief Allows existing state to be used as if it were a Nuclei object.
 *
 *  Nuclei objects own their state. The NucleiView class aliases existing state,
 *  but allows the user to interact with the aliased state as if it were in a
 *  Nuclei object.
 *
 *  @tparam NucleiType
 */
template<typename NucleiType>
class NucleiView
  : public utilities::IndexableContainerBase<NucleiView<NucleiType>> {
private:
    /// Type of *this
    using my_type = NucleiView<NucleiType>;

    /// Type of the base
    using base_type = utilities::IndexableContainerBase<my_type>;

    /// Traits type helping out with the TMP
    using traits_type = detail_::ViewTraits<NucleiType>;

    static constexpr bool is_const = traits_type::is_const_v;

    /// Typedef so we don't need "typename" and "template" for references
    template<typename U>
    using apply_const_ref = typename traits_type::template apply_const_ref<U>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::NucleiViewPIMPL<NucleiType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type used for indexing and offsets
    using size_type = typename base_type::size_type;

    // -- Nuclei/Nucleus types -------------------------------------------------

    /// Type *this is behaving like
    using nuclei_type = typename traits_type::type;

    /// Type of an element in *this
    using value_type = typename nuclei_type::value_type;

    /// Mutable reference to an element in *this
    using reference =
      std::conditional_t<is_const, typename nuclei_type::reference,
                         typename nuclei_type::const_reference>;

    /// Read-only reference to an element in *this
    using const_reference = typename nuclei_type::const_reference;

    // -- PointCharge types ----------------------------------------------------

    /// Type used to store the charge
    using charge_type = typename nuclei_type::charge_type;

    // -- Ctors, Assignment, and dtor ------------------------------------------

    /** @brief Creates a view to an empty Nuclei object.
     *
     *  @throw None No throw guarantee.
     */
    NucleiView() noexcept;

    NucleiView(apply_const_ref<nuclei_type> nuclei);

    /** @brief Creates a new view powered by the provided PIMPL.
     *
     *  @param[in] pimpl A pointer to the istance's state. If @p pimpl is null
     *             then the resulting view is the same as if it had been
     *             default initialized.
     *
     *  @throw None No throw guarantee.
     */
    explicit NucleiView(pimpl_pointer pimpl) noexcept;

    /** @brief Makes *this a view of the same Nuclei as @p other.
     *
     *  This ctor will make a new NucleiView which is a deep copy of @p other.
     *  In the context of views, a deep copy entails aliasing the same Nuclei
     *  object, but doing so through new state. In other words, after this
     *  constructor is called the resulting object will behave like a reference
     *  to the same Nuclei object as @p other, but *this can be made to
     *  reference a different Nuclei object independent of @p other.
     *
     *  @param[in] other The view we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state for
     *         *this. Strong throw guarantee.
     */
    NucleiView(const NucleiView& other);

    /** @brief Transfers ownership of @p other to *this.
     *
     *  This ctor will move the state in @p other into a newly created object.
     *
     *  @param[in,out] other The object whose state is being transferred to
     *                       make *this. After this operation @p other will be
     *                       a view of an empty Nuclei object.
     *
     * @throw None No throw guarantee.
     */
    NucleiView(NucleiView&& other) noexcept;

    /** @brief Overwrites the state in *this with a copy of @p rhs.
     *
     *  This function makes a deep copy of @p rhs via the copy ctor and then
     *  overwrites the state of *this with that deep copy. See the copy ctor's
     *  documentation for more details.
     *
     *  @param[in] rhs The view we are copying.
     *
     *  @return *this after overwriting its state with a copy of @p rhs.
     *
     *  @throw std::bad_alloc if the copy ctor throws. Strong throw guarantee.
     */
    NucleiView& operator=(const NucleiView& rhs);

    /** @brief Overwrites the state in *this with the state in @p rhs.
     *
     *  This function will release the state contained in *this and transfer it
     *  to *this.
     *
     *  @param[in,out] rhs The view we are taking the state from. After this
     *                     function @p rhs will be a view of an empty Nuclei
     *                     object.
     *
     *  @throw None No throw guarantee.
     */
    NucleiView& operator=(NucleiView&& rhs) noexcept;

    /// Default no throw dtor
    ~NucleiView() noexcept;

    /** @brief Exchanges the state of *this with @p other.
     *
     *  This function will "swap" the state of *this with the state in @p other.
     *  Thus, after this call *this will contain the state which was previously
     *  in @p other and @p other will contain the state which previously was in
     *  *this.
     *
     *  @param[in,out] other The view to swap state with. After this call
     *                       @p other will contain the state which was in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(NucleiView& other) noexcept;

    /** @brief Determines if *this is value equal to an existing Nuclei object.
     *
     *  *This method compares the Nuclei object *this is a view of to the
     *   provided Nuclei object. The Nuclei objects are compared for value
     *   equality as defined by Nuclei::operator==.
     *
     *   @param[in] rhs The Nuclei object to compare to.
     *
     *   @return True if *this compares value equal to @p rhs and false
     *           otherwise.
     *
     *   @throw None No throw guarantee.
     */
    bool operator==(const nuclei_type& rhs) const noexcept;

    /** @brief Determines if *this differs from an existing Nuclei object.
     *
     *  The NucleiView class defines different as not value equal. Value equal
     *  is defined by the corresponding operator==. This method simply negates
     *  the result of calling operator==.
     *
     *  @param[in] rhs The Nuclei object to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const nuclei_type& rhs) const noexcept;

private:
    /// Allows the base class to access at_ and size_
    friend base_type;

    /// Implements retrieving read/write references
    reference at_(size_type i);

    /// Implements retrieving read-only references
    const_reference at_(size_type i) const;

    /// Used by base to determine the number of elements in *this
    size_type size_() const noexcept;

    /// Wraps determining if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// The object implementing *this
    pimpl_pointer m_pimpl_;
};

/** @brief Compares a Nuclei object to a NucleiView object.
 *
 *  @relates NucleiView
 *
 *  This function simply calls NucleiView::operator==() on lhs. See the
 *  Nuclei overload of NucleiView::operator==() for more information.
 *
 *  @param[in] lhs The Nuclei object being compared to @p rhs.
 *  @param[in] rhs The NucleiView object being compared @p lhs.
 *
 *  @return True if @p lhs is equal to @p rhs and false otherwise.
 *
 *  @throw None No throw guarantee
 */
template<typename NucleiType>
inline bool operator==(const typename NucleiView<NucleiType>::nuclei_type& lhs,
                       const NucleiView<NucleiType>& rhs) noexcept {
    return rhs == lhs;
}

/** @brief Determines if a Nuclei object is different than the the Nuclei
 *         object @p rhs is a view of.
 *
 *  @relates NucleiView
 *
 *  This function is defined so that comparisons between a Nuclei and a
 *  NucleiView object can be done symmetrically. In practice this function
 *  simply calls NucleiView::operator!= on @p lhs. See the documentation of the
 *  Nuclei overload of NucleiView::operator!= for more details.
 *
 *  @param[in] lhs The Nuclei object being compared to @p rhs.
 *  @param[in] rhs The NucleiView object being compared to @p lhs.
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename NucleiType>
inline bool operator!=(const typename NucleiView<NucleiType>::nuclei_type& lhs,
                       const NucleiView<NucleiType>& rhs) noexcept {
    return rhs != lhs;
}

/** @brief Allows a NucleiView object to be printed akin to a Nuclei object.
 *
 *  @relates NucleiView
 *
 *  This method allows NucleiView objects to be printed.
 *
 *  @param[in,out] os The stream we are printing to. After the operation @p os
 *                    will contain a textual representation of @p nuclei.
 *  @param[in] nuclei The NucleiView to print.
 *  @return Returns @p os, after adding @p nuclei to it.
 *  @throw std::ios_base::failure if anything goes wrong while writing. Weak
 *                                throw guarantee.
 */
template<typename NucleiType>
std::ostream& operator<<(std::ostream& os,
                         const NucleiView<NucleiType>& nuclei) {
    for(const auto& nuke_i : nuclei) os << nuke_i << std::endl;
    return os;
}

extern template class NucleiView<Nuclei>;
extern template class NucleiView<const Nuclei>;

} // namespace chemist

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
#include <chemist/fragmenting/capping/cap.hpp>
#include <set>
#include <utilities/containers/indexable_container_base.hpp>
#include <vector>

namespace chemist::fragmenting {

/** @brief The set of caps associated with a fragment.
 *
 *  When a fragment of a molecule severs covalent bonds, those bonds must be
 *  capped. This class is used to track the caps. For design details see:
 *  https://nwchemex.github.io/Chemist/developer/design/chemistry/caps.html
 */
class CapSet : public utilities::IndexableContainerBase<CapSet> {
private:
    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<CapSet>;

public:
    /// Type of each cap
    using value_type = Cap;

    /// Type of a mutable reference to a cap
    using reference = value_type&;

    /// Type of a read-only reference to a cap
    using const_reference = const value_type&;

    /// Type of a nucleus
    using nucleus_type = Nucleus;

    /// Type of a mutable reference to a set of cap nuclei
    using nuclei_reference = NucleiView<Nuclei>;

    /// Type of a read-only reference to a set of cap nuclei
    using const_nuclei_reference = NucleiView<const Nuclei>;

    /// Type of a const reference to a nucleus
    using const_nucleus_reference = value_type::const_reference;

    /// Type used for indexing and offsets by this class. Unsigned integral.
    using size_type = typename base_type::size_type;

    /// Type used to specify a set of indices
    using index_set_type = std::set<size_type>;

    /** @brief Creates ane empty CapSet.
     *
     *  The CapSet resulting from the default ctor has no caps in it. Caps can
     *  be added by calling either push_back or add_cap.
     *
     *  @throw None No throw guarantee.
     */
    CapSet() = default;

    /** @brief Creates a CapSet initialized to the values in @p il.
     *
     *  This ctor is intended primarily for unit testing and tutorials as it
     *  requires you to initialize *this with a compile-time known number of
     *  caps.
     *
     *  @param[in] il The caps to initialize *this with.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        initial state. Strong throw guarantee.
     */
    explicit CapSet(std::initializer_list<value_type> il) : m_caps_(il) {}

    /** @brief Creates a CapSet from a range of Cap objects.
     *
     *  @tparam BeginItr Type of the iterator pointing to the first cap.
     *  @tparam EndItr Type of the iterator pointing to just past the last
     *                 cap.
     *
     *  This ctor is used when the user already has the Caps in a container,
     *  or when the caps are being created by a generator object. The resulting
     *  CapSet object will contain the Cap objects in the range [begin, end()).
     *
     *  @param[in,out] begin The iterator pointing to the first cap. After this
     *                       ctor has run @p begin will be equal to @p end.
     *  @param[in,out] end The iterator pointing to just past the last cap.
     *                     This ctor will not modify @p end.
     *
     *  @throw std::bad_alloc if there is a problem allocating the internal
     *                        state. Strong throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    CapSet(BeginItr&& begin, EndItr&& end) :
      m_caps_(std::forward<BeginItr>(begin), std::forward<EndItr>(end)) {}

    /** @brief Adds @p cap to *this.
     *
     *  This method can be used to add an already created Cap instance to
     *  *this. The cap is appended on to the end of *this. If prior to calling
     *  this method *this has @f$n@f$ caps. After a successful invocation of
     *  this method, *this will have @f$n + 1@f$ caps and @f$(*this)[n]@f$ will
     *  be @p cap.
     *
     *  @param[in] cap The value being added to *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for
     *                        storing @p cap. Strong throw guarantee.
     */
    void push_back(value_type cap) { m_caps_.emplace_back(std::move(cap)); }

    /** @brief Used to construct a cap in place.
     *
     *  @tparam Args The types of the atoms comprising the caps. Must be
     *               implicitly convertible to atom_type or to the type of
     *               an iterator over atom_type objects.
     *
     *  @param[in] anchor The index of atom the cap is attached to.
     *  @param[in] replaced The index of the atom the cap is replacing.
     *  @param[in] atoms The atoms comprising the cap or a pair of iterators.
     *
     *
     *  @throw std::bad_alloc if there is a problem creating the cap or adding
     *                        it to *this. Strong throw guarantee.
     */
    template<typename... Args>
    void emplace_back(size_type anchor, size_type replaced, Args&&... atoms) {
        push_back(value_type(anchor, replaced, std::forward<Args>(atoms)...));
    }

    /** @brief Retrieves the set of caps for a set of indices.
     *
     *  @tparam BeingItr The type of the iterator pointing to the first index.
     *  @tparam EndItr   The type of the iterator pointing to just past the last
     *                   index.
     *
     *  This method fills in an object of type `index_set_type` from the
     *  provided iterators and then dispatches to get_caps(index_set_type).
     *  See the description for get_caps(index_set_type) for more details.
     *
     *  @param[in,out] begin An iterator pointing to the first index. After
     *                       this method is called @p begin will be equal to
     *                       @p end.
     *  @param[in,out] end   An iterator pointing to just past the last index.
     *                       This method does not change @p end.
     *
     *  @return A container with the indices of the caps needed to cap the
     *          fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Weak throw guarantee because @p begin will be in
     *                        a valid, but otherwise undefined state.
     */
    template<typename BeginItr, typename EndItr>
    index_set_type get_cap_indices(BeginItr&& begin, EndItr&& end) const {
        return get_cap_indices(index_set_type(std::forward<BeginItr>(begin),
                                              std::forward<EndItr>(end)));
    }

    /** @brief Retrieves the set of cap indices for a set fragment indices.
     *
     *  Typically fragments are specified by stating the indices of the elements
     *  which  are in the fragment. If any of those indices is an anchor index
     *  and the corresponding replaced index is not in the fragment, then a
     *  bond has been broken. This method returns the indices of the caps
     *  needed for the input fragment.
     *
     *  @param[in] fragment_indices Indices for the elements which are in the
     *                              fragment.
     *  @return A container with the indices of the caps needed for the
     * fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    index_set_type get_cap_indices(
      const index_set_type& fragment_indices) const;

    /** @brief Returns the nuclei needed to cap the input fragment.
     *
     *  @tparam BeginItr The type of the iterator that points to the index of
     *                   the first nucleus.
     *  @tparam EndItr   The type of the iterator that points to just past the
     *                   index of the last nucleus.
     *
     *  This method uses the provided iterators to initialize a container of
     *  type index_set_type. The resulting container is then forwarded to
     *  get_cap_nuclei(const index_set_type&). See the documentation of
     *  get_cap_nuclei(const index_set_type&) for more details.
     *
     *  @param[in,out] begin The iterator which points to the index of the
     *                       first nucleus in the fragment. After this method
     *                       is called @p begin will equal @p end.
     *  @param[in,out] end The iterator which points to just past the index of
     *                     the last nucleus in the fragment. This method will
     *                     leave @p end unchanged.
     *
     *  @return A mutable view of a Nuclei object containing the nuclei needed
     *          to cap the input fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return. Weak
     *                        throw guarantee as @p begin will be in a valid,
     *                        but otherwise undefined state.
     */
    template<typename BeginItr, typename EndItr>
    nuclei_reference get_cap_nuclei(BeginItr&& begin, EndItr&& end) {
        return get_cap_nuclei(index_set_type(std::forward<BeginItr>(begin),
                                             std::forward<EndItr>(end)));
    }

    /** @brief Returns the nuclei needed to cap @p fragment_indices.
     *
     *  Given a set of indices corresponding to the nuclei in the fragment,
     *  @p fragment_indices, this method will find the caps needed for the
     *  fragment, combine the nuclei into a Nuclei object, and return a view
     *  of the Nuclei object.
     *
     *  @param[in] fragment_indices The indices of the nuclei in the fragment.
     *
     *  @return A mutable view of a Nuclei object containing the nuclei needed
     *          to cap the input fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    nuclei_reference get_cap_nuclei(const index_set_type& fragment_indices);

    /** @brief Returns the nuclei needed to cap the input fragment.
     *
     *  @tparam BeginItr The type of the iterator that points to the index of
     *                   the first nucleus.
     *  @tparam EndItr   The type of the iterator that points to just past the
     *                   index of the last nucleus.
     *
     *  This method is the same as the non-const version except that the
     *  resulting view is read-only.
     *
     *  @param[in,out] begin The iterator which points to the index of the
     *                       first nucleus in the fragment. After this method
     *                       is called @p begin will equal @p end.
     *  @param[in,out] end The iterator which points to just past the index of
     *                     the last nucleus in the fragment. This method will
     *                     leave @p end unchanged.
     *
     *  @return A read-only view of a Nuclei object containing the nuclei needed
     *          to cap the input fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return. Weak
     *                        throw guarantee as @p begin will be in a valid,
     *                        but otherwise undefined state.
     */
    template<typename BeginItr, typename EndItr>
    const_nuclei_reference get_cap_nuclei(BeginItr&& begin,
                                          EndItr&& end) const {
        return get_cap_nuclei(index_set_type(std::forward<BeginItr>(begin),
                                             std::forward<EndItr>(end)));
    }

    /** @brief Returns the nuclei needed to cap @p fragment_indices.
     *
     *  This method is the same as the non-const version except that the
     *  resulting view is read-only.
     *
     *  @param[in] fragment_indices The indices of the nuclei in the fragment.
     *
     *  @return A read-only view of a Nuclei object containing the nuclei needed
     *          to cap the input fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    const_nuclei_reference get_cap_nuclei(
      const index_set_type& fragment_indices) const;

private:
    /// Allows the base to implement *this via CRTP
    friend base_type;

    /// Container the caps are stored in
    using cap_set = std::vector<value_type>;

    /// Used to implement operator[]/at
    reference at_(size_type i) noexcept { return m_caps_[i]; }

    /// Used to implement operator[] const/at const
    const_reference at_(size_type i) const noexcept { return m_caps_[i]; }

    /// Used to implement size()
    size_type size_() const noexcept { return m_caps_.size(); }

    /// The actual caps in this set
    cap_set m_caps_;
};

} // namespace chemist::fragmenting

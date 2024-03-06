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
#include <chemist/chemical_system/nucleus/nuclei_view.hpp>
#include <chemist/fragmenting/detail_/fragmented_base.hpp>

namespace chemist::fragmenting {
namespace detail_ {
class FragmentedNucleiPIMPL;
}

/** @brief Class describing super-/sub-set relationship among sets of `Nuclei`
 *         objects.
 *
 *  See https://tinyurl.com/3ukrf6t8 for design details.
 *
 */
class FragmentedNuclei : public FragmentingBase<FragmentedNuclei> {
private:
    /// Type *this inherits from
    using base_type = FragmentingBase<FragmentedNuclei>;

    /// Type of the pimpl
    using pimpl_type = detail_::FragmentedNucleiPIMPL;

public:
    /// Type of both the fragments and the supersystem
    using value_type = Nuclei;

    /// Type of a mutable reference to a fragment
    using reference = NucleiView<value_type>;

    /// Type of a read-only reference to a fragment
    using const_reference = NucleiView<const value_type>;

    /// Type of a reference to a fragment returned by non-const *this, will be
    /// read-only.
    using reference = typename impl_type::const_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename impl_type::const_reference;

    /// Type used for referring to nuclei indices, unsigned integral type
    using index_type = size_type;

    /// Type used for referring to fragments by index, unsigned integral type
    using size_type = base_type::size_type;

    // -------------------------------------------------------------------------
    // --- Ctors, Assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a null FragmentedNuclei object.
     *
     *  The default FragmentedNuclei object is not associated with a
     *  supersystem. Notably, this is considered a different state than a
     *  FragmentedNuclei object which is associated with an empty supersystem.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei() noexcept;

    /** @brief Creates a FragmentedNuclei object associated with @p supersystem
     *
     *  Each fragment in a FragmentedNuclei is a subset of the same supersystem.
     *  This ctor is used to create a FragmentedNuclei object which will
     *  contain subsets of @p supersystem.
     *
     *  @param[in] supersystem The supersystem we are fragmenting.
     *
     *  @throw std::bad_alloc if there is a problem creating the internal state.
     *                        Strong throw guarantee.
     */
    explicit FragmentedNuclei(supersystem_type supersystem);

    FragmentedNuclei(const FragmentedNuclei& other);
    FragmentedNuclei& operator=(const FragmentedNuclei& rhs);
    FragmentedNuclei(FragmentedNuclei&& other) noexcept;
    FragmentedNuclei& operator=(FragmentedNuclei&& rhs) noexcept;

    /// Defaulted no-throw dtor
    ~FragmentedNuclei() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters/Setters
    // -------------------------------------------------------------------------

    /** @brief Adds a fragment given an initializer list of indices.
     *
     *  This method is used primarily in unit testing, but it allows you to
     *  add fragments to *this by providing an initializer list of atom
     * indices.
     *
     *  For example, to add a fragment containing atoms 0, 4, and 5 (indicies
     *  refer to offsets in the object returned by `this->supersystem()`)
     *  to *this, the syntax is:
     *
     *  @code
     *  add_fragment({0, 4, 5});
     *  @endcode
     *
     *  @param[in] il An initializer list containing the atom indices which
     *                should be in the fragment.
     *
     *  @throw std::runtime_error if *this does not contain a supersystem.
     *                            Strong throw guarantee.
     *  @throw std::out_of_range if any of the indices in @p il are not in the
     *                           range [0, supersystem().size()). Strong throw
     *                           guarantee.
     *  @throw std::bad_alloc if there is a problem creating the fragment.
     *                        Strong throw guarantee.
     */
    void add_fragment(std::initializer_list<index_type> il);

    /** @brief Adds a fragment given iterators to a range of indices.
     *
     *  Fragments in general contain more than one atom, thus creation of
     *  a fragment typically requires specifying more than one index. In the
     *  usual C++ manner, this method accepts two iterators which can be used
     *  to obtain the indices for this fragment.
     *
     *  For example, to add a fragment containing atoms 0, 4, and 5 (indicies
     *  refer to offsets in the object returned by `this->supersystem()`)
     *  to *this, the syntax is:
     *
     *  @code
     *  std::vector<FragmentedNuclei::size_type> indices{0, 4, 5};
     *  add_fragment(indices.begin(), indices.end());
     *  @endcode
     *
     *  @param[in, out] b An iterator which points to the index of the first
     *                    atom in the fragment. @p b will only be dereferenced
     *                    while `b != e` is true. If @p b is passed by reference
     *                    `b == e` will be true upon return.
     *  @param[in] e An iterator which points to just past the last index of
     *               the last atom in the fragment. @p e will not be
     *               dereferenced.
     *
     *  @throw std::runtime_error if *this does not contain a supersystem.
     *                            Strong throw guarantee.
     *  @throw std::out_of_range if any of the indices returned by `*b` are not
     *                           in the range [0, supersystem().size()). Strong
     *                           throw guarantee.
     *  @throw std::bad_alloc if there is a problem creating the fragment.
     *                        Strong throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    void add_fragment(BeginItr&& b, EndItr&& e) {
        auto s = m_frags_.new_subset();
        for(; b != e; ++b) s.insert(*b);
        m_frags_.insert(std::move(s));
    }

    /// Does *this have a supersystem?
    bool has_supersystem() const noexcept {
        return &m_frags_.object() != nullptr;
    }

    /** @brief Provides access to the supersystem.
     *
     *  If *this has a supersystem associated with it, this method can be used
     *  to access the supersystem.
     *
     *  @returns A read-only reference to the supersystem *this is fragmenting.
     *
     *  @throw std::runtime_error if *this is not associated with a supersystem.
     *                            Strong throw guarantee.
     */

    const_supersystem_reference supersystem() const {
        if(!has_supersystem())
            throw std::runtime_error("Supersystem was not set!");
        return m_frags_.object();
    }

    void swap(FragmentedNuclei& other) noexcept;

    /** @brief Determines if two FragmentedNuclei instances are equal.
     *
     *  In order for two FragmentedNuclei instances to be considered equal they
     *  both must either contain a supersystem or they both must not contain a
     *  supersystem. If they both contain a supersystem, then additionally they
     *  both must contain the same supersystem as well as containing the same
     *  fragments of that supersystem.
     *
     *  @param[in] rhs The object we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FragmentedNuclei& rhs) const noexcept {
        return m_frags_ == rhs.m_frags_;
    }

    /** @brief Determines if two FragmentedNuclei instances are different.
     *
     *  Two FragmentedNuclei instances are different if they are not value
     *  equal. In practice this means that this method simply negates
     *  operator==. See the documentation for operator== for the criteria used
     *  to determine if the *this is value equal to @p rhs.
     *
     *  @param[in] rhs The object *this is being compared to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const FragmentedNuclei& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Allows the base class to access at_/size_ for the CRTP
    friend base_type;

    /// Implements non-const versions of operator[]/at
    reference at_(size_type i) noexcept { return m_frags_[i]; }

    /// Implements const versions of operator[]/at
    const_reference at_(size_type i) const noexcept { return m_frags_[i]; }

    /// Implements size
    size_type size_() const noexcept { return m_frags_.size(); }

    /// Object actually managing the sets
    impl_type m_frags_;
};

} // namespace chemist::fragmenting

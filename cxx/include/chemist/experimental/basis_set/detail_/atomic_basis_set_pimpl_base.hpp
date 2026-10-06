/*
 * Copyright 2026 NWChemEx-Project
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
#include <chemist/experimental/basis_set/ao_shell_base_view.hpp>
#include <chemist/experimental/basis_set/ao_shell_enums.hpp>
#include <chemist/experimental/basis_set/primitive_view.hpp>
#include <chemist/experimental/point/point_view.hpp>
#include <chemist/experimental/traits/atomic_basis_set_traits.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace chemist::experimental::detail_ {

/** @brief The interface every implementation of AtomicBasisSet satisfies.
 *
 *  An atomic basis set is a set of shells sharing one center. Per
 *  docs/source/developer/design/basis_set/ao_hierarchy.rst, the type of a
 *  shell encodes its purity and its AO ordering, but AtomicBasisSet is not
 *  templated on it; instead it holds one of these, selected at runtime from a
 *  ShellPurity and an AOOrdering. Every set therefore holds shells of exactly
 *  one type, and *this exposes them polymorphically, as AOShellBaseView
 *  objects.
 *
 *  The derived classes are AtomicBasisSetPIMPL, which owns the state, and
 *  AtomicBasisSetViewPIMPL, which aliases state owned by something else. The
 *  same AtomicBasisSetPIMPLCommon implements every hook below for both. The
 *  state of a set is:
 *
 *  - one array of coefficients and one of exponents, holding every primitive
 *    of every shell back to back, shell-major,
 *  - the angular momentum of each shell,
 *  - the offsets of the shells into the primitive arrays,
 *  - the center, the name of the basis set, and the atomic number.
 *
 *  No shell owns any of this; indexing builds a view of the shell from it.
 *
 *  Following AOShellBaseCommon, the public members are non-virtual. They check
 *  their arguments and then call the corresponding unchecked `*_` hook.
 *  Anything which follows from other members, e.g. n_aos(), is computed here
 *  rather than being a hook.
 */
class AtomicBasisSetPIMPLBase {
private:
    /// Types of the mutable atomic basis set
    using traits_type = ChemistClassTraits<AtomicBasisSet>;

public:
    /// Type of a pointer to an implementation
    using pimpl_pointer = typename traits_type::pimpl_pointer;

    /// Pull in the types of the mutable atomic basis set
    ///@{
    using angular_momentum_type = typename traits_type::angular_momentum_type;
    using atomic_number_reference =
      typename traits_type::atomic_number_reference;
    using atomic_number_type     = typename traits_type::atomic_number_type;
    using buffer_reference       = typename traits_type::buffer_reference;
    using center_reference       = typename traits_type::center_reference;
    using const_buffer_reference = typename traits_type::const_buffer_reference;
    using const_atomic_number_reference =
      typename traits_type::const_atomic_number_reference;
    using const_center_reference = typename traits_type::const_center_reference;
    using const_name_reference   = typename traits_type::const_name_reference;
    using const_primitive_reference =
      typename traits_type::const_primitive_reference;
    using const_shell_view_reference =
      typename traits_type::const_shell_view_reference;
    using name_reference      = typename traits_type::name_reference;
    using name_type           = typename traits_type::name_type;
    using primitive_reference = typename traits_type::primitive_reference;
    using range_type          = typename traits_type::range_type;
    using shell_pointer       = typename traits_type::shell_pointer;
    using size_type           = typename traits_type::size_type;
    ///@}

    /// Polymorphic, no-throw dtor
    virtual ~AtomicBasisSetPIMPLBase() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Shell type
    // -------------------------------------------------------------------------

    /// Returns the purity shared by every shell in *this
    ShellPurity purity() const noexcept { return purity_(); }

    /// Returns the AO ordering shared by every shell in *this
    AOOrdering ordering() const noexcept { return ordering_(); }

    // -------------------------------------------------------------------------
    // -- Shells
    // -------------------------------------------------------------------------

    /// Returns the number of shells in *this
    size_type size() const noexcept { return size_(); }

    /** @brief Returns a read-only view of the shell at offset @p i.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the view. Strong
     *                        throw guarantee.
     */
    shell_pointer at(size_type i) const {
        check_shell_(i);
        return at_(i);
    }

    /** @brief Returns the angular momentum of the shell at offset @p i.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     */
    angular_momentum_type get_l(size_type i) const {
        check_shell_(i);
        return l_(i);
    }

    /** @brief Sets the angular momentum of the shell at offset @p i.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     *  @throw std::logic_error if *this aliases read-only state. Strong throw
     *                          guarantee.
     */
    void set_l(size_type i, angular_momentum_type l) {
        check_shell_(i);
        set_l_(i, l);
    }

    /** @brief Returns the total number of AOs in *this.
     *
     *  This is the sum of the shell sizes, which follow from their angular
     *  momenta and the purity exactly as in AOShellBaseCommon::size.
     *
     *  @throw None No throw guarantee.
     */
    size_type n_aos() const noexcept {
        const bool pure = purity() == ShellPurity::pure;
        size_type n     = 0;
        for(size_type i = 0; i < size(); ++i) {
            const auto l = l_(i);
            n += pure ? 2 * l + 1 : (l + 1) * (l + 2) / 2;
        }
        return n;
    }

    /** @brief Adds a shell with angular momentum @p l and the primitives
     *         given by @p coefficients and @p exponents.
     *
     *  @throw std::invalid_argument if @p coefficients and @p exponents are
     *                               not the same length. Strong throw
     *                               guarantee.
     *  @throw std::runtime_error if *this aliases its state, which can not be
     *                            resized, or if the parameters are not of the
     *                            same concrete floating-point type as the
     *                            ones already in *this. Strong throw
     *                            guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    void add_shell(angular_momentum_type l, const_buffer_reference coefficients,
                   const_buffer_reference exponents) {
        if(coefficients.size() != exponents.size())
            throw std::invalid_argument(
              "chemist::experimental::AtomicBasisSet: the coefficient and "
              "exponent arrays must be the same length.");
        add_shell_(l, std::move(coefficients), std::move(exponents));
    }

    /** @brief Adds a copy of @p shell to *this.
     *
     *  Only the angular momentum and the primitives of @p shell are copied;
     *  the center of the new shell is the center of *this.
     *
     *  @throw std::invalid_argument if @p shell's purity, ordering, or center
     *                               differs from that of *this. Strong throw
     *                               guarantee.
     *  @throw std::runtime_error under the same conditions as add_shell.
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    void push_back(const_shell_view_reference shell) {
        const bool pure = purity() == ShellPurity::pure;
        if(shell.is_pure() != pure)
            throw_bad_shell_("its purity differs from that of the set");
        if(!is_same_ordering_(shell))
            throw_bad_shell_("its AO ordering differs from that of the set");
        if(shell.get_center() != std::as_const(*this).center())
            throw_bad_shell_("its center differs from that of the set");
        auto cg = shell.get_contracted_gaussian();
        add_shell(shell.get_l(), cg.get_coefficient_buffer(),
                  cg.get_exponent_buffer());
    }

    // -------------------------------------------------------------------------
    // -- Primitives
    // -------------------------------------------------------------------------

    /// Returns the total number of primitives in *this
    size_type n_primitives() const noexcept { return n_primitives_(); }

    /** @brief Returns the offsets of the primitives of shell @p i.
     *
     *  @return The half-open range [first, second) of offsets, into the
     *          flattened primitives of *this, of shell @p i's primitives.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     */
    range_type primitive_range(size_type i) const {
        check_shell_(i);
        return primitive_range_(i);
    }

    /** @brief Returns the offset of the shell primitive @p i belongs to.
     *
     *  @throw std::out_of_range if @p i is not in [0, n_primitives()). Strong
     *                           throw guarantee.
     */
    size_type primitive_to_shell(size_type i) const {
        check_primitive_(i);
        return primitive_to_shell_(i);
    }

    /** @brief Returns a view of the primitive at offset @p i.
     *
     *  @throw std::out_of_range if @p i is not in [0, n_primitives()). Strong
     *                           throw guarantee.
     *  @throw std::logic_error if the mutable overload is called and *this
     *                          aliases read-only state. Strong throw
     *                          guarantee.
     */
    ///@{
    primitive_reference primitive(size_type i) {
        check_primitive_(i);
        return primitive_(i);
    }
    const_primitive_reference primitive(size_type i) const {
        check_primitive_(i);
        return primitive_(i);
    }
    ///@}

    /** @brief Returns the array of every coefficient/exponent in *this.
     *
     *  @throw std::logic_error if the mutable overload is called and *this
     *                          aliases read-only state. Strong throw
     *                          guarantee.
     */
    ///@{
    buffer_reference coefficient_buffer() { return coefficient_buffer_(); }
    const_buffer_reference coefficient_buffer() const {
        return coefficient_buffer_();
    }
    buffer_reference exponent_buffer() { return exponent_buffer_(); }
    const_buffer_reference exponent_buffer() const {
        return exponent_buffer_();
    }
    ///@}

    // -------------------------------------------------------------------------
    // -- Per-center state
    // -------------------------------------------------------------------------

    /** @brief Returns the center, name, and atomic number of *this.
     *
     *  @throw std::logic_error if a mutable overload is called and *this
     *                          aliases read-only state. Strong throw
     *                          guarantee.
     */
    ///@{
    center_reference center() { return center_(); }
    const_center_reference center() const { return center_(); }
    name_reference name() { return name_(); }
    const_name_reference name() const { return name_(); }
    atomic_number_reference atomic_number() { return atomic_number_(); }
    const_atomic_number_reference atomic_number() const {
        return atomic_number_();
    }
    ///@}

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a copy of *this.
     *
     *  The copy is deep if *this owns its state and shallow (i.e. another
     *  alias of the same state) if it does not.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    pimpl_pointer clone() const { return clone_(); }

    /** @brief Returns an implementation aliasing the state of *this.
     *
     *  @throw std::logic_error if as_view is called and *this aliases
     *                          read-only state. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the result.
     *                        Strong throw guarantee.
     */
    ///@{
    pimpl_pointer as_view() { return as_view_(); }
    pimpl_pointer as_const_view() const { return as_const_view_(); }
    ///@}

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two implementations are value equal if they have the same purity,
     *  ordering, name, atomic number, and center, the same number of shells,
     *  and their shells are pairwise value equal. Whether either owns its
     *  state is not considered.
     *
     *  @throw std::bad_alloc if there is a problem allocating the shell views
     *                        compared. Strong throw guarantee.
     */
    bool are_equal(const AtomicBasisSetPIMPLBase& rhs) const {
        if(purity() != rhs.purity() || ordering() != rhs.ordering())
            return false;
        if(name() != rhs.name() || atomic_number() != rhs.atomic_number())
            return false;
        if(center() != rhs.center() || size() != rhs.size()) return false;
        for(size_type i = 0; i < size(); ++i)
            if(!at_(i)->are_equal(*rhs.at_(i))) return false;
        return true;
    }

protected:
    /// Only the derived classes should be creating/copying *this
    ///@{
    AtomicBasisSetPIMPLBase() noexcept                               = default;
    AtomicBasisSetPIMPLBase(const AtomicBasisSetPIMPLBase&) noexcept = default;
    AtomicBasisSetPIMPLBase(AtomicBasisSetPIMPLBase&&) noexcept      = default;
    AtomicBasisSetPIMPLBase& operator=(
      const AtomicBasisSetPIMPLBase&) noexcept = default;
    AtomicBasisSetPIMPLBase& operator=(AtomicBasisSetPIMPLBase&&) noexcept =
      default;
    ///@}

    /// Hooks implementing the public API. Offsets have already been checked.
    ///@{
    virtual ShellPurity purity_() const noexcept                           = 0;
    virtual AOOrdering ordering_() const noexcept                          = 0;
    virtual size_type size_() const noexcept                               = 0;
    virtual shell_pointer at_(size_type i) const                           = 0;
    virtual angular_momentum_type l_(size_type i) const noexcept           = 0;
    virtual void set_l_(size_type i, angular_momentum_type l)              = 0;
    virtual bool is_same_ordering_(const_shell_view_reference shell) const = 0;
    virtual void add_shell_(angular_momentum_type l,
                            const_buffer_reference coefficients,
                            const_buffer_reference exponents)              = 0;
    virtual size_type n_primitives_() const noexcept                       = 0;
    virtual range_type primitive_range_(size_type i) const noexcept        = 0;
    virtual size_type primitive_to_shell_(size_type i) const noexcept      = 0;
    virtual primitive_reference primitive_(size_type i)                    = 0;
    virtual const_primitive_reference primitive_(size_type i) const        = 0;
    virtual buffer_reference coefficient_buffer_()                         = 0;
    virtual const_buffer_reference coefficient_buffer_() const             = 0;
    virtual buffer_reference exponent_buffer_()                            = 0;
    virtual const_buffer_reference exponent_buffer_() const                = 0;
    virtual center_reference center_()                                     = 0;
    virtual const_center_reference center_() const                         = 0;
    virtual name_reference name_()                                         = 0;
    virtual const_name_reference name_() const                             = 0;
    virtual atomic_number_reference atomic_number_()                       = 0;
    virtual const_atomic_number_reference atomic_number_() const           = 0;
    virtual pimpl_pointer clone_() const                                   = 0;
    virtual pimpl_pointer as_view_()                                       = 0;
    virtual pimpl_pointer as_const_view_() const                           = 0;
    ///@}

private:
    /// Throws std::out_of_range if @p i is not a valid shell offset
    void check_shell_(size_type i) const {
        if(i < size()) return;
        throw std::out_of_range(
          "chemist::experimental::AtomicBasisSet: shell offset " +
          std::to_string(i) + " is out of range for a set with " +
          std::to_string(size()) + " shells.");
    }

    /// Throws std::out_of_range if @p i is not a valid primitive offset
    void check_primitive_(size_type i) const {
        if(i < n_primitives()) return;
        throw std::out_of_range(
          "chemist::experimental::AtomicBasisSet: primitive offset " +
          std::to_string(i) + " is out of range for a set with " +
          std::to_string(n_primitives()) + " primitives.");
    }

    /// Throws std::invalid_argument explaining why a shell was rejected
    [[noreturn]] static void throw_bad_shell_(const std::string& why) {
        throw std::invalid_argument(
          "chemist::experimental::AtomicBasisSet: can not add the shell "
          "because " +
          why + ".");
    }
};

} // namespace chemist::experimental::detail_

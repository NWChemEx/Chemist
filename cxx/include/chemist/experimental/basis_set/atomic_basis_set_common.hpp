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
#include <chemist/experimental/basis_set/ao_shell_enums.hpp>
#include <chemist/experimental/basis_set/detail_/atomic_basis_set_pimpl_base.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/experimental/detail_/float_serialization.hpp>
#include <chemist/experimental/traits/atomic_basis_set_traits.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace chemist::experimental {

/** @brief Implements the API shared by AtomicBasisSet and AtomicBasisSetView.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, an atomic
 *  basis set is a set of shells sharing one center, together with the name
 *  of the basis set and the atomic number of the atom it is for. Every shell
 *  in a set has the same type, i.e. the same purity and AO ordering, but which
 *  type that is is chosen at runtime (see ShellPurity and AOOrdering), so the
 *  shells are handed out polymorphically, as AOShellBaseView objects.
 *
 *  None of the shells exist as objects inside the set. The set stores the
 *  parameters of all of its shells contiguously, together with the angular
 *  momentum of each shell and the offsets of each shell into the parameters.
 *  Indexing builds a view of the requested shell from that state, so there is
 *  exactly one copy of each parameter, and of the center, however the set is
 *  accessed. The flattened state is also available directly, as the
 *  primitives of the set (primitive, n_primitives, ...) or as the raw
 *  parameter arrays (get_coefficient_buffer, get_exponent_buffer).
 *
 *  Both AtomicBasisSet and AtomicBasisSetView are implemented by an object
 *  deriving from detail_::AtomicBasisSetPIMPLBase. *this implements the API
 *  in terms of that object, which the derived class supplies via `pimpl_()`.
 *
 *  @tparam DerivedType The class deriving from *this. Must define `pimpl_()`
 *                      and declare every AtomicBasisSetCommon a friend.
 *  @tparam AtomicBasisSetType The, possibly const-qualified, AtomicBasisSet
 *                             the derived class models. Its
 *                             const-qualification decides whether the setters
 *                             exist.
 */
template<typename DerivedType, typename AtomicBasisSetType>
class AtomicBasisSetCommon {
private:
    /// Struct defining the types for the set *this acts like
    using traits_type = ChemistClassTraits<AtomicBasisSetType>;

    /// Is the state *this models read-only?
    static constexpr bool is_const = std::is_const_v<AtomicBasisSetType>;

    /// Enables a method only when *this can mutate the state
    template<typename U, typename ReturnType = void>
    using enable_if_mutable_t =
      std::enable_if_t<!std::is_const_v<U>, ReturnType>;

    /// Lets *this compare against, and assign from, other sets
    template<typename OtherDerived, typename OtherType>
    friend class AtomicBasisSetCommon;

public:
    /// Pull in the types of the set *this acts like
    ///@{
    using angular_momentum_type = typename traits_type::angular_momentum_type;
    using atomic_number_reference =
      typename traits_type::atomic_number_reference;
    using atomic_number_type = typename traits_type::atomic_number_type;
    using buffer_reference   = typename traits_type::buffer_reference;
    using buffer_type        = typename traits_type::buffer_type;
    using center_reference   = typename traits_type::center_reference;
    using center_type        = typename traits_type::center_type;
    using const_atomic_number_reference =
      typename traits_type::const_atomic_number_reference;
    using const_buffer_reference = typename traits_type::const_buffer_reference;
    using const_center_reference = typename traits_type::const_center_reference;
    using const_name_reference   = typename traits_type::const_name_reference;
    using const_primitive_reference =
      typename traits_type::const_primitive_reference;
    using const_shell_reference = typename traits_type::const_shell_reference;
    using const_shell_view_reference =
      typename traits_type::const_shell_view_reference;
    using name_type           = typename traits_type::name_type;
    using pimpl_pointer       = typename traits_type::pimpl_pointer;
    using pimpl_type          = typename traits_type::pimpl_type;
    using primitive_reference = typename traits_type::primitive_reference;
    using range_type          = typename traits_type::range_type;
    using shell_pointer       = typename traits_type::shell_pointer;
    using size_type           = typename traits_type::size_type;
    ///@}

    // -------------------------------------------------------------------------
    // -- Per-center state
    // -------------------------------------------------------------------------

    /** @brief Returns the name of the basis set, e.g. "cc-pVDZ".
     *
     *  @throw None No throw guarantee.
     */
    const_name_reference get_name() const { return pimpl_().name(); }

    /** @brief Returns the atomic number of the atom *this is for.
     *
     *  @throw None No throw guarantee.
     */
    const_atomic_number_reference get_atomic_number() const {
        return pimpl_().atomic_number();
    }

    /** @brief Returns the point every shell in *this is centered on.
     *
     *  This is the one copy of the center; every shell and primitive obtained
     *  from *this aliases it. The mutable overload therefore moves every
     *  shell at once when written through. When *this models a read-only
     *  set, both overloads return a read-only view.
     *
     *  @throw std::bad_alloc if there is a problem allocating the view.
     *                        Strong throw guarantee.
     */
    ///@{
    center_reference get_center() {
        if constexpr(is_const) {
            return std::as_const(pimpl_()).center();
        } else {
            return pimpl_().center();
        }
    }
    const_center_reference get_center() const { return pimpl_().center(); }
    ///@}

    /** @brief Sets the name of the basis set.
     *
     *  When *this models a read-only set this method does not participate in
     *  overload resolution.
     *
     *  @throw std::bad_alloc if there is a problem copying @p name. Strong
     *                        throw guarantee.
     */
    template<typename U = AtomicBasisSetType>
    enable_if_mutable_t<U> set_name(name_type name) {
        pimpl_().name() = std::move(name);
    }

    /** @brief Sets the atomic number.
     *
     *  When *this models a read-only set this method does not participate in
     *  overload resolution.
     *
     *  @throw None No throw guarantee.
     */
    template<typename U = AtomicBasisSetType>
    enable_if_mutable_t<U> set_atomic_number(atomic_number_type z) {
        pimpl_().atomic_number() = z;
    }

    /** @brief Moves the center, and with it every shell, to @p center.
     *
     *  When *this models a read-only set this method does not participate in
     *  overload resolution.
     *
     *  @tparam PointType The type of @p center. May be a Point or a view of
     *                    one.
     *
     *  @throw std::runtime_error if @p center's coordinates do not hold the
     *                            same concrete floating-point type as the
     *                            center of *this. Weak throw guarantee.
     */
    template<typename PointType, typename U = AtomicBasisSetType>
    enable_if_mutable_t<U> set_center(const PointType& center) {
        pimpl_().center() = center;
    }

    // -------------------------------------------------------------------------
    // -- Shells
    // -------------------------------------------------------------------------

    /// Returns the purity shared by every shell in *this
    ShellPurity purity() const noexcept { return pimpl_().purity(); }

    /// Returns the AO ordering shared by every shell in *this
    AOOrdering ordering() const noexcept { return pimpl_().ordering(); }

    /// Are the shells of *this pure (spherical)?
    bool is_pure() const noexcept { return purity() == ShellPurity::pure; }

    /// Are the shells of *this Cartesian? Same as `!is_pure()`.
    bool is_cartesian() const noexcept { return !is_pure(); }

    /// Returns the number of shells in *this
    size_type size() const noexcept { return pimpl_().size(); }

    /// Does *this have no shells?
    bool empty() const noexcept { return size() == 0; }

    /** @brief Returns the shell at offset @p i.
     *
     *  The shell is built on demand and the caller owns the returned pointer,
     *  but the shell aliases the state of *this: it is not a copy, writes to
     *  *this are visible through it, and it must not outlive *this. Anything
     *  which changes the size of the parameter arrays (adding a shell, swap,
     *  assignment, deserialization) invalidates it, much like an iterator into
     *  a std::vector.
     *
     *  The result is a read-only view of the concrete shell type selected by
     *  purity() and ordering(), e.g. a CCAShellView<const
     * CCAShell<CartesianAO>> for a Cartesian set in CCA order. Mutate the
     * shells through *this, e.g. set_l and primitive, rather than through the
     * shell.
     *
     *  @param[in] i The offset of the shell. Must be in [0, size()).
     *
     *  @return A pointer to a newly allocated, read-only view of the shell.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the view. Strong
     *                        throw guarantee.
     */
    shell_pointer at(size_type i) const { return pimpl_().at(i); }

    /// Same as at()
    shell_pointer operator[](size_type i) const { return at(i); }

    /** @brief Returns the angular momentum of shell @p i.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     */
    angular_momentum_type get_l(size_type i) const { return pimpl_().get_l(i); }

    /** @brief Sets the angular momentum of shell @p i to @p l.
     *
     *  When *this models a read-only set this method does not participate in
     *  overload resolution.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     */
    template<typename U = AtomicBasisSetType>
    enable_if_mutable_t<U> set_l(size_type i, angular_momentum_type l) {
        pimpl_().set_l(i, l);
    }

    /** @brief Returns the number of AOs in *this.
     *
     *  This is the sum of the sizes of the shells.
     *
     *  @throw None No throw guarantee.
     */
    size_type n_aos() const noexcept { return pimpl_().n_aos(); }

    // -------------------------------------------------------------------------
    // -- Primitives
    // -------------------------------------------------------------------------

    /// Returns the number of primitives in *this, across every shell
    size_type n_primitives() const noexcept { return pimpl_().n_primitives(); }

    /** @brief Returns the offsets of the primitives of shell @p i.
     *
     *  @return The half-open range [first, second) of offsets, among the
     *          primitives of *this, of shell @p i's primitives.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     */
    range_type primitive_range(size_type i) const {
        return pimpl_().primitive_range(i);
    }

    /** @brief Returns the offset of the shell primitive @p i belongs to.
     *
     *  @throw std::out_of_range if @p i is not in [0, n_primitives()). Strong
     *                           throw guarantee.
     */
    size_type primitive_to_shell(size_type i) const {
        return pimpl_().primitive_to_shell(i);
    }

    /** @brief Returns the primitive at offset @p i.
     *
     *  The primitives of *this are those of its shells, in order. The result
     *  aliases the state of *this, including the angular momentum of the
     *  primitive's shell and the center, so writing through it writes into
     *  *this. It is invalidated by the same things as the result of at().
     *  When *this models a read-only set, both overloads return a read-only
     *  view.
     *
     *  @throw std::out_of_range if @p i is not in [0, n_primitives()). Strong
     *                           throw guarantee.
     */
    ///@{
    primitive_reference primitive(size_type i) {
        if constexpr(is_const) {
            return std::as_const(pimpl_()).primitive(i);
        } else {
            return pimpl_().primitive(i);
        }
    }
    const_primitive_reference primitive(size_type i) const {
        return pimpl_().primitive(i);
    }
    ///@}

    /** @brief Returns the array of every coefficient (exponent) in *this.
     *
     *  The coefficients (exponents) of every primitive of every shell are
     *  stored back to back, in the order of the primitives. As with
     *  PointSetCommon::get_buffer, this is the escape hatch for code which
     *  needs a pointer to contiguous parameters. When *this models a
     *  read-only set, both overloads return a read-only view.
     */
    ///@{
    buffer_reference get_coefficient_buffer() {
        if constexpr(is_const) {
            return std::as_const(pimpl_()).coefficient_buffer();
        } else {
            return pimpl_().coefficient_buffer();
        }
    }
    const_buffer_reference get_coefficient_buffer() const {
        return pimpl_().coefficient_buffer();
    }
    buffer_reference get_exponent_buffer() {
        if constexpr(is_const) {
            return std::as_const(pimpl_()).exponent_buffer();
        } else {
            return pimpl_().exponent_buffer();
        }
    }
    const_buffer_reference get_exponent_buffer() const {
        return pimpl_().exponent_buffer();
    }
    ///@}

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two atomic basis sets are value equal if they hold shells of the same
     *  type (purity and ordering), have the same name, atomic number, and
     *  center, and their shells are pairwise value equal. Whether either owns
     *  its state is NOT considered.
     *
     *  @throw std::bad_alloc if there is a problem allocating the shell views
     *                        being compared. Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherType>
    bool operator==(
      const AtomicBasisSetCommon<OtherDerived, OtherType>& rhs) const {
        return pimpl_().are_equal(rhs.pimpl_());
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See operator== for
     *  the definition of value equal.
     */
    template<typename OtherDerived, typename OtherType>
    bool operator!=(
      const AtomicBasisSetCommon<OtherDerived, OtherType>& rhs) const {
        return !((*this) == rhs);
    }

    /** @brief Serializes *this into @p ar.
     *
     *  Writes the purity, ordering, name, atomic number, and center once,
     *  then the number of shells and, for each shell, its angular momentum,
     *  its number of primitives, and each primitive's coefficient/exponent
     *  pair. Only save is implemented here; loading requires resizing the
     *  state, which only AtomicBasisSet can do.
     *
     *  @tparam Archive The type of the cereal output archive.
     *
     *  @param[in,out] ar The archive to write to.
     *
     *  @throw std::runtime_error if any parameter is holding an
     *                            uncertainty-quantification type, which can
     *                            not be serialized. Weak throw guarantee.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        const auto& p = pimpl_();
        ar(static_cast<int>(p.purity()), static_cast<int>(p.ordering()));
        ar(p.name(), p.atomic_number());
        p.center().save(ar);
        const auto n_shells = p.size();
        ar(n_shells);
        const auto cs = p.coefficient_buffer();
        const auto es = p.exponent_buffer();
        for(size_type i = 0; i < n_shells; ++i) {
            const auto [b, e] = p.primitive_range(i);
            ar(p.get_l(i), e - b);
            for(auto j = b; j < e; ++j) {
                detail_::save_float(ar, cs.at(j));
                detail_::save_float(ar, es.at(j));
            }
        }
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AtomicBasisSetCommon() noexcept                            = default;
    AtomicBasisSetCommon(const AtomicBasisSetCommon&) noexcept = default;
    AtomicBasisSetCommon(AtomicBasisSetCommon&&) noexcept      = default;
    AtomicBasisSetCommon& operator=(const AtomicBasisSetCommon&) noexcept =
      default;
    AtomicBasisSetCommon& operator=(AtomicBasisSetCommon&&) noexcept = default;
    ~AtomicBasisSetCommon() noexcept                                 = default;
    ///@}

    /** @brief Writes the state of @p rhs through into the state of *this.
     *
     *  This implements write-through assignment for a view. Only values are
     *  written; the shape (purity, ordering, number of shells, and number of
     *  primitives in each shell) can not change, since a view can not resize
     *  what it aliases.
     *
     *  @throw std::runtime_error if the shape of @p rhs differs from that of
     *                            *this, which is checked before anything is
     *                            written (strong throw guarantee), or if the
     *                            floating-point types of the parameters
     *                            differ (weak throw guarantee).
     */
    template<typename OtherDerived, typename OtherType>
    void assign_(const AtomicBasisSetCommon<OtherDerived, OtherType>& rhs) {
        static_assert(!is_const, "Can not assign through a read-only view.");
        auto& p        = pimpl_();
        const auto& rp = rhs.pimpl_();
        if(&p == &rp) return;
        if(p.purity() != rp.purity() || p.ordering() != rp.ordering() ||
           p.size() != rp.size())
            throw_shape_mismatch_();
        for(size_type i = 0; i < p.size(); ++i)
            if(p.primitive_range(i) != rp.primitive_range(i))
                throw_shape_mismatch_();

        p.name()          = rp.name();
        p.atomic_number() = rp.atomic_number();
        p.center()        = rp.center();
        for(size_type i = 0; i < p.size(); ++i) p.set_l(i, rp.get_l(i));
        auto cs        = p.coefficient_buffer();
        auto es        = p.exponent_buffer();
        const auto rcs = rp.coefficient_buffer();
        const auto res = rp.exponent_buffer();
        for(size_type i = 0; i < p.n_primitives(); ++i) {
            detail_::assign(cs.at(i), rcs.at(i));
            detail_::assign(es.at(i), res.at(i));
        }
    }

private:
    /// Throws the error assign_ raises when the shapes differ
    [[noreturn]] static void throw_shape_mismatch_() {
        throw std::runtime_error(
          "chemist::experimental::AtomicBasisSetView: can not assign a set "
          "with a different shell type, number of shells, or number of "
          "primitives per shell through a view.");
    }

    /// Wraps getting the implementation from the derived class
    ///@{
    pimpl_type& pimpl_() { return static_cast<DerivedType&>(*this).pimpl_(); }
    const pimpl_type& pimpl_() const {
        return static_cast<const DerivedType&>(*this).pimpl_();
    }
    ///@}
};

} // namespace chemist::experimental

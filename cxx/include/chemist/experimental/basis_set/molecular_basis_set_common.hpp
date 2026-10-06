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
#include <algorithm>
#include <chemist/experimental/basis_set/atomic_basis_set_view.hpp>
#include <chemist/experimental/basis_set/detail_/atomic_basis_set_view_factory.hpp>
#include <chemist/experimental/detail_/buffer_slice.hpp>
#include <chemist/experimental/point/point_set_view.hpp>
#include <chemist/experimental/traits/molecular_basis_set_traits.hpp>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utilities/iterators/offset_iterator.hpp>
#include <utility>

namespace chemist::experimental {

/** @brief Implements the API shared by MolecularBasisSet and
 *         MolecularBasisSetView.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, a molecular
 *  basis set is a container of atomic basis sets. Different atoms may use
 *  different basis sets and, unlike the shells within one atomic basis set,
 *  different types of shell (purity and AO ordering); *this can be asked
 *  whether they agree.
 *
 *  None of the atomic basis sets exist as objects inside the set. The set
 *  stores the state of all of them back to back: one array of coefficients
 *  and one of exponents holding every primitive of every shell of every atom,
 *  the angular momentum of each shell, the offsets of each shell into the
 *  parameter arrays, the offsets of each atom into the shells, and, per atom,
 *  the center, the name of the basis set, the atomic number, and the type of
 *  the shells. Indexing builds an AtomicBasisSetView over the requested
 *  atom's slice of that state, so there is exactly one copy of every
 *  parameter however the set is accessed. The flattened state is also
 *  available directly: as the shells of the set (shell, n_shells, ...), as
 *  its primitives (primitive, n_primitives, ...), or as the raw parameter
 *  arrays (get_coefficient_buffer, get_exponent_buffer).
 *
 *  Offsets into the per-shell and per-primitive state are taken relative to
 *  the first element of the corresponding offset array, exactly as
 *  detail_::AtomicBasisSetViewPIMPL does, so that a view need not alias a
 *  whole set.
 *
 *  *this implements the API in terms of the derived class's state, which it
 *  reaches through the following hooks. The owning class defines mutable and
 *  read-only overloads of those returning views; the view class defines only
 *  read-only-qualified ones, since whether the state they return is mutable
 *  is decided by MolecularBasisSetType.
 *
 *  - `coefficient_data_()`, `exponent_data_()`: the parameter arrays.
 *  - `l_data_()`: the angular momentum of each shell.
 *  - `primitive_offset_data_()`: the offsets of the shells into the
 *    parameter arrays, plus one past the end.
 *  - `shell_offset_data_()`: the offsets of the atoms into the shells, plus
 *    one past the end.
 *  - `center_data_()`: the centers, as a point set.
 *  - `name_data_()`, `atomic_number_data_()`: per-atom names and atomic
 *    numbers.
 *  - `purity_data_()`, `ordering_data_()`: the type of each atom's shells.
 *
 *  @tparam DerivedType The class deriving from *this. Must define the hooks
 *                      above and declare every MolecularBasisSetCommon a
 *                      friend.
 *  @tparam MolecularBasisSetType The, possibly const-qualified,
 *                                MolecularBasisSet the derived class models.
 *                                Its const-qualification decides whether the
 *                                setters exist and whether indexing hands
 *                                out mutable views.
 */
template<typename DerivedType, typename MolecularBasisSetType>
class MolecularBasisSetCommon {
private:
    /// Struct defining the types for the set *this acts like
    using traits_type = ChemistClassTraits<MolecularBasisSetType>;

    /// Is the state *this models read-only?
    static constexpr bool is_const = std::is_const_v<MolecularBasisSetType>;

    /// Enables a method only when *this can mutate the state
    template<typename U, typename ReturnType = void>
    using enable_if_mutable_t =
      std::enable_if_t<!std::is_const_v<U>, ReturnType>;

    /// Lets *this compare against, and assign from, other sets
    template<typename OtherDerived, typename OtherType>
    friend class MolecularBasisSetCommon;

public:
    /// Pull in the types of the set *this acts like
    ///@{
    using angular_momentum_type = typename traits_type::angular_momentum_type;
    using atomic_basis_set_reference =
      typename traits_type::atomic_basis_set_reference;
    using atomic_basis_set_type = typename traits_type::atomic_basis_set_type;
    using buffer_reference      = typename traits_type::buffer_reference;
    using buffer_type           = typename traits_type::buffer_type;
    using center_set_reference  = typename traits_type::center_set_reference;
    using const_atomic_basis_set_reference =
      typename traits_type::const_atomic_basis_set_reference;
    using const_buffer_reference = typename traits_type::const_buffer_reference;
    using const_center_set_reference =
      typename traits_type::const_center_set_reference;
    using const_primitive_reference =
      typename traits_type::const_primitive_reference;
    using name_type           = typename traits_type::name_type;
    using primitive_reference = typename traits_type::primitive_reference;
    using range_type          = typename traits_type::range_type;
    using shell_pointer       = typename traits_type::shell_pointer;
    using size_type           = typename traits_type::size_type;
    ///@}

    /// Container types, for interoperability with the standard library
    ///@{
    using value_type      = atomic_basis_set_type;
    using reference       = atomic_basis_set_reference;
    using const_reference = const_atomic_basis_set_reference;
    using iterator        = utilities::iterators::OffsetIterator<DerivedType>;
    using const_iterator =
      utilities::iterators::OffsetIterator<const DerivedType>;
    ///@}

    // -------------------------------------------------------------------------
    // -- Atoms
    // -------------------------------------------------------------------------

    /// Returns the number of atoms, i.e. atomic basis sets, in *this
    size_type size() const noexcept { return downcast_().name_data_().size(); }

    /// Does *this have no atoms?
    bool empty() const noexcept { return size() == 0; }

    /** @brief Returns the atomic basis set of atom @p a.
     *
     *  The set is built on demand, as a view of atom @p a's slice of the
     *  state of *this: it is not a copy, writes through it are writes into
     *  *this (and vice versa), and it must not outlive *this. Anything which
     *  changes the size of that state (adding an atom, swap, assignment,
     *  deserialization) invalidates it, much like an iterator into a
     *  std::vector. Since it is a view, the shape of the atomic basis set
     *  (its number of shells and their numbers of primitives) can not be
     *  changed through it. When *this models a read-only set, both overloads
     *  return a read-only view.
     *
     *  @param[in] a The offset of the atom. Must be in [0, size()).
     *
     *  @throw std::out_of_range if @p a is not in [0, size()). Strong throw
     *                           guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the view.
     *                        Strong throw guarantee.
     */
    ///@{
    reference at(size_type a) {
        check_atom_(a);
        return at_(a);
    }
    const_reference at(size_type a) const {
        check_atom_(a);
        return at_(a);
    }
    ///@}

    /// Same as at()
    ///@{
    reference operator[](size_type a) { return at(a); }
    const_reference operator[](size_type a) const { return at(a); }
    ///@}

    /** @brief Iterators over the atomic basis sets of *this.
     *
     *  Dereferencing an iterator calls operator[], so it yields a newly built
     *  view, by value, rather than a reference. It is therefore invalidated
     *  by the same things as the result of at().
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    iterator begin() noexcept { return {0, &downcast_()}; }
    const_iterator begin() const noexcept { return {0, &downcast_()}; }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator end() noexcept { return {size(), &downcast_()}; }
    const_iterator end() const noexcept { return {size(), &downcast_()}; }
    const_iterator cend() const noexcept { return end(); }
    ///@}

    /** @brief Returns the centers of every atom, as a point set.
     *
     *  The centers are stored contiguously, so this aliases them rather than
     *  copying them; it is the one copy of each center, shared with the
     *  atomic basis sets, shells, and primitives obtained from *this. The
     *  mutable overload therefore moves every shell on an atom when one of
     *  its points is written through. When *this models a read-only set,
     *  both overloads return a read-only view.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    center_set_reference get_centers() {
        if constexpr(is_const) {
            return std::as_const(downcast_()).center_data_();
        } else {
            return downcast_().center_data_();
        }
    }
    const_center_set_reference get_centers() const {
        return downcast_().center_data_();
    }
    ///@}

    // -------------------------------------------------------------------------
    // -- Shell types
    // -------------------------------------------------------------------------

    /** @brief Returns the purity of the shells of atom @p a.
     *
     *  @throw std::out_of_range if @p a is not in [0, size()). Strong throw
     *                           guarantee.
     */
    ShellPurity purity(size_type a) const {
        check_atom_(a);
        return downcast_().purity_data_()[a];
    }

    /** @brief Returns the AO ordering of the shells of atom @p a.
     *
     *  @throw std::out_of_range if @p a is not in [0, size()). Strong throw
     *                           guarantee.
     */
    AOOrdering ordering(size_type a) const {
        check_atom_(a);
        return downcast_().ordering_data_()[a];
    }

    /** @brief Do the shells of every atom have the same purity?
     *
     *  This is true for an empty set.
     *
     *  @throw None No throw guarantee.
     */
    bool has_uniform_purity() const noexcept {
        return is_uniform_(downcast_().purity_data_());
    }

    /** @brief Do the shells of every atom have the same AO ordering?
     *
     *  This is true for an empty set.
     *
     *  @throw None No throw guarantee.
     */
    bool has_uniform_ordering() const noexcept {
        return is_uniform_(downcast_().ordering_data_());
    }

    /** @brief Are the shells of every atom pure (spherical)?
     *
     *  Unlike for an atomic basis set, this is not the same as
     *  `!is_cartesian()`, since some atoms may have pure shells and others
     *  Cartesian ones. Both are true for an empty set.
     *
     *  @throw None No throw guarantee.
     */
    bool is_pure() const noexcept { return all_have_(ShellPurity::pure); }

    /// Are the shells of every atom Cartesian? See is_pure().
    bool is_cartesian() const noexcept {
        return all_have_(ShellPurity::cartesian);
    }

    // -------------------------------------------------------------------------
    // -- Shells
    // -------------------------------------------------------------------------

    /// Returns the number of shells in *this, across every atom
    size_type n_shells() const noexcept { return downcast_().l_data_().size(); }

    /** @brief Returns the offsets of the shells of atom @p a.
     *
     *  @return The half-open range [first, second) of offsets, among the
     *          shells of *this, of atom @p a's shells.
     *
     *  @throw std::out_of_range if @p a is not in [0, size()). Strong throw
     *                           guarantee.
     */
    range_type shell_range(size_type a) const {
        check_atom_(a);
        return shell_range_(a);
    }

    /** @brief Returns the offset of the atom shell @p s belongs to.
     *
     *  @throw std::out_of_range if @p s is not in [0, n_shells()). Strong
     *                           throw guarantee.
     */
    size_type shell_to_atom(size_type s) const {
        check_shell_(s);
        return shell_to_atom_(s);
    }

    /** @brief Returns the shell at offset @p s.
     *
     *  The shells of *this are those of its atoms, in order. The result is
     *  exactly what indexing atom shell_to_atom(s) of *this would hand out,
     *  so see AtomicBasisSetCommon::at for its type and for what invalidates
     *  it.
     *
     *  @throw std::out_of_range if @p s is not in [0, n_shells()). Strong
     *                           throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the view. Strong
     *                        throw guarantee.
     */
    shell_pointer shell(size_type s) const {
        check_shell_(s);
        const auto a = shell_to_atom_(s);
        return at_(a).at(s - shell_range_(a).first);
    }

    /** @brief Returns the angular momentum of shell @p s.
     *
     *  @throw std::out_of_range if @p s is not in [0, n_shells()). Strong
     *                           throw guarantee.
     */
    angular_momentum_type get_l(size_type s) const {
        check_shell_(s);
        return downcast_().l_data_()[s];
    }

    /** @brief Sets the angular momentum of shell @p s to @p l.
     *
     *  When *this models a read-only set this method does not participate in
     *  overload resolution.
     *
     *  @throw std::out_of_range if @p s is not in [0, n_shells()). Strong
     *                           throw guarantee.
     */
    template<typename U = MolecularBasisSetType>
    enable_if_mutable_t<U> set_l(size_type s, angular_momentum_type l) {
        check_shell_(s);
        downcast_().l_data_()[s] = l;
    }

    /** @brief Returns the number of AOs in *this.
     *
     *  This is the sum of the sizes of the shells, each of which follows from
     *  its angular momentum and its atom's purity exactly as in
     *  AOShellBaseCommon::size.
     *
     *  @throw None No throw guarantee.
     */
    size_type n_aos() const noexcept {
        const auto ls       = downcast_().l_data_();
        const auto purities = downcast_().purity_data_();
        size_type n         = 0;
        for(size_type a = 0; a < size(); ++a) {
            const bool pure   = purities[a] == ShellPurity::pure;
            const auto [b, e] = shell_range_(a);
            for(auto s = b; s < e; ++s) {
                const auto l = ls[s];
                n += pure ? 2 * l + 1 : (l + 1) * (l + 2) / 2;
            }
        }
        return n;
    }

    // -------------------------------------------------------------------------
    // -- Primitives
    // -------------------------------------------------------------------------

    /// Returns the number of primitives in *this, across every shell
    size_type n_primitives() const noexcept {
        const auto offsets = downcast_().primitive_offset_data_();
        return offsets.back() - offsets.front();
    }

    /** @brief Returns the offsets of the primitives of shell @p s.
     *
     *  @return The half-open range [first, second) of offsets, among the
     *          primitives of *this, of shell @p s's primitives.
     *
     *  @throw std::out_of_range if @p s is not in [0, n_shells()). Strong
     *                           throw guarantee.
     */
    range_type primitive_range(size_type s) const {
        check_shell_(s);
        return primitive_range_(s);
    }

    /** @brief Returns the offset of the shell primitive @p p belongs to.
     *
     *  @throw std::out_of_range if @p p is not in [0, n_primitives()). Strong
     *                           throw guarantee.
     */
    size_type primitive_to_shell(size_type p) const {
        check_primitive_(p);
        return primitive_to_shell_(p);
    }

    /** @brief Returns the primitive at offset @p p.
     *
     *  The primitives of *this are those of its shells, in order. The result
     *  aliases the state of *this, including the angular momentum of the
     *  primitive's shell and the center of its atom, so writing through it
     *  writes into *this. It is invalidated by the same things as the result
     *  of at(). When *this models a read-only set, both overloads return a
     *  read-only view.
     *
     *  @throw std::out_of_range if @p p is not in [0, n_primitives()). Strong
     *                           throw guarantee.
     */
    ///@{
    primitive_reference primitive(size_type p) {
        if constexpr(is_const) {
            return std::as_const(*this).primitive(p);
        } else {
            check_primitive_(p);
            auto& d      = downcast_();
            const auto s = primitive_to_shell_(p);
            return primitive_reference(d.coefficient_data_().at(p),
                                       d.exponent_data_().at(p), d.l_data_()[s],
                                       d.center_data_()[shell_to_atom_(s)]);
        }
    }
    const_primitive_reference primitive(size_type p) const {
        check_primitive_(p);
        const auto& d                      = downcast_();
        const auto s                       = primitive_to_shell_(p);
        const_buffer_reference cs          = d.coefficient_data_();
        const_buffer_reference es          = d.exponent_data_();
        const_center_set_reference centers = d.center_data_();
        return const_primitive_reference(cs.at(p), es.at(p), d.l_data_()[s],
                                         centers[shell_to_atom_(s)]);
    }
    ///@}

    /** @brief Returns the array of every coefficient (exponent) in *this.
     *
     *  The coefficients (exponents) of every primitive of every shell of
     *  every atom are stored back to back, in the order of the primitives.
     *  As with PointSetCommon::get_buffer, this is the escape hatch for code
     *  which needs a pointer to contiguous parameters. When *this models a
     *  read-only set, both overloads return a read-only view.
     */
    ///@{
    buffer_reference get_coefficient_buffer() {
        if constexpr(is_const) {
            return std::as_const(downcast_()).coefficient_data_();
        } else {
            return downcast_().coefficient_data_();
        }
    }
    const_buffer_reference get_coefficient_buffer() const {
        return downcast_().coefficient_data_();
    }
    buffer_reference get_exponent_buffer() {
        if constexpr(is_const) {
            return std::as_const(downcast_()).exponent_data_();
        } else {
            return downcast_().exponent_data_();
        }
    }
    const_buffer_reference get_exponent_buffer() const {
        return downcast_().exponent_data_();
    }
    ///@}

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two molecular basis sets are value equal if they have the same number
     *  of atoms and their atomic basis sets are pairwise value equal (see
     *  AtomicBasisSetCommon::operator==). Whether either owns its state is
     *  NOT considered.
     *
     *  @throw std::bad_alloc if there is a problem allocating the views being
     *                        compared. Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherType>
    bool operator==(
      const MolecularBasisSetCommon<OtherDerived, OtherType>& rhs) const {
        if(size() != rhs.size()) return false;
        for(size_type a = 0; a < size(); ++a)
            if(at_(a) != rhs.at_(a)) return false;
        return true;
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See operator== for
     *  the definition of value equal.
     */
    template<typename OtherDerived, typename OtherType>
    bool operator!=(
      const MolecularBasisSetCommon<OtherDerived, OtherType>& rhs) const {
        return !((*this) == rhs);
    }

    /** @brief Serializes *this into @p ar.
     *
     *  Writes the number of atoms and then each atomic basis set, in the
     *  format AtomicBasisSetCommon::save uses. Only save is implemented here;
     *  loading requires resizing the state, which only MolecularBasisSet can
     *  do.
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
        ar(size());
        for(size_type a = 0; a < size(); ++a) at_(a).save(ar);
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    MolecularBasisSetCommon() noexcept                               = default;
    MolecularBasisSetCommon(const MolecularBasisSetCommon&) noexcept = default;
    MolecularBasisSetCommon(MolecularBasisSetCommon&&) noexcept      = default;
    MolecularBasisSetCommon& operator=(
      const MolecularBasisSetCommon&) noexcept = default;
    MolecularBasisSetCommon& operator=(MolecularBasisSetCommon&&) noexcept =
      default;
    ~MolecularBasisSetCommon() noexcept = default;
    ///@}

    /** @brief Writes the state of @p rhs through into the state of *this.
     *
     *  This implements write-through assignment for a view. Only values are
     *  written; the shape (number of atoms, the type of each atom's shells,
     *  the number of shells on each atom, and the number of primitives in
     *  each shell) can not change, since a view can not resize what it
     *  aliases.
     *
     *  @throw std::runtime_error if the shape of @p rhs differs from that of
     *                            *this, which is checked before anything is
     *                            written (strong throw guarantee), or if the
     *                            floating-point types of the parameters
     *                            differ (weak throw guarantee).
     */
    template<typename OtherDerived, typename OtherType>
    void assign_(const MolecularBasisSetCommon<OtherDerived, OtherType>& rhs) {
        static_assert(!is_const, "Can not assign through a read-only view.");
        if(size() != rhs.size() || n_shells() != rhs.n_shells())
            throw_shape_mismatch_();
        const auto& d  = downcast_();
        const auto& rd = rhs.downcast_();
        for(size_type a = 0; a < size(); ++a) {
            if(d.purity_data_()[a] != rd.purity_data_()[a] ||
               d.ordering_data_()[a] != rd.ordering_data_()[a] ||
               shell_range_(a) != rhs.shell_range_(a))
                throw_shape_mismatch_();
        }
        for(size_type s = 0; s < n_shells(); ++s)
            if(primitive_range_(s) != rhs.primitive_range_(s))
                throw_shape_mismatch_();

        for(size_type a = 0; a < size(); ++a) {
            auto atom = at_(a);
            atom      = rhs.at_(a);
        }
    }

private:
    /// Throws the error assign_ raises when the shapes differ
    [[noreturn]] static void throw_shape_mismatch_() {
        throw std::runtime_error(
          "chemist::experimental::MolecularBasisSetView: can not assign a set "
          "with a different number of atoms, shell types, number of shells "
          "per atom, or number of primitives per shell through a view.");
    }

    /// Throws std::out_of_range if @p a is not a valid atom offset
    void check_atom_(size_type a) const {
        if(a < size()) return;
        throw_out_of_range_("atom", a, size());
    }

    /// Throws std::out_of_range if @p s is not a valid shell offset
    void check_shell_(size_type s) const {
        if(s < n_shells()) return;
        throw_out_of_range_("shell", s, n_shells());
    }

    /// Throws std::out_of_range if @p p is not a valid primitive offset
    void check_primitive_(size_type p) const {
        if(p < n_primitives()) return;
        throw_out_of_range_("primitive", p, n_primitives());
    }

    /// Throws std::out_of_range for offset @p i into @p n @p what s
    [[noreturn]] static void throw_out_of_range_(const std::string& what,
                                                 size_type i, size_type n) {
        throw std::out_of_range(
          "chemist::experimental::MolecularBasisSet: " + what + " offset " +
          std::to_string(i) + " is out of range for a set with " +
          std::to_string(n) + " " + what + "s.");
    }

    /// Are all of the elements of @p values equal?
    template<typename SpanType>
    static bool is_uniform_(SpanType values) noexcept {
        return std::adjacent_find(values.begin(), values.end(),
                                  std::not_equal_to<>{}) == values.end();
    }

    /// Do the shells of every atom have purity @p purity?
    bool all_have_(ShellPurity purity) const noexcept {
        const auto purities = downcast_().purity_data_();
        return std::all_of(purities.begin(), purities.end(),
                           [purity](ShellPurity p) { return p == purity; });
    }

    /// Unchecked shell_range
    range_type shell_range_(size_type a) const noexcept {
        const auto offsets = downcast_().shell_offset_data_();
        return {offsets[a] - offsets.front(), offsets[a + 1] - offsets.front()};
    }

    /// Unchecked primitive_range
    range_type primitive_range_(size_type s) const noexcept {
        const auto offsets = downcast_().primitive_offset_data_();
        return {offsets[s] - offsets.front(), offsets[s + 1] - offsets.front()};
    }

    /// Unchecked shell_to_atom
    size_type shell_to_atom_(size_type s) const noexcept {
        return last_at_or_before_(downcast_().shell_offset_data_(), s);
    }

    /// Unchecked primitive_to_shell
    size_type primitive_to_shell_(size_type p) const noexcept {
        return last_at_or_before_(downcast_().primitive_offset_data_(), p);
    }

    /** @brief Returns the offset of the range in @p offsets which holds @p i.
     *
     *  That is the last range starting at or before @p i. Taking the last,
     *  rather than the first, skips over any empty ranges, e.g. an atom with
     *  no shells. @p i is relative to the first element of @p offsets.
     */
    template<typename SpanType>
    static size_type last_at_or_before_(SpanType offsets,
                                        size_type i) noexcept {
        const auto itr =
          std::upper_bound(offsets.begin(), offsets.end(), i + offsets.front());
        return static_cast<size_type>(itr - offsets.begin()) - 1;
    }

    /** @brief Builds a view of atom @p a's slice of the state.
     *
     *  The views of the state passed in decide whether the result is
     *  mutable; this is what lets one function implement both overloads of
     *  at_.
     */
    template<typename ViewType, typename BufferType, typename LSpan,
             typename CenterSet, typename NameSpan, typename ZSpan>
    ViewType make_atom_(size_type a, BufferType cs, BufferType es, LSpan ls,
                        CenterSet centers, NameSpan names, ZSpan zs) const {
        const auto& d               = downcast_();
        const auto offsets          = d.primitive_offset_data_();
        const auto [s_begin, s_end] = shell_range_(a);
        const auto n_shells_on_atom = s_end - s_begin;
        // offsets has one more element than there are shells, so this is
        // valid even for a trailing atom with no shells
        const auto p_begin         = offsets[s_begin] - offsets.front();
        const auto n_prims_on_atom = offsets[s_end] - offsets[s_begin];
        return ViewType(detail_::make_atomic_basis_set_view_pimpl(
          d.purity_data_()[a], d.ordering_data_()[a],
          detail_::slice_buffer(std::move(cs), p_begin, n_prims_on_atom),
          detail_::slice_buffer(std::move(es), p_begin, n_prims_on_atom),
          ls.subspan(s_begin, n_shells_on_atom),
          offsets.subspan(s_begin, n_shells_on_atom + 1), centers[a], names[a],
          zs[a]));
    }

    /// Unchecked at
    ///@{
    reference at_(size_type a) {
        if constexpr(is_const) {
            return std::as_const(*this).at_(a);
        } else {
            auto& d = downcast_();
            return make_atom_<reference>(
              a, d.coefficient_data_(), d.exponent_data_(), d.l_data_(),
              d.center_data_(), d.name_data_(), d.atomic_number_data_());
        }
    }
    const_reference at_(size_type a) const {
        const auto& d = downcast_();
        return make_atom_<const_reference>(
          a, const_buffer_reference(d.coefficient_data_()),
          const_buffer_reference(d.exponent_data_()),
          typename traits_type::const_l_span(d.l_data_()),
          const_center_set_reference(d.center_data_()),
          typename traits_type::const_name_span(d.name_data_()),
          typename traits_type::const_atomic_number_span(
            d.atomic_number_data_()));
    }
    ///@}

    /// Wraps casting *this to the derived class
    DerivedType& downcast_() noexcept {
        return static_cast<DerivedType&>(*this);
    }

    /// Wraps casting *this to a read-only reference to the derived class
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::experimental

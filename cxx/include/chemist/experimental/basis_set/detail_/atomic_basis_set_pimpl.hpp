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
#include <chemist/experimental/basis_set/cca_shell_view.hpp>
#include <chemist/experimental/basis_set/contracted_gaussian_view.hpp>
#include <chemist/experimental/basis_set/detail_/atomic_basis_set_pimpl_base.hpp>
#include <chemist/experimental/detail_/buffer_slice.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/types/floating_point.hpp>
#include <memory>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace chemist::experimental::detail_ {

template<typename ShellType>
class AtomicBasisSetViewPIMPL;

/** @brief Implements AtomicBasisSetPIMPLBase for one type of shell.
 *
 *  Everything an atomic basis set does can be written in terms of its state
 *  (see AtomicBasisSetPIMPLBase), so *this writes it once, and the derived
 *  classes only differ in whether they own that state or alias it. This is
 *  the same split as, e.g., ContractedGaussianCommon, applied to the
 *  implementation of a class rather than to its public API.
 *
 *  Indexing builds a view of the shell from the state: a slice of each
 *  parameter array, a reference to the shell's angular momentum, and a view of
 *  the center, assembled into a ContractedGaussianView and wrapped in the
 *  shell type's read-only view. Nothing is copied, so writes to the state are
 *  visible through every shell view built from it.
 *
 *  @tparam DerivedType The class deriving from *this. Must define the
 *                      `*_data_()` hooks used below and declare *this a
 *                      friend.
 *  @tparam ShellType The, possibly const-qualified, type of the shells. When
 *                    it is const-qualified the mutable hooks throw
 *                    std::logic_error, since the state is read-only.
 */
template<typename DerivedType, typename ShellType>
class AtomicBasisSetPIMPLCommon : public AtomicBasisSetPIMPLBase {
private:
    /// Type *this inherits from
    using base_type = AtomicBasisSetPIMPLBase;

    /// The shell type, without const
    using shell_value_type = std::remove_const_t<ShellType>;

    /// Struct defining the types of the shells
    using shell_traits = ChemistClassTraits<shell_value_type>;

    /// Type of a read-only view of one shell; what indexing builds
    using const_shell_view_type = typename shell_traits::const_view_type;

    /// Type of a mutable view of one shell
    using shell_view_type = typename shell_traits::view_type;

    /// Is the state read-only?
    static constexpr bool is_const = std::is_const_v<ShellType>;

public:
    /// Pull in the base's types
    ///@{
    using typename base_type::angular_momentum_type;
    using typename base_type::atomic_number_reference;
    using typename base_type::buffer_reference;
    using typename base_type::center_reference;
    using typename base_type::const_atomic_number_reference;
    using typename base_type::const_buffer_reference;
    using typename base_type::const_center_reference;
    using typename base_type::const_name_reference;
    using typename base_type::const_primitive_reference;
    using typename base_type::name_reference;
    using typename base_type::pimpl_pointer;
    using typename base_type::primitive_reference;
    using typename base_type::range_type;
    using typename base_type::shell_pointer;
    using typename base_type::size_type;
    ///@}

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AtomicBasisSetPIMPLCommon() noexcept = default;
    AtomicBasisSetPIMPLCommon(const AtomicBasisSetPIMPLCommon&) noexcept =
      default;
    AtomicBasisSetPIMPLCommon(AtomicBasisSetPIMPLCommon&&) noexcept = default;
    AtomicBasisSetPIMPLCommon& operator=(
      const AtomicBasisSetPIMPLCommon&) noexcept = default;
    AtomicBasisSetPIMPLCommon& operator=(AtomicBasisSetPIMPLCommon&&) noexcept =
      default;
    ///@}

    /// Implements the base's hooks in terms of the derived class's state
    ///@{
    ShellPurity purity_() const noexcept override {
        return shell_traits::purity;
    }

    AOOrdering ordering_() const noexcept override {
        return shell_traits::ordering;
    }

    size_type size_() const noexcept override {
        return downcast_().offset_data_().size() - 1;
    }

    shell_pointer at_(size_type i) const override {
        const auto& d             = downcast_();
        const auto [b, e]         = primitive_range_(i);
        const_buffer_reference cs = d.coefficient_data_();
        const_buffer_reference es = d.exponent_data_();
        const auto& l             = d.l_data_()[i];
        ContractedGaussianView<const ContractedGaussian> cg(
          slice_buffer(std::move(cs), b, e - b),
          slice_buffer(std::move(es), b, e - b), l,
          const_center_reference(d.center_data_()));
        return std::make_unique<const_shell_view_type>(std::move(cg));
    }

    angular_momentum_type l_(size_type i) const noexcept override {
        return downcast_().l_data_()[i];
    }

    void set_l_(size_type i, angular_momentum_type l) override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            downcast_().l_data_()[i] = l;
        }
    }

    bool is_same_ordering_(const AOShellView& shell) const override {
        return dynamic_cast<const const_shell_view_type*>(&shell) != nullptr ||
               dynamic_cast<const shell_view_type*>(&shell) != nullptr;
    }

    size_type n_primitives_() const noexcept override {
        const auto offsets = downcast_().offset_data_();
        return offsets.back() - offsets.front();
    }

    range_type primitive_range_(size_type i) const noexcept override {
        const auto offsets = downcast_().offset_data_();
        return {offsets[i] - offsets.front(), offsets[i + 1] - offsets.front()};
    }

    size_type primitive_to_shell_(size_type i) const noexcept override {
        // The shell owning primitive i is the last one starting at or before
        // it. Taking the last, rather than the first, skips over any shells
        // with no primitives.
        const auto offsets = downcast_().offset_data_();
        const auto itr =
          std::upper_bound(offsets.begin(), offsets.end(), i + offsets.front());
        return static_cast<size_type>(itr - offsets.begin()) - 1;
    }

    primitive_reference primitive_(size_type i) override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            auto& d      = downcast_();
            const auto s = primitive_to_shell_(i);
            return primitive_reference(d.coefficient_data_().at(i),
                                       d.exponent_data_().at(i), d.l_data_()[s],
                                       d.center_data_());
        }
    }

    const_primitive_reference primitive_(size_type i) const override {
        const auto& d             = downcast_();
        const auto s              = primitive_to_shell_(i);
        const_buffer_reference cs = d.coefficient_data_();
        const_buffer_reference es = d.exponent_data_();
        return const_primitive_reference(
          cs.at(i), es.at(i), d.l_data_()[s],
          const_center_reference(d.center_data_()));
    }

    buffer_reference coefficient_buffer_() override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            return downcast_().coefficient_data_();
        }
    }

    const_buffer_reference coefficient_buffer_() const override {
        return downcast_().coefficient_data_();
    }

    buffer_reference exponent_buffer_() override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            return downcast_().exponent_data_();
        }
    }

    const_buffer_reference exponent_buffer_() const override {
        return downcast_().exponent_data_();
    }

    center_reference center_() override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            return downcast_().center_data_();
        }
    }

    const_center_reference center_() const override {
        return downcast_().center_data_();
    }

    name_reference name_() override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            return downcast_().name_data_();
        }
    }

    const_name_reference name_() const override {
        return downcast_().name_data_();
    }

    atomic_number_reference atomic_number_() override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            return downcast_().atomic_number_data_();
        }
    }

    const_atomic_number_reference atomic_number_() const override {
        return downcast_().atomic_number_data_();
    }

    pimpl_pointer as_view_() override {
        if constexpr(is_const) {
            throw_read_only_();
        } else {
            auto& d = downcast_();
            return std::make_unique<AtomicBasisSetViewPIMPL<shell_value_type>>(
              d.coefficient_data_(), d.exponent_data_(), d.l_data_(),
              d.offset_data_(), d.center_data_(), d.name_data_(),
              d.atomic_number_data_());
        }
    }

    pimpl_pointer as_const_view_() const override {
        const auto& d = downcast_();
        return std::make_unique<
          AtomicBasisSetViewPIMPL<const shell_value_type>>(
          d.coefficient_data_(), d.exponent_data_(), d.l_data_(),
          d.offset_data_(), d.center_data_(), d.name_data_(),
          d.atomic_number_data_());
    }
    ///@}

private:
    /// Thrown by the mutable hooks when the state is read-only
    [[noreturn]] static void throw_read_only_() {
        throw std::logic_error(
          "chemist::experimental::AtomicBasisSet: can not mutate read-only "
          "state.");
    }

    /// Wraps casting *this to the derived class
    DerivedType& downcast_() noexcept {
        return static_cast<DerivedType&>(*this);
    }

    /// Wraps casting *this to a read-only reference to the derived class
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

/** @brief Implements an atomic basis set by aliasing state owned elsewhere.
 *
 *  This is what AtomicBasisSetView holds. It aliases each piece of state
 *  listed in AtomicBasisSetPIMPLBase separately, rather than aliasing an
 *  AtomicBasisSetPIMPL, so that the state can live anywhere; in particular,
 *  it can be a slice of the state of a larger container. For that reason the
 *  shell offsets need not start at zero: they are taken relative to their
 *  first element, which is where the primitive arrays are taken to start.
 *
 *  A view can not change the number of shells or primitives, so add_shell
 *  throws.
 *
 *  @tparam ShellType The, possibly const-qualified, type of the shells. When
 *                    it is const-qualified the aliased state is read-only.
 */
template<typename ShellType>
class AtomicBasisSetViewPIMPL
  : public AtomicBasisSetPIMPLCommon<AtomicBasisSetViewPIMPL<ShellType>,
                                     ShellType> {
private:
    /// Type *this inherits from
    using base_type =
      AtomicBasisSetPIMPLCommon<AtomicBasisSetViewPIMPL<ShellType>, ShellType>;

    /// Lets the CRTP base reach the *_data_ hooks
    friend base_type;

    /// Is the aliased state read-only?
    static constexpr bool is_const = std::is_const_v<ShellType>;

    /// Types of the atomic basis set *this acts like
    using abs_traits = ChemistClassTraits<
      std::conditional_t<is_const, const AtomicBasisSet, AtomicBasisSet>>;

    /// Applies const to T if the aliased state is read-only
    template<typename T>
    using apply_const_t = std::conditional_t<is_const, const T, T>;

public:
    /// Pull in the base's types
    ///@{
    using typename base_type::angular_momentum_type;
    using typename base_type::pimpl_pointer;
    using typename base_type::size_type;
    ///@}

    /// Type of a, possibly read-only, view of a parameter array
    using alias_buffer_reference = typename abs_traits::buffer_reference;

    /// Type of a, possibly read-only, view of the angular momenta
    using l_span_type = std::span<apply_const_t<angular_momentum_type>>;

    /// Type of a read-only view of the shell offsets
    using offset_span_type = std::span<const size_type>;

    /// Type of a, possibly read-only, view of the center
    using alias_center_reference = typename abs_traits::center_reference;

    /// Type of a, possibly read-only, reference to the name
    using alias_name_reference = typename abs_traits::name_reference;

    /// Type of a, possibly read-only, reference to the atomic number
    using alias_atomic_number_reference =
      typename abs_traits::atomic_number_reference;

    /** @brief Creates an implementation aliasing the provided state.
     *
     *  @param[in] coefficients Every coefficient, shell-major.
     *  @param[in] exponents Every exponent, shell-major.
     *  @param[in] ls The angular momentum of each shell.
     *  @param[in] offsets The offsets of the shells into the parameter
     *                     arrays, plus one past the end. Taken relative to
     *                     the first element.
     *  @param[in] center The center shared by every shell.
     *  @param[in] name The name of the basis set.
     *  @param[in] atomic_number The atomic number.
     *
     *  @throw std::invalid_argument if @p offsets does not have exactly one
     *                               more element than @p ls, or if the
     *                               parameter arrays are not exactly as long
     *                               as @p offsets says. Strong throw
     *                               guarantee.
     */
    AtomicBasisSetViewPIMPL(alias_buffer_reference coefficients,
                            alias_buffer_reference exponents, l_span_type ls,
                            offset_span_type offsets,
                            alias_center_reference center,
                            alias_name_reference name,
                            alias_atomic_number_reference atomic_number) :
      m_coefficients_(std::move(coefficients)),
      m_exponents_(std::move(exponents)),
      m_ls_(ls),
      m_offsets_(offsets),
      m_center_(std::move(center)),
      m_name_(&name),
      m_atomic_number_(&atomic_number) {
        if(m_offsets_.size() != m_ls_.size() + 1)
            throw std::invalid_argument(
              "chemist::experimental::AtomicBasisSet: there must be one more "
              "shell offset than there are shells.");
        const auto n = m_offsets_.back() - m_offsets_.front();
        if(m_coefficients_.size() != n || m_exponents_.size() != n)
            throw std::invalid_argument(
              "chemist::experimental::AtomicBasisSet: the parameter arrays "
              "must hold exactly the primitives the shell offsets describe.");
    }

    /// Creates another alias of the state @p other aliases
    AtomicBasisSetViewPIMPL(const AtomicBasisSetViewPIMPL& other) = default;

protected:
    /// Implements clone as a shallow copy
    pimpl_pointer clone_() const override {
        return std::make_unique<AtomicBasisSetViewPIMPL>(*this);
    }

    /// A view can not change the number of shells, so this always throws
    void add_shell_(angular_momentum_type,
                    typename base_type::const_buffer_reference,
                    typename base_type::const_buffer_reference) override {
        throw std::runtime_error(
          "chemist::experimental::AtomicBasisSet: can not add a shell "
          "through a view.");
    }

private:
    /** @brief Implements the CRTP base's state access.
     *
     *  Only const-qualified overloads are needed: whether the returned
     *  references are mutable is decided by ShellType, not by the
     *  const-qualification of *this.
     */
    ///@{
    alias_buffer_reference coefficient_data_() const { return m_coefficients_; }
    alias_buffer_reference exponent_data_() const { return m_exponents_; }
    l_span_type l_data_() const noexcept { return m_ls_; }
    offset_span_type offset_data_() const noexcept { return m_offsets_; }
    alias_center_reference center_data_() const { return m_center_; }
    alias_name_reference name_data_() const noexcept { return *m_name_; }
    alias_atomic_number_reference atomic_number_data_() const noexcept {
        return *m_atomic_number_;
    }
    ///@}

    /// Aliases the coefficients of every primitive
    alias_buffer_reference m_coefficients_;

    /// Aliases the exponents of every primitive
    alias_buffer_reference m_exponents_;

    /// Aliases the angular momentum of each shell
    l_span_type m_ls_;

    /// Aliases the offsets of the shells into the parameter arrays
    offset_span_type m_offsets_;

    /// Aliases the center
    alias_center_reference m_center_;

    /// Aliases the name. A pointer so that *this can be copied.
    std::remove_reference_t<alias_name_reference>* m_name_;

    /// Aliases the atomic number. A pointer so that *this can be copied.
    std::remove_reference_t<alias_atomic_number_reference>* m_atomic_number_;
};

/** @brief Implements an atomic basis set which owns its state.
 *
 *  This is what AtomicBasisSet holds. See AtomicBasisSetPIMPLBase for the
 *  layout of the state.
 *
 *  @tparam ShellType The type of the shells. Must not be const-qualified.
 */
template<typename ShellType>
class AtomicBasisSetPIMPL
  : public AtomicBasisSetPIMPLCommon<AtomicBasisSetPIMPL<ShellType>,
                                     ShellType> {
private:
    static_assert(!std::is_const_v<ShellType>,
                  "An owning AtomicBasisSetPIMPL can not be read-only.");

    /// Type *this inherits from
    using base_type =
      AtomicBasisSetPIMPLCommon<AtomicBasisSetPIMPL<ShellType>, ShellType>;

    /// Lets the CRTP base reach the *_data_ hooks
    friend base_type;

    /// Types of the atomic basis set *this implements
    using abs_traits = ChemistClassTraits<AtomicBasisSet>;

    /// The concrete types the parameter arrays may hold; see PointSet::fp_types
    using fp_types = chemist::types::floating_point_types;

public:
    /// Pull in the base's types
    ///@{
    using typename base_type::angular_momentum_type;
    using typename base_type::buffer_reference;
    using typename base_type::center_reference;
    using typename base_type::const_buffer_reference;
    using typename base_type::const_center_reference;
    using typename base_type::pimpl_pointer;
    using typename base_type::size_type;
    ///@}

    /// Type used to own a parameter array
    using buffer_type = typename abs_traits::buffer_type;

    /// Type used to own the center
    using center_type = typename abs_traits::center_type;

    /// Type used to own the name
    using name_type = typename abs_traits::name_type;

    /// Type used to own the atomic number
    using atomic_number_type = typename abs_traits::atomic_number_type;

    /// Creates a set with no shells, no name, atomic number 0, at the origin
    AtomicBasisSetPIMPL() = default;

    /** @brief Creates a set with no shells.
     *
     *  @param[in] name The name of the basis set.
     *  @param[in] atomic_number The atomic number.
     *  @param[in] center The point every shell will be centered on.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state.
     *                        Strong throw guarantee.
     */
    AtomicBasisSetPIMPL(name_type name, atomic_number_type atomic_number,
                        center_type center) :
      m_center_(std::move(center)),
      m_name_(std::move(name)),
      m_atomic_number_(atomic_number) {}

    /// Deep copies @p other
    AtomicBasisSetPIMPL(const AtomicBasisSetPIMPL& other) = default;

protected:
    /// Implements clone as a deep copy
    pimpl_pointer clone_() const override {
        return std::make_unique<AtomicBasisSetPIMPL>(*this);
    }

    /** @brief Implements add_shell.
     *
     *  The new state is built in full and then swapped in, so that a failure
     *  part way through (e.g. a floating-point type mismatch in the exponents
     *  after the coefficients went in fine) leaves *this untouched. This also
     *  makes it safe for @p coefficients and @p exponents to alias *this.
     *  Atomic basis sets are small, so the copy is cheap.
     */
    void add_shell_(angular_momentum_type l,
                    const_buffer_reference coefficients,
                    const_buffer_reference exponents) override {
        const auto n = coefficients.size();
        buffer_type cs(m_coefficients_);
        buffer_type es(m_exponents_);
        for(size_type i = 0; i < n; ++i) {
            cs.template push_back<fp_types>(detail_::copy(coefficients.at(i)));
            es.template push_back<fp_types>(detail_::copy(exponents.at(i)));
        }
        auto ls = m_ls_;
        ls.push_back(l);
        auto offsets = m_offsets_;
        offsets.push_back(offsets.back() + n);

        std::swap(m_coefficients_, cs);
        std::swap(m_exponents_, es);
        m_ls_.swap(ls);
        m_offsets_.swap(offsets);
    }

private:
    /// Implements the CRTP base's state access
    ///@{
    buffer_reference coefficient_data_() { return m_coefficients_.as_view(); }
    const_buffer_reference coefficient_data_() const {
        return m_coefficients_.as_view();
    }
    buffer_reference exponent_data_() { return m_exponents_.as_view(); }
    const_buffer_reference exponent_data_() const {
        return m_exponents_.as_view();
    }
    std::span<angular_momentum_type> l_data_() noexcept { return m_ls_; }
    std::span<const angular_momentum_type> l_data_() const noexcept {
        return m_ls_;
    }
    std::span<const size_type> offset_data_() const noexcept {
        return m_offsets_;
    }
    center_reference center_data_() { return center_reference(m_center_); }
    const_center_reference center_data_() const {
        return const_center_reference(m_center_);
    }
    name_type& name_data_() noexcept { return m_name_; }
    const name_type& name_data_() const noexcept { return m_name_; }
    atomic_number_type& atomic_number_data_() noexcept {
        return m_atomic_number_;
    }
    const atomic_number_type& atomic_number_data_() const noexcept {
        return m_atomic_number_;
    }
    ///@}

    /// The coefficient of every primitive of every shell, shell-major
    buffer_type m_coefficients_;

    /// The exponent of every primitive of every shell, shell-major
    buffer_type m_exponents_;

    /// The angular momentum of each shell
    std::vector<angular_momentum_type> m_ls_;

    /// The offsets of the shells into the parameter arrays, plus one past the
    /// end; so shell i owns [m_offsets_[i], m_offsets_[i + 1])
    std::vector<size_type> m_offsets_{0};

    /// The point every shell is centered on
    center_type m_center_;

    /// The name of the basis set
    name_type m_name_;

    /// The atomic number
    atomic_number_type m_atomic_number_ = 0;
};

extern template class AtomicBasisSetPIMPL<CCAShell<CartesianAO>>;
extern template class AtomicBasisSetPIMPL<CCAShell<SphericalAO>>;
extern template class AtomicBasisSetViewPIMPL<CCAShell<CartesianAO>>;
extern template class AtomicBasisSetViewPIMPL<CCAShell<SphericalAO>>;
extern template class AtomicBasisSetViewPIMPL<const CCAShell<CartesianAO>>;
extern template class AtomicBasisSetViewPIMPL<const CCAShell<SphericalAO>>;

} // namespace chemist::experimental::detail_

#pragma once
#include "../contracted_gaussian/cgto_pimpl.hpp"
#include "chemist/basis_set/shell.hpp"
#include <utilities/containers/own_or_borrow.hpp>

namespace chemist::detail_ {

/** @brief Implements the Shell class
 *
 *  In electronic structure theory a "shell" is a set of AOs which have the same
 *  total angular momemtum, @f$\ell@f$ and the same primitive Gaussians.
 *
 *
 *  @tparam T Assumed to be a POD, non-cv qualified, floating point type such as
 *            double or float.
 */
template<typename T>
class ShellPIMPL {
public:
    /// Type used for indexing and offsets
    using size_type = typename Shell<T>::size_type;

    /// Type used for storing the purity of the shell
    using pure_type = ShellType;

    /// Type used to store the total angular momentum of the shell
    using am_type = size_type;

    /// Type used to hold the parameters for a CGTO
    using cgto_set = std::vector<T>;

    /** @brief Holds a state that is suitable for use as a placeholder Shell
     *
     *  This ctor will create a shell that has all state members default
     *  initialized. In particular this means the resulting Shell is a Cartesian
     *  s-shell whose only basis function is comprised of no CGTOs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        MathSet PIMPL. Strong throw guarantee.
     */
    ShellPIMPL() = default;

    /** @brief Creates a new ShellPIMPL instance that is a deep copy of @p rhs.
     *
     *  This ctor creates a new ShellPIMPL instance by deep copying an existing
     *  instance.
     *
     *  @param[in] rhs The instance to make a deep copy of.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p rhs's
     *                        state. Strong throw guarantee.
     */
    ShellPIMPL(const ShellPIMPL<T>& rhs);

    /** @brief Creates a new ShellPIMPL instance with the specified state.
     *
     *  This ctor allows the user to set the state of the Shell instance and
     *  determine whether or not the Shell aliases its state or not. Owning the
     *  state is indicated by passing by value/reference, whereas providing
     *  pointers indicates aliasing.
     *
     *  @tparam U Expected to be bool or bool*
     *  @tparam V Expected to be size_type or size_type*
     *  @tparam W Expected to be MathSet<T> or MathSet<T>*
     *  @tparam X Expected to be MathSet<T> or MathSet<T>*
     *
     *  @param[in] pure If true this shell will be pure, whereas if it is false
     *                  the shell will be Cartesian.
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] cs The contraction coefficients for the CGTO common to all
     *                AOs in this shell.
     *  @param[in] es The exponents for the primitive Gaussians forming the
     *                CGTO common to all AOs in this shell.
     *
     *  @throw std::bad_alloc if we are copying @p cs and/or @p es and there is
     *                        insufficient memory. Strong throw guarantee.
     */
    template<typename U, typename V, typename W, typename X>
    ShellPIMPL(U&& pure, V&& l, W&& cs, X&& es);

    /** @brief Determines the purity of the shell.
     *
     *  A shell of AOs is said to be pure if each AO is an eigenfunction of
     *  L**2, otherwise it is not. This function can be used to set/inquire into
     *  the purity of the current shell.
     *
     *  @return True if the shell is pure and false otherwise. The resulting
     *          value is returned by reference so that it can be changed.
     *
     *  @throw none No throw guarantee.
     */
    pure_type& purity() noexcept { return m_pure_.value(); }

    /** @brief Determines the purity of the shell.
     *
     *  A shell of AOs is said to be pure if each AO is an eigenfunction of
     *  L**2, otherwise it is not. This function can be used to inquire into
     *  the purity of the current shell.
     *
     *  @return True if the shell is pure and false otherwise. The resulting
     *          value is read-only.
     *
     *  @throw none No throw guarantee.
     */
    const pure_type& purity() const noexcept { return m_pure_.value(); }

    /** @brief Determines the total angular momentum of the shell.
     *
     *  All AOs in a shell have the same total angular momentum. This function
     *  can be used to retrieve/set the total angular momentum of the shell.
     *
     *  @return The total angular momentum of the shell. The resulting value is
     *          returned as a read/write reference so that it can be modified if
     *          desired.
     *
     *  @throw none No throw guarantee.
     */
    am_type& l() noexcept { return m_l_.value(); }

    /** @brief Determines the total angular momentum of the shell.
     *
     *  All AOs in a shell have the same total angular momentum. This function
     *  can be used to retrieve the total angular momentum of the shell.
     *
     *  @return The total angular momentum of the shell. The resulting value is
     *          read-only.
     *
     *  @throw none No throw guarantee.
     */
    const am_type& l() const noexcept { return m_l_.value(); }

    /** @brief Determines the number of AOs in the shell.
     *
     *  A pure shell of total angular momentum @f$\ell@f$ has @f$2\ell + 1@f$
     *  AOs in it, whereas a Cartesian shell of the same total angular momentum
     *  has @f$\binom{2 + \ell}{2}@f$ (the number of ways of distributing
     *  @f$\ell@f$ units of angular momenta among the three Cartesian
     *  coordinates).
     *
     *  @return The number of AOs in the shell.
     *
     *  @throw none No throw guarantee.
     */
    size_type size() const noexcept;

    /** @brief Returns the PIMPL for the @p index-th CGTO in the shell
     *
     *  This function is primarily intended as code factorization for creating
     *  a PIMPL for a ContractedGaussianView instance. The PIMPL returned from
     *  this function is always read-/write-able regardless of const-ness and
     *  assumes that the caller will properly account for const correctness
     *  before returning the ContractedGaussianView instance.
     *
     *  @param[in] index Which of the `size()` AOs to return? @p index must be
     *                   in the range [0, `size()`), but no bounds check is
     *                   performed.
     *  @return A PIMPL that aliases the state of the @p index-th CGTO in this
     *          shell.
     */
    std::unique_ptr<detail_::CGTOPIMPL<T>> at(size_type index) const noexcept;

private:
    /// Whether or not this shell is pure
    utilities::OwnOrBorrow<pure_type> m_pure_;
    /// The total angular momentum of the shell
    utilities::OwnOrBorrow<am_type> m_l_;
    /// Contraction coefficients of the CGTOs common to all AOs in this shell
    utilities::OwnOrBorrow<cgto_set> m_cs_;
    /// The exponents of the CGTOs common to all AOs in this shell
    utilities::OwnOrBorrow<cgto_set> m_es_;
}; // class ShellPIMPL

// --------------------- Implementations ---------------------------------------

template<typename T>
ShellPIMPL<T>::ShellPIMPL(const ShellPIMPL<T>& rhs) :
  ShellPIMPL(rhs.purity(), rhs.l(), rhs.m_cs_.value(), rhs.m_es_.value()) {}

template<typename T>
template<typename U, typename V, typename W, typename X>
ShellPIMPL<T>::ShellPIMPL(U&& pure, V&& l, W&& cs, X&& es) :
  m_pure_(std::forward<U>(pure)),
  m_l_(std::forward<V>(l)),
  m_cs_(std::forward<W>(cs)),
  m_es_(std::forward<X>(es)) {}

template<typename T>
typename ShellPIMPL<T>::size_type ShellPIMPL<T>::size() const noexcept {
    const auto L     = m_l_.value();
    const auto p_val = 2ul * L + 1ul;
    // closed form for 2+L choose 2
    const auto c_val   = (L * L + 3ul * L + 2ul) / 2ul;
    const bool is_pure = m_pure_.value() == ShellType::pure;
    return is_pure ? p_val : c_val;
}

template<typename T>
std::unique_ptr<detail_::CGTOPIMPL<T>> ShellPIMPL<T>::at(
  size_type) const noexcept {
    auto p_cs = const_cast<cgto_set*>(&m_cs_.value());
    auto p_es = const_cast<cgto_set*>(&m_es_.value());
    return std::make_unique<detail_::CGTOPIMPL<T>>(p_cs, p_es);
}

} // namespace chemist::detail_

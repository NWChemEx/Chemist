#pragma once
#include "libchemist/basis_set/shell/detail_/shell_pimpl.hpp"

namespace libchemist::detail_ {

/** @brief Implements a Center
 *
 *  As a technical note, the C++ standard specializes std::vector<bool> so that
 *  it does not return references. We therefore can not use it and allow the
 *  user to do something like `center[i].pure() = false`. To get around this, we
 *  internally store the purity as an integer.
 *
 */
template<typename T>
class CenterPIMPL {
public:
    /// Unsigned integral type used for indexes and offsets
    using size_type = std::size_t;
    /// Type used to store the purity of a shell
    using pure_type = int;
    /// Type used to store the total angular momentum of a shell
    using am_type = size_type;
    /// Type used to hold the purities for a set of shells
    using pure_set = std::vector<pure_type>;
    /// Type used to hold the total angular momentum of a set of shells
    using am_set = std::vector<am_type>;
    /// Type used to hold the parameters for a CTGO
    using param_set = std::vector<T>;
    /// Type used to hold a set of param_set instances
    using shell_set = std::vector<param_set>;

    CenterPIMPL() = default;
    CenterPIMPL(const CenterPIMPL<T>& rhs);

    template<typename U, typename V, typename W, typename X>
    CenterPIMPL(U&& purities, V&& ls, W&& cs, X&& es);

    void add_shell(bool purity, am_type l, param_set cs, param_set es);

    size_type size() const noexcept { return m_purities_.value().size(); }

    std::unique_ptr<ShellPIMPL<T>> at(size_type i) const;

private:
    /// The purities of the shells making up this center
    utilities::OwnOrBorrow<pure_set> m_purities_;
    /// The total angular momenta of the shells making up this center
    utilities::OwnOrBorrow<am_set> m_ls_;
    /// The contraction coefficients for each shell making up this center
    utilities::OwnOrBorrow<shell_set> m_coefs_;
    /// The exponents for each shell making up this center
    utilities::OwnOrBorrow<shell_set> m_exps_;
}; // class Center

// ---------------------------- Implementations --------------------------------

template<typename T>
CenterPIMPL<T>::CenterPIMPL(const CenterPIMPL<T>& rhs) :
  CenterPIMPL(rhs.m_purities_.value(), rhs.m_ls_.value(), rhs.m_coefs_.value(),
              rhs.m_exps_.value()) {}

template<typename T>
template<typename U, typename V, typename W, typename X>
CenterPIMPL<T>::CenterPIMPL(U&& purities, V&& ls, W&& cs, X&& es) :
  m_purities_(std::forward<U>(purities)),
  m_ls_(std::forward<V>(ls)),
  m_coefs_(std::forward<W>(cs)),
  m_exps_(std::forward<X>(es)) {}

template<typename T>
void CenterPIMPL<T>::add_shell(bool purity, am_type l, param_set cs,
                               param_set es) {
    m_purities_.value().push_back(purity);
    m_ls_.value().push_back(l);
    m_coefs_.value().emplace_back(std::move(cs));
    m_exps_.value().emplace_back(std::move(es));
}

template<typename T>
std::unique_ptr<ShellPIMPL<T>> CenterPIMPL<T>::at(size_type i) const {
    return std::make_unique<ShellPIMPL<T>>(
      const_cast<pure_type*>(&(m_purities_.value()[i])),
      const_cast<am_type*>(&(m_ls_.value()[i])),
      const_cast<param_set*>(&(m_coefs_.value()[i])),
      const_cast<param_set*>(&(m_exps_.value()[i])));
}

} // namespace libchemist::detail_

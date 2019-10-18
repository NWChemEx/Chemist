#pragma once
#include "libchemist/basis_set/shell/detail_/shell_pimpl.hpp"

namespace libchemist::detail_ {

template<typename T>
class CenterPIMPL {
public:
    using size_type = std::size_t;

    CenterPIMPL() = default;
    CenterPIMPL(const CenterPIMPL<T>& rhs);

    template<typename U, typename V, typename W, typename X>
    CenterPIMPL(U&& purities, V&& ls, W&& cs, X&& es);

    size_type size() const noexcept { return m_purities_.value().size(); }

    std::unique_ptr<ShellPIMPL<T>> at(size_type i) const;

private:
    utilities::OwnOrBorrow<MathSet<bool>> m_purities_;
    utilities::OwnOrBorrow<MathSet<int>> m_ls_;
    utilities::OwnOrBorrow<MathSet<MathSet<T>>> m_coefs_;
    utilities::OwnOrBorrow<MathSet<MathSet<T>>> m_exps_;
};

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
std::unique_ptr<ShellPIMPL<T>> CenterPIMPL<T>::at(size_type i) const {
    return std::make_unique<ShellPIMPL<T>>(
      const_cast<bool*>(&(m_purities_.value()[i])),
      const_cast<int*>(&(m_ls_.value()[i])),
      const_cast<MathSet<T>*>(&(m_coefs_.value()[i])),
      const_cast<MathSet<T>*>(&(m_exps_.value()[i])));
}

} // namespace libchemist::detail_

#pragma once
#include <utilities/containers/own_or_borrow.hpp>

namespace libchemist::detail_ {

template<typename T>
class PrimitivePIMPL {
public:
    /// Type of a read/write reference to one of this Primitive's parameters
    using reference = T&;
    /// Type of a read-only reference to one of this Primitive's parameters
    using const_reference = const T&;

    PrimitivePIMPL() noexcept = default;
    PrimitivePIMPL(T c, T a) noexcept : m_coef_(c), m_exp_(a) {}
    PrimitivePIMPL(const PrimitivePIMPL<T>& rhs) noexcept;
    PrimitivePIMPL(PrimitivePIMPL<T>&& rhs) noexcept = default;
    PrimitivePIMPL<T>& operator=(const PrimitivePIMPL<T>& rhs) noexcept;
    PrimitivePIMPL<T>& operator=(PrimitivePIMPL<T>&& rhs) noexcept = default;

    reference coef() noexcept { return m_coef_.value(); }
    const_reference coef() const noexcept { return m_coef_.value(); }
    reference exp() noexcept { return m_exp_.value(); }
    const_reference exp() const noexcept { return m_exp_.value(); }

private:
    utilities::OwnOrBorrow<T> m_coef_;
    utilities::OwnOrBorrow<T> m_exp_;
};

// --------------------------------Implementations------------------------------

template<typename T>
PrimitivePIMPL<T>::PrimitivePIMPL(const PrimitivePIMPL<T>& rhs) noexcept :
  PrimitivePIMPL(rhs.coef(), rhs.exp()) {}

template<typename T>
PrimitivePIMPL<T>& PrimitivePIMPL<T>::operator=(
  const PrimitivePIMPL<T>& rhs) noexcept {
    m_coef_ = rhs.coef();
    m_exp_  = rhs.exp();
    return *this;
}

} // namespace libchemist::detail_

#pragma once
#include <utility> //std::move
namespace libchemist::basis_set::detail_ {

template<typename T>
class GaussianPIMPL {
public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    constexpr GaussianPIMPL(T c, T a) noexcept;
    constexpr reference coef() noexcept { return m_c_; }
    constexpr const_reference coef() const noexcept { return m_c_; }
    constexpr reference exp() noexcept { return m_a_; }
    constexpr const_reference exp() const noexcept { return m_a_; }

private:
    T m_c_;
    T m_a_;
};

template<typename T>
class GaussianAliasPIMPL {
public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    GaussianAliasPIMPL(T* c, T* a) noexcept;
    constexpr reference coef() noexcept { return *m_c_; }
    constexpr const_reference coef() const noexcept { return *m_c_; }
    constexpr reference exp() noexcept { return *m_a_; }
    constexpr const_reference exp() const noexcept { return *m_a_; }

private:
    T* m_c_;
    T* m_a_;
};

//-----------------------------Implementations----------------------------------

template<typename T>
constexpr GaussianPIMPL<T>::GaussianPIMPL(T c, T a) noexcept :
  m_c_(std::move(c)),
  m_a_(std::move(a)) {}

template<typename T>
GaussianAliasPIMPL<T>::GaussianAliasPIMPL(T* c, T* a) noexcept :
  m_c_(std::move(c)),
  m_a_(std::move(a)) {}

} // namespace libchemist::basis_set::detail_

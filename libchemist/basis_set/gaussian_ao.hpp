#pragma once
#include "libchemist/basis_set/detail_/gaussian_ao_pimpl.hpp"
#include "libchemist/basis_set/gaussian.hpp"

namespace libchemist::basis_set {
/** @brief A class for modeling a (Cartesian) Gaussian function
 *
 *  (Cartesian) Gaussian basis functions are linear combination of Gaussian
 *  primitives that are centered on the same point in space. This class holds a
 *  set of Gaussian primitives and factors out the Cartesian coordinate of the
 *  center and places the contraction coefficients and exponents into contiguous
 *  arrays. We do not store which Cartesian basis function this is nor is there
 *  a way to generate this information at this time.
 *
 *  @tparam Traits
 */
template<typename PIMPLType>
class GaussianAO {
private:
    using prim_traits = typename PIMPLType::prim_traits;

public:
    using coef_type   = typename PIMPLType::coef_type;
    using exp_type    = typename PIMPLType::exp_type;
    using center_type = typename PIMPLType::center_type;

    explicit constexpr GaussianAO(PIMPLType pimpl) noexcept;

    constexpr auto& center() noexcept { return m_pimpl_.center(); }
    constexpr const auto& center() const noexcept { return m_pimpl_.center(); }

    constexpr type::size size() const noexcept { return m_pimpl_.size(); }

    constexpr auto& coef(type::size i) noexcept { return m_pimpl_.coef(i); }
    constexpr const auto& coef(type::size i) const noexcept;

    constexpr auto& exp(type::size i) noexcept { return m_pimpl_.exp(i); }
    constexpr const auto& exp(type::size i) const noexcept;

    auto operator[](type::size i) noexcept {
        return make_gaussian_alias<prim_traits>(i, this);
    }

    auto operator[](type::size i) const noexcept {
        return make_gaussian_alias<prim_traits>(i, this);
    }

private:
    PIMPLType m_pimpl_;
};

//----------------------Helper Functions----------------------------------------

template<typename... Args>
constexpr auto make_gaussian_ao(Args... args) noexcept {
    return GaussianAO{
      detail_::make_gaussian_ao_pimpl(std::forward<Args>(args)...)};
}

template<typename Parent1, typename Parent2>
constexpr bool operator==(const GaussianAO<Parent1>& lhs,
                          const GaussianAO<Parent2>& rhs) noexcept {
    if(lhs.size() != rhs.size()) return false;
    for(type::size i = 0; i < lhs.size(); ++i)
        if(std::tie(lhs.coef(i), lhs.exp(i)) !=
           std::tie(rhs.coef(i), rhs.exp(i)))
            return false;
    return std::tie(lhs.center()[0], lhs.center()[1], lhs.center()[2]) ==
           std::tie(rhs.center()[0], rhs.center()[1], rhs.center()[2]);
}

template<typename Parent1, typename Parent2>
constexpr bool operator!=(const GaussianAO<Parent1>& lhs,
                          const GaussianAO<Parent2>& rhs) noexcept {
    return !(lhs == rhs);
}

//----------------------Implementations----------------------------------------

template<typename PIMPLType>
constexpr GaussianAO<PIMPLType>::GaussianAO(PIMPLType pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

template<typename PIMPLType>
constexpr const auto& GaussianAO<PIMPLType>::coef(type::size i) const noexcept {
    return m_pimpl_.coef(i);
}

template<typename PIMPLType>
constexpr const auto& GaussianAO<PIMPLType>::exp(type::size i) const noexcept {
    return m_pimpl_.exp(i);
}

} // namespace libchemist::basis_set

#pragma once
#include "libchemist/basis_set/detail_/utility.hpp"
#include "libchemist/basis_set/gaussian.hpp"

namespace libchemist::basis_set {

template<typename BasisSetType>
class GaussianView {
private:
    using basis_set_type = std::remove_pointer_t<BasisSetType>;
    using index_type     = typename basis_set_type::index_type;

public:
    template<typename... Idxs>
    explicit constexpr GaussianView(BasisSetType bs, Idxs... idxs) noexcept :
      m_index_({idxs...}),
      m_bs_(bs) {
        static_assert(sizeof...(Idxs) == std::tuple_size_v<index_type>,
                      "Wrong number of indices provided");
    }

    constexpr auto& coefficient() noexcept {
        return detail_::coefficient(m_bs_, m_index_);
    }
    constexpr auto& coefficient() const noexcept {
        return detail_::coefficient(m_bs_, m_index_);
    }

    constexpr auto& exponent() noexcept {
        return detail_::exponent(m_bs_, m_index_);
    }
    constexpr auto& exponent() const noexcept {
        return detail_::exponent(m_bs_, m_index_);
    }

    constexpr auto& center() noexcept {
        return detail_::center(m_bs_, m_index_);
    }
    constexpr auto& center() const noexcept {
        return detail_::center(m_bs_, m_index_);
    }

    template<typename T>
    constexpr bool operator==(const GaussianView<T>& rhs) const noexcept {
        return detail_::compare_gaussians(*this, rhs);
    }

    template<typename T>
    constexpr bool operator!=(const GaussianView<T>& rhs) const noexcept {
        return !((*this) == rhs);
    }

private:
    index_type m_index_;
    BasisSetType m_bs_;
};

} // namespace libchemist::basis_set

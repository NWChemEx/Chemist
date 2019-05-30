#pragma once

namespace libchemist::basis_set {

template<typename BasisSetType>
class GaussianAOView {
private:
    using basis_set_type = std::remove_pointer_t<BasisSetType>;
    using index_type     = typename basis_set_type::index_type;

public:
    template<typename... Idxs>
    explicit constexpr GaussianAOView(BasisSetType bs, Idxs... idxs) noexcept :
      m_index_({idxs...}),
      m_bs_(bs) {
        static_assert(sizeof...(Idxs) == std::tuple_size_v<index_type>,
                      "Wrong number of indices provided");
    }

    constexpr auto primitive(type::size i) noexcept {
        return GaussianView{m_bs_, detail_::cat_indices(m_index_, i)};
    }

    constexpr auto primitive(type::size i) const noexcept {
        return GaussianView{m_bs_, detail_::cat_indices(m_index_, i)};
    }

    constexpr auto nprims() const noexcept {
        return detail_::nprims(m_bs_, m_index_);
    }

    constexpr auto& coefficient(type::size i) noexcept {
        return detail_::coefficient(m_bs_, detail_::cat_indices(m_index_, i));
    }
    constexpr auto& coefficient(type::size i) const noexcept {
        return detail_::coefficient(m_bs_, detail_::cat_indices(m_index_, i));
    }

    constexpr auto& exponent(type::size i) noexcept {
        return detail_::exponent(m_bs_, detail_::cat_indices(m_index_, i));
    }
    constexpr auto& exponent(type::size i) const noexcept {
        return detail_::exponent(m_bs_, detail_::cat_indices(m_index_, i));
    }

    constexpr auto& center(type::size i) noexcept {
        return detail_::center(m_bs_, detail_::cat_indices(m_index_, i));
    }

    constexpr auto& center(type::size i) const noexcept {
        return detail_::center(m_bs_, detail_::cat_indices(m_index_, i));
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
    BasisSetType m_bs_;
    index_type m_index_;
};

} // namespace libchemist::basis_set

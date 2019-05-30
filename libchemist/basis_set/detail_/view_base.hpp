#pragma once

namespace libchemist::basis_set::detail_ {

template<typename Parent, typename BasisSetType>
class GaussianViewBase {
private:
    using basis_set_type = typename Parent::basis_set_type;

public:
    GaussianViewBase(const Parent* parent,
                     basis_set_type bs) constexpr auto index() const noexcept {
        return m_parent_->index_();
    }

    constexpr auto& coefficient() noexcept {
        return std::apply(m_bs_->coefficient, *m_bs_, index());
    }

    constexpr auto& coefficient() const noexcept {
        return std::apply(m_bs_->coefficient, *m_bs_, index());
    }

private:
    const Parent* m_parent_;
    basis_set_type m_bs_;
};

template<typename Parent, typename BasisSetType>

} // namespace libchemist::basis_set::detail_

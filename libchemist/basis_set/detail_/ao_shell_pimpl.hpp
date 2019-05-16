#pragma once
#include "libchemist/basis_set/gaussian_ao.hpp"
namespace libchemist::basis_set::detail_ {

template<typename ParentType>
class AOShellBase {
private:
    using traits_type = typename ParentType::traits_type;

public:
    auto& center() { return get_value<0>(); }
    const auto& center() const { return get_value_<0>(); }

    auto& l() { return get_value_<1>(); }
    const auto& l() const { return get_value_<1>(); }

    auto& pure() { return get_value_<2>(); }
    const auto& pure() const { return get_value_<3>(); }

    auto& exp(type::size i, type::size j) { return (*this)[i][j].exp(); }
    const auto& exp(type::size i, type::size j) const {
        return (*this)[i][j].exp();
    }

    auto& coef(type::size i) { return (*this)[i][j].coef(); }
    const auto& coef(type::size i) const { return (*this)[i][j].coef(); }

    auto& operator()(type::size bfi, type::size primi) { return (*this)[i][j]; }
    const auto& operator()(type::size i, type::size j) const {
        return (*this)[i][j];
    }
    auto& operator[](type::size i) { return m_pimpl_.at_(i); }
    const auto& operator[](type::size i) const { return m_pimpl_.at_(i); }

private:
    using tuple_type = std::tuple<typename traits_type::center_type,
                                  typename traits_type::am_type,
                                  typename traits_type::pure_type>;
    template<type::size Index>
    auto& get_value_() {
        return detail_::get_value(m_params_);
    }

    template<type::size Index>
    const auto& get_value_() const {
        return detail_::get_value(m_params_);
    }

    tuple_type m_params_;

    ParentType m_pimpl_;
};

} // namespace libchemist::basis_set::detail_

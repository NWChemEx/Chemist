#pragma once
#include "libchemist/basis_set/gaussian_center_alias.hpp"

namespace libchemist::basis_set {
namespace detail_ {
template<typename T>
class GaussianBasisSetPIMPL;

extern template GaussianBasisSetPIMPL<double>;
extern template GaussianBasisSetPIMPL<float>;
} // namespace detail_

template<typename T = double>
class GaussianBasisSet {
public:
    template<typename CenterType>
    void add_center(CenterType&& c);

private:
    template<typename ShellType>
    void add_shell_(ShellType&& s);

    std::unique_ptr<GaussianBasisSetPIMPL<T>> m_pimpl_;
};

extern template GaussianBasisSetPIMPL<double>;
extern template GaussianBasisSetPIMPL<float>;

//-----------------------Implementations----------------------------------------
template<typename T>
template<typename CenterType>
void GaussianBasisSet<T>::add_center(CenterType&& s) {
    auto r0 = s.center();
}

template<typename T>
template<typename ShellType>
void GaussianBasisSet<T>::add_shell(ShellType&& s) {
    if(s.nao() == 0) return;
    const auto ao0    = s.ao(0);
    const auto nprims = ao0.nprims();
    std::vector<T> cs(nprims);
    std::vector<T> as(nprims);
    for(type::size i = 0; i < nprims; ++i) {
        const auto ao0pi = ao0.primitive(i);
        cs[i]            = ao0pi.coefficient();
        as[i]            = ao0pi.exponent();
    }
    add_shell(s.pure(), s.l(), std::move(cs), std::move(as));
}

} // namespace libchemist::basis_set

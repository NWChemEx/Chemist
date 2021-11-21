#include "chemist/detail_/orbital_space_pimpl.hpp"
#include "chemist/orbital_space.hpp"
#include "chemist/ta_helpers/ta_hashers.hpp"
#include <random>
namespace chemist {

#define OrbSpace OrbitalSpace<e_type, t_type>

template<typename e_type, typename t_type>
const typename OrbSpace::pimpl_type& OrbSpace::pimpl_() const {
    assert(m_pimpl_ != nullptr);
    return *m_pimpl_;
}

template<typename e_type, typename t_type>
OrbSpace::OrbitalSpace() : m_pimpl_(std::make_unique<pimpl_type>()) {}

template<typename e_type, typename t_type>
OrbSpace::OrbitalSpace(basis_type bs, t_type S, t_type C, t_type Cdagger,
                       t_type D) :
  m_pimpl_(std::make_unique<pimpl_type>(bs, S, C, Cdagger, D)) {}

template<typename e_type, typename t_type>
OrbSpace::OrbitalSpace(basis_type bs, t_type S, t_type C, t_type Cdagger) :
  m_pimpl_(std::make_unique<pimpl_type>(bs, S, C, Cdagger)) {}

template<typename e_type, typename t_type>
OrbSpace::OrbitalSpace(basis_type bs, t_type S, t_type C) :
  m_pimpl_(std::make_unique<pimpl_type>(bs, S, C)) {}

template<typename e_type, typename t_type>
OrbSpace::OrbitalSpace(const OrbitalSpace<e_type, t_type>& rhs) :
  m_pimpl_(rhs.pimpl_().clone()) {}

template<typename e_type, typename t_type>
OrbSpace::OrbitalSpace(OrbitalSpace<e_type, t_type>&& rhs) noexcept = default;

template<typename e_type, typename t_type>
OrbSpace& OrbSpace::operator=(const OrbitalSpace<e_type, t_type>& rhs) {
    return *this = std::move(OrbSpace(rhs));
}

template<typename e_type, typename t_type>
OrbSpace& OrbSpace::operator=(OrbitalSpace<e_type, t_type>&& rhs) noexcept =
  default;

template<typename e_type, typename t_type>
OrbSpace::~OrbitalSpace() noexcept = default;

template<typename e_type, typename t_type>
const typename OrbSpace::basis_type& OrbSpace::basis_set() const {
    return pimpl_().basis_set();
}

template<typename e_type, typename t_type>
const t_type& OrbSpace::S() const {
    return pimpl_().S();
}

template<typename e_type, typename t_type>
const t_type& OrbSpace::C() const {
    return pimpl_().C();
}

template<typename e_type, typename t_type>
const t_type& OrbSpace::Cdagger() const {
    return pimpl_().Cdagger();
}

template<typename e_type, typename t_type>
const t_type& OrbSpace::density() const {
    return pimpl_().density();
}

template<typename e_type, typename t_type>
t_type OrbSpace::transform_from_ao(const t_type X,
                                   const size_vec& modes) const {
    return pimpl_().transform_from_ao(X, modes);
}

template<typename e_type, typename t_type>
t_type OrbSpace::transform_to_ao(const t_type X, const size_vec& modes) const {
    return pimpl_().transform_to_ao(X, modes);
}

template<typename e_type, typename t_type>
void OrbSpace::hash(sde::Hasher& h) const {
    h(basis_set(), S(), C());
}

#define AOS AOSpace<e_type, t_type>

template<typename e_type, typename t_type>
AOS::AOSpace() : OrbSpace() {}

template<typename e_type, typename t_type>
AOS::AOSpace(basis_type bs, t_type S) :
  OrbSpace(bs, S,
           TA::diagonal_array<t_type, tensor_elems>(S.world(), S.trange()),
           TA::diagonal_array<t_type, tensor_elems>(S.world(), S.trange()), S) {
}

template<typename e_type, typename t_type>
t_type AOS::transform_from_ao(const t_type X, const size_vec& modes) const {
    return X;
}

template<typename e_type, typename t_type>
t_type AOS::transform_to_ao(const t_type X, const size_vec& modes) const {
    return X;
}

template class OrbitalSpace<float, type::tensor<float>>;
template class OrbitalSpace<float, type::tensor<double>>;
template class OrbitalSpace<double, type::tensor<float>>;
template class OrbitalSpace<double, type::tensor<double>>;

template class AOSpace<float, type::tensor<float>>;
template class AOSpace<float, type::tensor<double>>;
template class AOSpace<double, type::tensor<float>>;
template class AOSpace<double, type::tensor<double>>;

} // namespace chemist

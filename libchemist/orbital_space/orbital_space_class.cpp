#include "libchemist/orbital_space/detail_/orbital_space_pimpl.hpp"
#include "libchemist/orbital_space/orbital_space_class.hpp"
#include <random>

namespace libchemist {

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
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<double> dist;
    h(dist(rng), dist(rng), dist(rng), dist(rng));
}

#undef OrbSpace

template class OrbitalSpace<float, type::tensor<float>>;
template class OrbitalSpace<float, type::tensor<double>>;
template class OrbitalSpace<double, type::tensor<float>>;
template class OrbitalSpace<double, type::tensor<double>>;
template class OrbitalSpace<float, type::tensor_of_tensors<float>>;
template class OrbitalSpace<float, type::tensor_of_tensors<double>>;
template class OrbitalSpace<double, type::tensor_of_tensors<float>>;
template class OrbitalSpace<double, type::tensor_of_tensors<double>>;


} // namespace libchemist

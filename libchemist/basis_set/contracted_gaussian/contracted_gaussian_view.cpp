#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp"
#include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"

namespace libchemist {

template<typename T>
using cgto_pimpl = detail_::AliasingCGTOPIMPL<T>;

template<typename T>
ContractedGaussianView<T>::ContractedGaussianView() :
  ContractedGaussianView(std::vector<T*>{}, std::vector<T*>{}, nullptr, nullptr,
                         nullptr) {}

template<typename T>
ContractedGaussianView<T>::ContractedGaussianView(std::vector<T*> coefs,
                                                  std::vector<T*> exps, T* x,
                                                  T* y, T* z) :
  m_pimpl_(std::make_unique<cgto_pimpl<T>>(std::move(coefs), std::move(exps))),
  Point<T>(std::make_unique<detail_::PointPIMPL<T>>(
    const_cast<value_type*>(x), const_cast<value_type*>(y),
    const_cast<value_type*>(z))) {}

template class ContractedGaussian<double>;
template class ContractedGaussian<float>;
} // namespace libchemist

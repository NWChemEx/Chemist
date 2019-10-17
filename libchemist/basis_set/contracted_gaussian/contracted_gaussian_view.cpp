#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp"
#include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"
#include "libchemist/point/detail_/point_pimpl.hpp"

namespace libchemist {

template<typename T>
ContractedGaussianView<T>::ContractedGaussianView() :
  m_pimpl_(std::make_unique<detail_::CGTOPIMPL<no_cv_t>>(),
           std::make_unique<detail_::PointPIMPL<no_cv_t>>(nullptr, nullptr,
                                                          nullptr)) {}

template class ContractedGaussianView<double>;
template class ContractedGaussianView<const double>;
template class ContractedGaussianView<float>;
template class ContractedGaussianView<const float>;

} // namespace libchemist

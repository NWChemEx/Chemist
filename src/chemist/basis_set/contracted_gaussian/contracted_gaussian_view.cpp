#include "../../point/point_pimpl.hpp"
#include "cgto_pimpl.hpp"
#include "chemist/basis_set/contracted_gaussian_view.hpp"

namespace chemist {

template<typename T>
ContractedGaussianView<T>::ContractedGaussianView() :
  ContractedGaussianView(ContractedGaussian<std::remove_cv_t<T>>(
    std::make_unique<detail_::CGTOPIMPL<std::remove_cv_t<T>>>(),
    std::make_unique<detail_::PointPIMPL<std::remove_cv_t<T>>>(nullptr, nullptr,
                                                               nullptr))) {}

template class ContractedGaussianView<double>;
template class ContractedGaussianView<const double>;
template class ContractedGaussianView<float>;
template class ContractedGaussianView<const float>;

} // namespace chemist

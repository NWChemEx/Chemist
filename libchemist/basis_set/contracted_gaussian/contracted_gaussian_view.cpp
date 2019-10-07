#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp"
#include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"
#include "libchemist/point/detail_/point_pimpl.hpp"
#include <utilities/iter_tools/enumerate.hpp>

namespace libchemist {

template<typename T>
using cgto_pimpl = detail_::AliasingCGTOPIMPL<T>;

template<typename no_cv_t, typename T>
static auto convert_vector(std::vector<T*> input) {
    if constexpr(std::is_same_v<no_cv_t, T>) {
        return std::move(input);
    } else {
        std::vector<no_cv_t*> rv(input.size());

        for(auto[i, x] : utilities::Enumerate(input)) {
            auto y = const_cast<no_cv_t*>(x);
            rv[i]  = y;
        }
        return rv;
    }
}

template<typename T>
ContractedGaussianView<T>::ContractedGaussianView() :
  ContractedGaussianView(std::vector<T*>{}, std::vector<T*>{}, nullptr, nullptr,
                         nullptr) {}

template<typename T>
ContractedGaussianView<T>::ContractedGaussianView(std::vector<T*> coefs,
                                                  std::vector<T*> exps, T* x,
                                                  T* y, T* z) :
  m_pimpl_(std::make_unique<cgto_pimpl<no_cv_t>>(convert_vector<no_cv_t>(coefs),
                                                 convert_vector<no_cv_t>(exps)),
           std::make_unique<detail_::PointPIMPL<no_cv_t>>(
             const_cast<no_cv_t*>(x), const_cast<no_cv_t*>(y),
             const_cast<no_cv_t*>(z))) {}

template class ContractedGaussianView<double>;
template class ContractedGaussianView<const double>;
template class ContractedGaussianView<float>;
template class ContractedGaussianView<const float>;
} // namespace libchemist

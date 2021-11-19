#include "../../point/point_pimpl.hpp"
#include "chemist/basis_set/shell_view.hpp"
#include "shell_pimpl.hpp"

namespace chemist {

template<typename T>
ShellView<T>::ShellView() :
  ShellView(Shell<std::remove_cv_t<T>>(
    std::make_unique<detail_::ShellPIMPL<std::remove_cv_t<T>>>(),
    std::make_unique<detail_::PointPIMPL<std::remove_cv_t<T>>>(nullptr, nullptr,
                                                               nullptr))) {}

template class ShellView<double>;
template class ShellView<const double>;
template class ShellView<float>;
template class ShellView<const float>;

} // namespace chemist

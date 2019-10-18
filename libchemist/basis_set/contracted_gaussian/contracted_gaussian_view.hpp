#pragma once
#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian.hpp"
#include "libchemist/basis_set/detail_/view_base.hpp"

namespace libchemist {

/** @brief Models a reference to a ContractedGaussian
 *
 *  This class models a reference to a ContractedGaussian. Instances of this
 *  class are aliases and any modifications made through the API will modify
 *  the aliased instance.
 *
 *  @tparam T The cv-qualified type of the parameters stored in the aliased
 *          ContractedGaussian.
 */
template<typename T>
class ContractedGaussianView
  : public detail_::ViewBase<T, ContractedGaussian, ContractedGaussianView<T>> {
private:
    using base_type =
      detail_::ViewBase<T, ContractedGaussian, ContractedGaussianView<T>>;

public:
    /** @brief Creates a ContractedGaussianView that does not alias a
     *         ContractedGaussian.
     *
     *  This ctor is used to create a new ContractedGaussianView instance which
     *  does not alias a ContractedGaussianView instance. The resulting instance
     *  is primarily intended as a placeholder and attempting to access the
     *  members of the instance is undefined behavior. The resulting instance
     *  can be made into a valid instance by copy/move assignment.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *         PIMPL. Strong throw guarantee.
     */
    ContractedGaussianView();

    /// Brings the base's aliasing ctor into scope
    using base_type::base_type;
}; // End class contracted_gaussian

extern template class ContractedGaussianView<double>;
extern template class ContractedGaussianView<const double>;
extern template class ContractedGaussianView<float>;
extern template class ContractedGaussianView<const float>;

} // namespace libchemist

#pragma once
#include "chemist/basis_set/contracted_gaussian.hpp"
#include "chemist/basis_set/detail_/ao_basis_view_base.hpp"

namespace chemist {

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
  : public detail_::AOBasisViewBase<T, ContractedGaussian,
                                    ContractedGaussianView<T>> {
private:
    using no_cv_t   = std::remove_cv_t<T>;
    using my_type   = ContractedGaussianView<T>;
    using base_type = detail_::AOBasisViewBase<T, ContractedGaussian, my_type>;

public:
    using size_type = typename base_type::size_type;

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

    /// Brings the base's ctors into scope
    using detail_::AOBasisViewBase<T, ContractedGaussian,
                                   my_type>::AOBasisViewBase;
}; // End class contracted_gaussian

extern template class ContractedGaussianView<double>;
extern template class ContractedGaussianView<const double>;
extern template class ContractedGaussianView<float>;
extern template class ContractedGaussianView<const float>;

} // namespace chemist

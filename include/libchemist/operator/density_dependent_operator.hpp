#pragma once
#include "libchemist/operator/operator_base.hpp"
namespace libchemist {

/** @brief A class to represent density-dependent operators
 *
 *  This class allows for the identification of an operator
 *  as a density dependent operator (e.g. mean-field operators).
 *  Derived from `Operator`.
 *
 *  All density dependent operator types are to be derived from
 *  this class.
 */
struct DensityDependentOperator : public OperatorBase {
public:
    using density_type = type::tensor;

private:
    density_type m_rho_;
};

class MeanFieldElectronCoulomb : public DensityDependentOperator {};

class MeanFieldElectronExactExchange : public DensityDependentOperator {};

class MeanFieldKohnShamExchangeCorrelation : public DensityDependentOperator {};

} // namespace libchemist

#pragma once
#include "chemist/operators/detail_/operator_impl.hpp"
#include "chemist/operators/slater_type_geminal.hpp"

namespace chemist::operators {

/** @brief Models the correlation factor in F12 theories.
 *
 *  In F12 the correlation factor is the operator which couples the electrons
 *  together. It is typically either a single Slater-type geminal or a linear-
 *  combination of Gaussian functions. The parameters for the correlation factor
 *  are stored in the object of type @p Geminal, whereas the CorrelationFactor
 *  class itself models the overall operator.
 *
 *  @tparam Geminal Type of the object storing the parameters
 */
template<typename Geminal>
class CorrelationFactor
  : public detail_::OperatorImpl<CorrelationFactor, Geminal> {
private:
    /// Type of the object this class inherits from
    using base_type = detail_::OperatorImpl<CorrelationFactor, Geminal>;
    /// Type of this object
    using my_type = CorrelationFactor<Geminal>;

public:
    CorrelationFactor()                         = default;
    CorrelationFactor(const CorrelationFactor&) = default;
    CorrelationFactor(CorrelationFactor&&)      = default;
    CorrelationFactor& operator=(const CorrelationFactor&) = default;
    CorrelationFactor& operator=(CorrelationFactor&&) = default;
    CorrelationFactor(Geminal g) : base_type(std::move(g)) {}

    /** @brief Makes a new CorrelationFactor equal to the product of this one
     *         and @p rhs.
     *
     *  This function multiples this instance's geminal by the geminal in
     *  @p rhs. The resulting geminal is then returned in a new
     *  CorrelationFactor instance.
     *
     *  @param[in] rhs The CorrelationFactor we are multiplying this instance
     *                 by.
     *
     *  @return The CorrelationFactor which results from multiplying this
     *          instance by @p rhs.
     */
    my_type operator*(const my_type& rhs) const;

protected:
    /// Overrides to be consistent with typical notation
    std::string as_string_impl() const override { return "f\u0302₁₂"; }
};

/// CorrelationFactor which uses a two-electron STG
using ElectronElectronSTG = CorrelationFactor<STG>;

// -----------------------------------------------------------------------------
// --------------------- Inline Implementations --------------------------------
// -----------------------------------------------------------------------------

#define CORRELATION_FACTOR CorrelationFactor<Geminal>

template<typename Geminal>
CORRELATION_FACTOR CORRELATION_FACTOR::operator*(
  const CorrelationFactor& rhs) const {
    const auto& lhs_geminal = this->template at<0>();
    const auto& rhs_geminal = rhs.template at<0>();
    const auto f12_squared  = lhs_geminal * rhs_geminal;
    return my_type(std::move(f12_squared));
}

#undef CORRELATION_FACTOR

extern template class CorrelationFactor<STG>;

} // namespace chemist::operators

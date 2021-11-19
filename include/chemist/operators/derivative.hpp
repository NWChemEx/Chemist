#pragma once
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

/** @brief Models a derivative
 *
 *  @tparam Numerator The type of the numerator of the derivative.
 *  @tparam Denominators The type(s) of the denominator(s) of the derivative.
 */
template<typename Numerator, typename... Denominators>
class Derivative
  : public detail_::OperatorImpl<Derivative, Numerator, Denominators...> {
private:
    using base_type =
      detail_::OperatorImpl<Derivative, Numerator, Denominators...>;

public:
    Derivative()                  = default;
    Derivative(const Derivative&) = default;
    Derivative(Derivative&&)      = default;
    Derivative(Numerator p1, Denominators... ps) :
      base_type(std::move(p1), std::move(ps)...) {}
    Derivative& operator=(const Derivative&) = default;
    Derivative& operator=(Derivative&&) = default;
};

} // namespace chemist::operators

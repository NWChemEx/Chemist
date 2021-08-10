#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"

namespace libchemist::operators {

template<typename Numerator, typename... Denominators>
class Derivative
  : public detail_::OperatorImpl<Derivative, Numerator, Denominators...> {
private:
    using base_type =
      detail_::OperatorImpl<Derivative, Numerator, Denominators...>;

public:
    using base_type::base_type;
};

} // namespace libchemist::operators

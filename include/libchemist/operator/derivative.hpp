#pragma once

namespace libchemist {

template<typename Numerator, typename... Denominators>
class Derivative : public OperatorImpl<Derivative, Numerator, Denominators...> {
private:
    using base_type = OperatorImpl<Derivative, Numerator, Denominators...>;

public:
    using base_type::base_type;
};

} // namespace libchemist

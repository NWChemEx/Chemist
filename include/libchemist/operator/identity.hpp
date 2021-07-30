#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist {

template<typename... Particles>
class Identity : public detail_::OperatorImpl<Identity, Particles...> {
private:
    using base_type = detail_::OperatorImpl<Identity, Particles...>;

public:
    using base_type::OperatorImpl;
};

using ElectronIdentity = Identity<Electron>;

} // namespace libchemist

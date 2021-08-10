#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist::operators {

template<typename... Particles>
class Identity : public detail_::OperatorImpl<Identity, Particles...> {
private:
    using base_type = detail_::OperatorImpl<Identity, Particles...>;

public:
    Identity()                = default;
    Identity(const Identity&) = default;
    Identity(Identity&&)      = default;
    Identity(Particles... ps) : base_type(std::move(ps)...) {}
    Identity& operator=(const Identity&) = default;
    Identity& operator=(Identity&&) = default;

protected:
    std::string as_string_impl() const { return "I\u0302"; }
};

using ElectronIdentity = Identity<Electron>;

} // namespace libchemist::operators

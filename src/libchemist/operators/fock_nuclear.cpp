#include "libchemist/operators/fock_nuclear.hpp"

namespace libchemist::operators {

Fock_Nuclear::Fock_Nuclear() noexcept  = default;
Fock_Nuclear::~Fock_Nuclear() noexcept = default;

Fock_Nuclear::Fock_Nuclear(const Fock_Nuclear& other)     = default;
Fock_Nuclear::Fock_Nuclear(Fock_Nuclear&& other) noexcept = default;
Fock_Nuclear& Fock_Nuclear::operator=(const Fock_Nuclear& other) = default;
Fock_Nuclear& Fock_Nuclear::operator=(Fock_Nuclear&& other) noexcept = default;

bool Fock_Nuclear::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Fock_Nuclear*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Fock_Nuclear::clone_impl() const {
    return std::make_unique<Fock_Nuclear>(*this);
}

} // namespace libchemist::operators

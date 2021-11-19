#include "chemist/operators/fock.hpp"

namespace chemist::operators {

Fock::Fock() noexcept  = default;
Fock::~Fock() noexcept = default;

Fock::Fock(const Fock& other)     = default;
Fock::Fock(Fock&& other) noexcept = default;
Fock& Fock::operator=(const Fock& other) = default;
Fock& Fock::operator=(Fock&& other) noexcept = default;

bool Fock::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Fock*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Fock::clone_impl() const {
    return std::make_unique<Fock>(*this);
}

} // namespace chemist::operators

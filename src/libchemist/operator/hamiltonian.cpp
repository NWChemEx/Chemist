#include "libchemist/operator/hamiltonian.hpp"

namespace libchemist::operators {

Hamiltonian::Hamiltonian() noexcept  = default;
Hamiltonian::~Hamiltonian() noexcept = default;

Hamiltonian::Hamiltonian(const Hamiltonian& other)     = default;
Hamiltonian::Hamiltonian(Hamiltonian&& other) noexcept = default;
Hamiltonian& Hamiltonian::operator=(const Hamiltonian& other) = default;

Hamiltonian& Hamiltonian::operator=(Hamiltonian&& other) noexcept = default;

bool Hamiltonian::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Hamiltonian*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Hamiltonian::clone_impl() const {
    return std::make_unique<Hamiltonian>(*this);
}

} // namespace libchemist::operators

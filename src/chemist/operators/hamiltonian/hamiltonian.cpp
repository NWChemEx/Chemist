#include "chemist/operators/hamiltonian/hamiltonian.hpp"
#include "chemist/operators/kinetic.hpp"

namespace chemist::operators {

using size_type = typename Hamiltonian::size_type;

Hamiltonian::Hamiltonian() noexcept  = default;
Hamiltonian::~Hamiltonian() noexcept = default;

Hamiltonian::Hamiltonian(const Hamiltonian& other)     = default;
Hamiltonian::Hamiltonian(Hamiltonian&& other) noexcept = default;
Hamiltonian& Hamiltonian::operator=(const Hamiltonian& other) = default;

Hamiltonian& Hamiltonian::operator=(Hamiltonian&& other) noexcept = default;

size_type Hamiltonian::nelectrons() const noexcept {
    using T_type = NElectronKinetic;
    if(!has_term<T_type>()) return 0;
    auto T = get_terms<T_type>();
    return T[0]->at<0>().size;
}

bool Hamiltonian::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Hamiltonian*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Hamiltonian::clone_impl() const {
    return std::make_unique<Hamiltonian>(*this);
}

} // namespace chemist::operators

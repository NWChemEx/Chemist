#include "libchemist/operators/hamiltonian/hamiltonian_nuclear.hpp"
#include "libchemist/operators/kinetic_nuclear.hpp"

namespace libchemist::operators {

using size_type = typename Hamiltonian_Nuclear::size_type;

Hamiltonian_Nuclear::Hamiltonian_Nuclear() noexcept  = default;
Hamiltonian_Nuclear::~Hamiltonian_Nuclear() noexcept = default;

Hamiltonian_Nuclear::Hamiltonian_Nuclear(const Hamiltonian_Nuclear& other)     = default;
Hamiltonian_Nuclear::Hamiltonian_Nuclear(Hamiltonian_Nuclear&& other) noexcept = default;
Hamiltonian_Nuclear& Hamiltonian_Nuclear::operator=(const Hamiltonian_Nuclear& other) = default;

Hamiltonian_Nuclear& Hamiltonian_Nuclear::operator=(Hamiltonian_Nuclear&& other) noexcept = default;

size_type Hamiltonian_Nuclear::nelectrons() const noexcept {
    using T_type = NElectronKinetic_Nuclear;
    if(!has_term<T_type>()) return 0;
    auto T = get_terms<T_type>();
    return T[0]->at<0>().size;
}

bool Hamiltonian_Nuclear::is_equal_impl(const OperatorBase& other) const noexcept {
    auto ptr = dynamic_cast<const Hamiltonian_Nuclear*>(&other);
    if(!ptr) return false;
    return *this == *ptr;
}

std::unique_ptr<detail_::OperatorBase> Hamiltonian_Nuclear::clone_impl() const {
    return std::make_unique<Hamiltonian_Nuclear>(*this);
}

} // namespace libchemist::operators

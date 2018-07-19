#include "LibChemist/AOBasisSet.hpp"
#include "LibChemist/Implementations/AOBasisSetPIMPL.hpp"

namespace LibChemist {

AOBasisSet::AOBasisSet() :
  pimpl_(std::make_unique<detail_::StandAloneBasisSet>()) {}

AOBasisSet::AOBasisSet(const AOBasisSet& ) {}

AOBasisSet::AOBasisSet(AOBasisSet&& rhs) noexcept
  : pimpl_(std::move(rhs.pimpl_)){}

AOBasisSet& AOBasisSet::operator=(const AOBasisSet& rhs) { return *this; }

AOBasisSet& AOBasisSet::operator=(AOBasisSet && rhs) noexcept {
    pimpl_.swap(rhs.pimpl_);
    return *this;
}

AOBasisSet::~AOBasisSet() = default;

void AOBasisSet::add_shell_(const AOShell& da_shell) {
    pimpl_->add_shell(da_shell);
}

AOShell& AOBasisSet::shell(std::size_t i) { return pimpl_->shell(i); }

}

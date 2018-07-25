#include "LibChemist/AOBasisSet.hpp"
#include "LibChemist/Implementations/AOBasisSetPIMPL.hpp"
#include <algorithm> //For max element

namespace LibChemist {


using size_type = typename AOBasisSet::size_type;
using iterator = typename AOBasisSet::iterator;
using const_iterator = typename AOBasisSet::const_iterator;

AOBasisSet::AOBasisSet() :
  pimpl_(std::make_unique<detail_::ContiguousBasisSet>()) {}

AOBasisSet::AOBasisSet(std::initializer_list<AOShell> il) : AOBasisSet() {
    for(auto& shelli : il)add_shell(shelli);
}

AOBasisSet::AOBasisSet(const AOBasisSet& rhs) : AOBasisSet() {
    for(size_type i = 0; i < rhs.size(); ++i) add_shell(rhs[i]);
}

AOBasisSet::AOBasisSet(AOBasisSet&& rhs) noexcept
  : pimpl_(std::move(rhs.pimpl_)){}

AOBasisSet& AOBasisSet::operator=(const AOBasisSet& rhs) {
    return *this = std::move(AOBasisSet(rhs));
}

AOBasisSet& AOBasisSet::operator=(AOBasisSet && rhs) noexcept {
    pimpl_.swap(rhs.pimpl_);
    return *this;
}

AOBasisSet::~AOBasisSet() noexcept = default;

void AOBasisSet::add_shell(const AOShell& da_shell) {
    pimpl_->add_shell(da_shell);
}

size_type AOBasisSet::size() const noexcept { return pimpl_->size(); }
AOShell& AOBasisSet::shell(std::size_t i) noexcept { return pimpl_->shell(i); }

iterator AOBasisSet::begin() noexcept { return pimpl_->begin(); }
iterator AOBasisSet::end() noexcept {return pimpl_->end(); }
const_iterator AOBasisSet::begin() const noexcept { return pimpl_->begin(); }
const_iterator AOBasisSet::end() const noexcept {return pimpl_->end(); }


std::pair<size_type, iterator> max_l(AOBasisSet& bs) noexcept {
    auto itr = std::max_element(bs.begin(), bs.end(),
      [](const AOShell& shelli, const AOShell& shellj) {
        return shelli.l() < shellj.l(); });
    return itr != bs.end() ? std::make_pair(itr->l(), itr) :
                             std::make_pair(0ul, itr);
};

std::pair<size_type, const_iterator> max_l(const AOBasisSet& bs) noexcept {
    auto itr = std::max_element(bs.begin(), bs.end(),
      [](const AOShell& shelli, const AOShell& shellj) {
        return shelli.l() < shellj.l(); });
    return itr != bs.end() ? std::make_pair(itr->l(), itr) :
                             std::make_pair(0ul, itr);
};

}

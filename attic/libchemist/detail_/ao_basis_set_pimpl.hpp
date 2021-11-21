#pragma once
#include "chemist/ao_basis_set.hpp"
#include "chemist/detail_/ao_shell_pimpl.hpp"
#include "chemist/detail_/container_pimpl.hpp"

namespace chemist::detail_ {

class ContiguousBasisSet : public ContainerPIMPL<AOBasisSet> {
public:
    ContiguousBasisSet()                              = default;
    ContiguousBasisSet(const ContiguousBasisSet& rhs) = default;

private:
    using AoS_t = typename ContiguousAOShell::AoS_t;

    AoSFacade<AoS_t> impl_;
    std::vector<value_type> shells_;

    std::unique_ptr<ContainerPIMPL<AOBasisSet>> clone_() const override {
        return std::make_unique<ContiguousBasisSet>(*this);
    }
    reference at_(size_type i) override { return shells_[i]; }
    size_type size_() const noexcept override { return impl_.size(); }
    void push_back_(value_type shell) override {
        AoS_t si;
        si.insert<0>(shell.center());
        si.insert<1>(shell.l());
        for(size_type i = 0; i < shell.nprims(); ++i) {
            si.insert<2>(shell.coef(i));
            si.insert<3>(shell.alpha(i));
        }
        auto ptr    = std::make_unique<ContiguousAOShell>(*impl_.insert(si));
        ptr->pure() = shell.pure();
        shells_.push_back(value_type(std::move(ptr)));
    }
    iterator begin_() noexcept override { return shells_.begin(); }
    iterator end_() noexcept override { return shells_.end(); }
    const_iterator begin_() const noexcept override { return shells_.cbegin(); }
    const_iterator end_() const noexcept override { return shells_.cend(); }
};

} // namespace chemist::detail_

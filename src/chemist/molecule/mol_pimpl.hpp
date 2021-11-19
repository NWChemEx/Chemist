#pragma once
#include "../detail_/container_pimpl.hpp"
#include "atom_pimpl.hpp"
#include "chemist/molecule/molecule.hpp"

namespace chemist::detail_ {

class MolPIMPL : public ContainerPIMPL<Molecule> {
public:
    ///@{
    double& charge() noexcept { return charge_(); }
    size_type& multiplicity() noexcept { return multiplicity_(); }
    ///@}
private:
    virtual double& charge_() noexcept          = 0;
    virtual size_type& multiplicity_() noexcept = 0;
};

class ContiguousMol : public MolPIMPL {
private:
    using AoS_t = typename ContiguousAtomPIMPL::AoS_t;

    AoSFacade<AoS_t> impl_;
    std::vector<value_type> atoms_;
    double my_charge_  = 0.0;
    size_type my_mult_ = 1ul;

    std::unique_ptr<ContainerPIMPL<Molecule>> clone_() const override {
        return std::make_unique<ContiguousMol>(*this);
    }
    reference at_(size_type i) override { return atoms_[i]; }
    size_type size_() const noexcept override { return impl_.size(); }
    void push_back_(value_type atom) override {
        AoS_t ai;
        ai.insert<0>(atom.Z());
        ai.insert<1>(atom.mass());
        ai.insert<2>(atom.coords());
        auto ptr    = std::make_unique<ContiguousAtomPIMPL>(*impl_.insert(ai));
        ptr->name() = atom.name();
        atoms_.push_back(Atom(std::move(ptr)));
    }
    iterator begin_() noexcept override { return atoms_.begin(); }
    iterator end_() noexcept override { return atoms_.end(); }
    const_iterator begin_() const noexcept override { return atoms_.cbegin(); }
    const_iterator end_() const noexcept override { return atoms_.cend(); }

    double& charge_() noexcept override { return my_charge_; }
    size_type& multiplicity_() noexcept override { return my_mult_; }
};

} // namespace chemist::detail_

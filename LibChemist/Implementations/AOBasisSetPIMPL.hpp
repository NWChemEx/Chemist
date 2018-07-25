#pragma once
#include "LibChemist/AOBasisSet.hpp"
#include "LibChemist/Implementations/AOShellPIMPL.hpp"

namespace LibChemist::detail_ {

class AOBasisSetPIMPL {
public:
    using size_type = typename AOBasisSet::size_type;
    using iterator = typename AOBasisSet::iterator;
    using const_iterator = typename AOBasisSet::const_iterator;

    /**
     * @defgroup Public API of an AOBasisSetPIMPL
     *
     * The functions in this section have the same documentation as the
     * corresponding members of the AOBasisSet class.
     */
    ///@{
    AOShell& shell(size_type i) { return shell_(i); }
    size_type size()const noexcept {return size_(); }
    void add_shell(const AOShell& da_shell) { add_shell_(da_shell); }
    iterator begin() noexcept { return begin_(); }
    iterator end() noexcept { return end_(); }
    const_iterator begin() const noexcept { return begin_(); }
    const_iterator end() const noexcept {return end_(); }
    ///@}
private:
    virtual AOShell& shell_(size_type i) = 0;
    virtual size_type size_() const noexcept = 0;
    virtual void add_shell_(const AOShell& da_shell) = 0;
    virtual iterator begin_() noexcept = 0;
    virtual iterator end_() noexcept = 0;
    virtual const_iterator begin_() const noexcept = 0;
    virtual const_iterator end_() const noexcept = 0;
};

class ContiguousBasisSet : public AOBasisSetPIMPL {
private:
    using coord_type = std::array<double, 3>;
    using Shell = typename ContiguousAOShell::AoS_t;

    AoSFacade<Shell> impl_;
    std::vector<AOShell> shells_;

    AOShell& shell_(size_type i) override { return shells_[i]; }

    size_type size_() const noexcept override { return impl_.size(); }

    void add_shell_(const AOShell& shell) override {
        Shell si;
        si.insert<0>(shell.center());
        si.insert<1>(shell.l());
        for(size_type i=0; i <shell.nprims(); ++i) {
            si.insert<2>(shell.coef(i));
            si.insert<3>(shell.alpha(i));
        }
        impl_.insert(si);
        const auto nshells = size_();
        auto ptr = std::make_unique<ContiguousAOShell>(impl_.get(nshells -1));
        shells_.push_back(AOShell(std::move(ptr)));
    }

    iterator begin_() noexcept override { return shells_.begin(); }
    iterator end_() noexcept override {return shells_.end(); }
    const_iterator begin_() const noexcept override { return shells_.cbegin(); }
    const_iterator end_() const noexcept override  { return shells_.cend(); }
};

} //namespace LibChemist::detail_

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

class StandAloneBasisSet : public AOBasisSetPIMPL {
private:
    using coord_type = std::array<double, 3>;

    std::vector<double> carts_;
    std::vector<size_type> nprims_;
    std::vector<double> c_ij_;
    std::vector<double> alphas_;

    std::vector<AOShell> shells_;

    AOShell& shell_(size_type i) override { return shells_[i]; }

    size_type size_() const noexcept override { return shells_.size(); }

    void add_shell_(const AOShell& shell) override {
        const auto np = shell.nprims();
        nprims_.push_back(np);

        // The biggest gotcha here is that we need to reset the pointers for
        // each shell if any of the vectors reallocate.  We check this by
        // watching their capacities
        bool redo_pointers = false;

        size_type orig = carts_.capacity();
        for(size_type i=0; i < 3; ++i) carts_.push_back(shell.center()[i]);
        if(orig != carts_.capacity()) redo_pointers = true;

        orig = c_ij_.capacity();
        for(size_type i=0; i < np; ++i)
            c_ij_.push_back(shell.coef(i));
        if(orig != c_ij_.capacity()) redo_pointers = true;

        orig = alphas_.capacity();
        for(size_type i=0; i < np; ++i)
            alphas_.push_back(shell.alpha(i));
        if(orig != alphas_.capacity()) redo_pointers = true;

        shells_.push_back(shell); //It's a normal shell for the moment...

        // We minimally need to set the pointer of the shell we just made
        // which is the "shells_.size() - 1" -th shell, resetting is the same
        // process so we combine them

        size_type si = redo_pointers ? 0 : shells_.size() - 1;
        for(size_type pi = 0; si < shells_.size(); ++si) {
            //Dirty hack, in memory 3 consecutive doubles look just like
            // std::array<double, 3>, static assert protects us if that changes
            static_assert(sizeof(coord_type) == 3*sizeof(double),
                "Oh Oh, looks like the STL changed std::array...");
            coord_type* carts = reinterpret_cast<coord_type*>(&carts_[si*3]);
            double* c_ij = &c_ij_[pi];
            double* alpha = &alphas_[pi];
            const auto npi = nprims_[si];
            auto ptr =
              std::make_unique<detail_::AliasAOShell>(carts, c_ij, alpha, npi);
            ptr->pure() = shells_[si].pure();
            ptr->l() = shells_[si].l();
            pi += npi;
            shells_[si] = AOShell(std::move(ptr));
        }
    }

    iterator begin_() noexcept override { return shells_.begin(); }
    iterator end_() noexcept override {return shells_.end(); }
    const_iterator begin_() const noexcept override { return shells_.cbegin(); }
    const_iterator end_() const noexcept override  { return shells_.cend(); }
};

} //namespace LibChemist::detail_

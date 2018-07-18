#pragma once
#include "LibChemist/AOShell.hpp"
#include <Utilities/Mathematician/Combinatorics.hpp> //For binomial coefficient

namespace LibChemist::detail_ {

class AOShellPIMPL {
public:
    using size_type = typename AOShell::size_type;
    using coord_type = typename AOShell::coord_type;

    AOShellPIMPL() = default;
    virtual ~AOShellPIMPL() = default;

    std::unique_ptr<AOShellPIMPL> clone()const { return clone_(); }

    /**
     * @defgroup Public API for PIMPL implementation functions
     *
     * @brief Members of this group are ultimately called by the members of the
     *        AOShell class.
     *
     * Documentation for all members in this section mirrors that of the AOShell
     * class.
     */
    ///@{
    bool& pure() noexcept{ return pure_(); }

    size_type size() const {
        return pure()? 2ul * l() + 1ul :
               Utilities::binomial_coefficient<size_type>(2ul + l() , l());
    }
    size_type& l() noexcept { return l_(); }
    coord_type& center() noexcept { return center_(); }
    double& coef(size_type i) noexcept { return coef_(i); }
    double& alpha(size_type i) noexcept {return alpha_(i); }
    void add_prim(double alpha, double c) { add_prim_(alpha, c); }
    ///@}
protected:
    AOShellPIMPL(const AOShellPIMPL& rhs) = default;
    AOShellPIMPL(AOShellPIMPL&& rhs) = delete;
    AOShellPIMPL& operator=(const AOShellPIMPL& rhs) = default;
    AOShellPIMPL& operator=(AOShellPIMPL&& rhs) = delete;
private:
    bool is_pure_ = true;
    size_type ang_mom_ = 0;

    /**
     * @defgroup Hooks for derived class.
     *
     * The members in this section can be implemented in the derived class to
     * return their namesake quantities subject to the API guidelines laid out
     * in the AOShell class documentation.
     */
    ///@{
    virtual std::unique_ptr<AOShellPIMPL&> clone_() const = 0;

    virtual bool& pure_() noexcept { return is_pure_; }

    virtual size_type& l_() noexcept { return ang_mom_; }

    virtual coord_type& center_() noexcept = 0;

    virtual double& coef_(size_type) noexcept = 0;

    virtual double& alpha_(size_type) noexcept = 0;

    virtual void add_prim_(double, double) = 0;
    ///@}
};

class StandAloneAOShell : public AOShellPIMPL {
private:
    coord_type carts_;
    std::vector<double> c_ij_;
    std::vector<double> alphas_;

    std::unique_ptr<AOShellPIMPL> clone_() const override {
        return std::unique_ptr<AOShellPIMPL>(new StandAloneAOShell(*this));
    }

    coord_type& center_() noexcept override { return carts_; }

    double& coef_(size_type i) noexcept override { return c_ij_[i]; }

    double& alpha_(size_type i) noexcept override { return alphas_[i]; }

    void add_prim_(double alpha, double c) override {
        alphas_.push_back(alpha);
        c_ij_.push_back(c);
    }
};

} // namespace LibChemist::detail_

#pragma once
#include "chemist/ao_shell.hpp"
#include "chemist/detail_/aos_facade.hpp"
#include <vector>

namespace chemist::detail_ {

/// Defines the API for a PIMPL that can run an AOShell instance
class AOShellPIMPL {
public:
    /// Typedefs from the AOShell class
    ///@{
    using size_type  = typename AOShell::size_type;
    using coord_type = typename AOShell::coord_type;
    ///@}

    /// Default ctor, no state
    AOShellPIMPL() = default;

    /// Default dtor, no state
    virtual ~AOShellPIMPL() = default;

    /// Provides a polymorphic copy of the PIMPL
    std::unique_ptr<AOShellPIMPL> clone() const { return clone_(); }

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
    bool& pure() noexcept { return pure_(); }
    size_type nprims() const noexcept { return nprims_(); }
    size_type& l() noexcept { return l_(); }
    coord_type& center() noexcept { return center_(); }
    double& coef(size_type i) noexcept { return coef_(i); }
    double& alpha(size_type i) noexcept { return alpha_(i); }
    void add_prim(double alpha, double c) { add_prim_(alpha, c); }
    ///@}
protected:
    /**
     * @defgroup Copy/Move ctors and assignment operators
     * @brief Functions for copying and moving an AOShell PIMPL
     *
     * Given the polymorphic nature of this class we have disabled moving and
     * insisted that copying goes through the clone() function.  The copy ctor
     * is protected so that derived classes can use it to implement the clone
     * function.
     */
    ///@{
    AOShellPIMPL(const AOShellPIMPL& rhs) = default;
    AOShellPIMPL(AOShellPIMPL&& rhs)      = delete;
    AOShellPIMPL& operator=(const AOShellPIMPL& rhs) = delete;
    AOShellPIMPL& operator=(AOShellPIMPL&& rhs) = delete;
    ///@}
private:
    /**
     * @defgroup Hooks for derived class.
     *
     * The members in this section can be implemented in the derived class to
     * return their namesake quantities subject to the API guidelines laid out
     * in the AOShell class documentation.
     */
    ///@{
    virtual std::unique_ptr<AOShellPIMPL> clone_() const = 0;

    virtual bool& pure_() noexcept = 0;

    virtual size_type& l_() noexcept = 0;

    virtual size_type nprims_() const noexcept = 0;

    virtual coord_type& center_() noexcept = 0;

    virtual double& coef_(size_type) noexcept = 0;

    virtual double& alpha_(size_type) noexcept = 0;

    virtual void add_prim_(double, double) = 0;
    ///@}
};

/// Implementation assuming AOShell is stored in an SoA
class ContiguousAOShell : public AOShellPIMPL {
public:
    /// Type of an AoSElement that holds an AOShell's state
    using AoS_t = AoSElement<coord_type, size_type, double, double>;

    /// Makes a spherical AOShell centered at the origin with l=0
    ContiguousAOShell() : impl_(std::make_shared<AoS_t>()), is_pure_(true) {
        impl_->insert<0>(coord_type{});
        impl_->insert<1>(0);
    }

    /// Builds an AOShell around an already exisiting state
    ContiguousAOShell(std::shared_ptr<AoS_t> impl) : impl_(impl) {}

protected:
    /// Deep copies the shell
    ContiguousAOShell(const ContiguousAOShell& rhs) :
      impl_(std::make_shared<AoS_t>(*rhs.impl_)), is_pure_(rhs.is_pure_) {}

private:
    /// The actual state of the shell
    std::shared_ptr<AoS_t> impl_;
    /// Whether the shell is spherical or Cartesian
    bool is_pure_ = true;

    /// detail_ of the AOShellPIMPL's API
    ///@{
    std::unique_ptr<AOShellPIMPL> clone_() const override {
        return std::unique_ptr<ContiguousAOShell>(new ContiguousAOShell(*this));
    }
    size_type nprims_() const noexcept { return impl_->size<2>(); }
    bool& pure_() noexcept override { return is_pure_; }
    size_type& l_() noexcept override { return impl_->at<1>(); }
    coord_type& center_() noexcept override { return impl_->at<0>(); }
    double& coef_(size_type i) noexcept override { return impl_->at<2>(i); }
    double& alpha_(size_type i) noexcept override { return impl_->at<3>(i); }
    void add_prim_(double alpha, double c) override {
        impl_->insert<3>(alpha);
        impl_->insert<2>(c);
    }
    ///@}
};

} // namespace chemist::detail_

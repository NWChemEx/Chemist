#pragma once
#include "libchemist/managers/basis_set_manager.hpp"

namespace libchemist::detail_ {

/// Class defining the API for the implementation of the BasisSetManager
class BasisSetManagerPIMPL {
public:
    /// Forward typedefs from main class
    ///@{
    using ao_basis_type = typename BasisSetManager::ao_basis_type;
    using size_type     = typename BasisSetManager::size_type;
    ///@}

    /// Type of a unique_ptr to this class
    using unique_me = std::unique_ptr<BasisSetManagerPIMPL>;

    /// Default ctor does nothing
    BasisSetManagerPIMPL() = default;

    /// Deleted functions to prevent slicing
    ///@{
    BasisSetManagerPIMPL(BasisSetManagerPIMPL&& rhs) = delete;
    BasisSetManagerPIMPL& operator=(const BasisSetManagerPIMPL& rhs) = delete;
    BasisSetManagerPIMPL& operator=(BasisSetManagerPIMPL&& rhs) = delete;
    ///@}

    /// Default dtor
    virtual ~BasisSetManagerPIMPL() = default;

    /// Polymorphic copy function
    unique_me clone() const { return clone_(); }

    /// Functions comprising the public API of the PIMPL
    ///@{
    ao_basis_type get_basis(const std::string& name, size_type Z) const {
        return get_basis_(name, Z);
    }
    ///@}

protected:
    /// protected to allow derived class to use for clone_ implementation
    BasisSetManagerPIMPL(const BasisSetManagerPIMPL& rhs) = default;

private:
    /// Hooks to be implemented by derived class
    ///@{
    virtual unique_me clone_() const                    = 0;
    virtual ao_basis_type get_basis_(const std::string& name,
                                     size_type Z) const = 0;
    ///@}
};

/// Returns the default NWX PIMPL
std::unique_ptr<BasisSetManagerPIMPL> nwx_default_bs();

} // namespace libchemist::detail_

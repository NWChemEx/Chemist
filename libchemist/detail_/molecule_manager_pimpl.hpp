#pragma once
#include "libchemist/molecule_manager.hpp"
#include "libchemist/periodic_table.hpp"

namespace libchemist::detail_ {

/// Class defining the public API of a MoleculeManagerPIMPL
class MoleculeManagerPIMPL {
public:
    /// Forward typedefs
    ///@{
    using key_type   = typename MoleculeManager::key_type;
    using value_type = typename MoleculeManager::value_type;
    ///@}

    /// Typedef for unique_ptr to this class
    using unique_pimpl = std::unique_ptr<MoleculeManagerPIMPL>;

    /// Default ctor, makes a default ptable_
    explicit MoleculeManagerPIMPL() : MoleculeManagerPIMPL(PeriodicTable{}) {}

    explicit MoleculeManagerPIMPL(PeriodicTable ptable) :
      ptable_(std::move(ptable)) {}

    /// Deleted to avoid slicing the derived class's state
    ///@{
    MoleculeManagerPIMPL(MoleculeManagerPIMPL&& rhs) = delete;
    MoleculeManagerPIMPL& operator=(const MoleculeManagerPIMPL& rhs) = delete;
    MoleculeManagerPIMPL& operator=(MoleculeManagerPIMPL&& rhs) = delete;
    ///@}

    /// Default dtor, frees up ptable_
    virtual ~MoleculeManagerPIMPL() = default;

    /// Polymorphic copy operator
    unique_pimpl clone() const { return clone_(); }

    /// Defines public API, all functions work the same as they do for the
    /// MoleculeManager
    ///@{
    value_type at(const key_type& name) const { return at_(name); }
    ///@}
protected:
    /// The periodic table instance derived classes should use
    PeriodicTable ptable_;

    /// Protected to allow use in clone_ implementation
    MoleculeManagerPIMPL(const MoleculeManagerPIMPL& rhs) = default;

private:
    /// To be implemented by the derived class
    ///@{
    virtual unique_pimpl clone_() const                = 0;
    virtual value_type at_(const key_type& name) const = 0;
    ///@}
};

/// Returns NWX's default PIMPL
std::unique_ptr<MoleculeManagerPIMPL> nwx_default_mols();

} // namespace libchemist::detail_

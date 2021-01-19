#pragma once
#include "libchemist/managers/periodic_table.hpp"

namespace libchemist::detail_ {

/// Class defining the public API of a PIMPL for the PeriodicTable class
class PeriodicTablePIMPL {
public:
    /// Forward typedefs from PeriodicTable
    ///@{
    using size_type    = typename PeriodicTable::size_type;
    using isotope_list = typename PeriodicTable::isotope_list;
    ///@}

    PeriodicTablePIMPL()                         = default;
    PeriodicTablePIMPL(PeriodicTablePIMPL&& rhs) = delete;
    PeriodicTablePIMPL& operator=(const PeriodicTablePIMPL& rhs) = delete;
    PeriodicTablePIMPL& operator=(PeriodicTablePIMPL&& rhs) = delete;
    virtual ~PeriodicTablePIMPL()                           = default;

    /// Makes a polymorphic copy of the PIMPL
    std::unique_ptr<PeriodicTablePIMPL> clone() const { return clone_(); }

    /// Functions defining the public API of the PeriodicTablePIMPL
    ///@{
    size_type sym_2_Z(const std::string& sym) const { return sym_2_Z_(sym); }
    size_type max_Z() const noexcept { return max_Z_(); }
    isotope_list isotopes(size_type Z) const { return isotopes_(Z); }
    Atom get_atom(size_type Z) const { return get_atom_(Z); }
    Atom get_isotope(size_type Z, size_type mass_num) const {
        return get_isotope_(Z, mass_num);
    }
    ///@}
protected:
    PeriodicTablePIMPL(const PeriodicTablePIMPL& rhs) = default;

private:
    /// To be implemented by the derived class
    ///@{
    virtual std::unique_ptr<PeriodicTablePIMPL> clone_() const       = 0;
    virtual size_type sym_2_Z_(const std::string& sym) const         = 0;
    virtual size_type max_Z_() const noexcept                        = 0;
    virtual isotope_list isotopes_(size_type Z) const                = 0;
    virtual Atom get_atom_(size_type Z) const                        = 0;
    virtual Atom get_isotope_(size_type Z, size_type mass_num) const = 0;
    ///@}
};

/// Creates a PeriodicTablePIMPL instance w/NWChemEX defaults
std::unique_ptr<PeriodicTablePIMPL> nwx_default_ptable();

} // namespace libchemist::detail_

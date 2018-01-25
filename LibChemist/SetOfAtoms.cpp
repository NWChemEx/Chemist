#include "LibChemist/SetOfAtoms.hpp"
namespace LibChemist {

BasisSet get_general_basis(const std::string& name, const SetOfAtoms& atoms)
{
    BasisSet rv;
    for(const Atom& ai : atoms)
        rv = basis_set_concatenate(rv, ai.get_basis(name));
    return rv;
}

BasisSet get_basis(const std::string& name, const SetOfAtoms& atoms)
{
    BasisSet rv;
    for(const Atom& ai : atoms)
        rv = basis_set_concatenate(rv,
                                   ungeneralize_basis_set(ai.get_basis(name)));
    return rv;
}

SetOfAtoms apply_basis_set(const std::string& name,
                           const std::map<size_t, std::vector<BasisShell>>& bs,
                           const SetOfAtoms& atoms)
{
    SetOfAtoms rv(atoms);
    for(Atom& ai : rv)
    {
        size_t Z = ai.property(AtomProperty::Z);
        if(bs.count(Z))
        {
            for(const BasisShell& si : bs.at(Z))
                ai.add_shell(name, si);
        }
    }
    return rv;
}

} // namespace LibChemist

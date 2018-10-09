"""
This script will turn each of the xyz files in "molecules" into the C++ commands
to make said molecule in a source file
"""
import os
import re
from generate_atomicinfo import parse_symbols

class Molecule:
    def __init__(self):
        self.carts = []
        self.atoms = []

    def add_atom(self, Z, carts):
        self.atoms.append(Z)
        for q in carts:
            self.carts.append(q)

    def __repr__(self):
        rv = ""
        for i, ai in enumerate(self.atoms):
            rv += ai + " "
            for j in range(3):
                rv += str(self.carts[i*3 + j]) + " "
            rv += "\n"
        return rv

    def cxxify(self, tab, f):
        tab2 = tab*2
        f.write("Molecule(\n")
        for i, ai in enumerate(self.atoms):
            f.write(
"""{}Atom{{mass_t(ptable_.get_atom({}).mass()), {}ul,
{}cart_t{{""".format(tab2, ai, ai, tab2 + "     "))
            line = ""
            line = "{}cart_t{{".format(tab2 + "     ")
            line+="{}, {},".format(self.carts[i*3], self.carts[i*3+1])
            line +=" {}}},".format(self.carts[i*3+2])            
            f.write("{}, {},".format(self.carts[i*3], self.carts[i*3+1]))
            #Write third coordinate on newline to stay under 80 character column limit
            if(len(line)>80):
                f.write("\n{}{}".format(tab2+tab, self.carts[i*3+2]))
            else:
                f.write(" {}".format(self.carts[i*3+2]))
            f.write("}}")
            if i != len(self.atoms) -1:
                f.write(",")
                f.write("\n")
        f.write(");".format(tab))


def parse_file(file_name, sym2Z, ang2au):
    an_atom = r"^\s*(\S{1,2})((?:\s*\d*.\d+)+)"
    mol = Molecule()
    with open(file_name, 'r') as f:
        for l in f:
            is_match = re.match(an_atom, l)
            if is_match:
                (sym, str_carts) = is_match.groups()
                Z = sym2Z[sym.lower()]
                mol.add_atom(Z, [float(x)*ang2au for x in str_carts.split()])                
    return mol

def print_source(out_dir, mols):
    tab = "    "
    with open(os.path.join(out_dir, "NWXMoleculeManagerPIMPL.cpp"), 'w') as f:
        f.write(
"""/*
 * This file was autogenerated by generate_mols.py. Any edits to it will be
 * lost the next time it is generated.
 */

#include \"LibChemist/Implementations/MoleculeManagerPIMPL.hpp\"

namespace LibChemist::detail_ {

class HardCodedMolsPIMPL : public MoleculeManagerPIMPL {
public:
    using MoleculeManagerPIMPL::MoleculeManagerPIMPL;

protected:
    HardCodedMolsPIMPL(const HardCodedMolsPIMPL& rhs) = default;

private:
    unique_pimpl clone_() const override {
        using unique_me = std::unique_ptr<HardCodedMolsPIMPL>;
        return unique_me(new HardCodedMolsPIMPL(*this));
    }

    value_type at_(const key_type& name) const override {
        using cart_t = typename Atom::coord_type;
        using mass_t = typename Atom::mass_type;
        """)
        for mname, m in mols.items():
            f.write("if(name == \"{}\") {{\n{}return ".format(mname, tab*3))
            m.cxxify(tab*3, f)
            f.write("\n{}}} else ".format(tab*2))
        f.write("\nthrow std::out_of_range(\"Unknown molecule name\");\n")
        f.write(
"""    } // end at_
};    // end HardCodedMolsPIMPL

std::unique_ptr<MoleculeManagerPIMPL> nwx_default_mols() {
    return std::make_unique<HardCodedMolsPIMPL>();
}

} // namespace LibChemist::detail_
""")


def main(ang2au):
    my_dir = os.path.dirname(os.path.realpath(__file__))
    mols = list ([(f[:-4]) for f in os.listdir(os.path.join(my_dir, "molecules")) if f.endswith('.xyz')])
    atoms = parse_symbols(os.path.join(my_dir, "physical_data",
                                       "ElementNames.txt"), {})
    sym2Z = {ai.sym.lower() : ai.Z for ai in atoms.values()}
    mol_out = {}
    for m in mols:
        dir = os.path.join(my_dir,"molecules", m + ".xyz")
        mol_out[m] = parse_file(dir, sym2Z, ang2au)
    out_dir = os.path.join(os.path.dirname(my_dir), "LibChemist", "Defaults")
    print_source(out_dir, mol_out)


if __name__ == "__main__" :
    ang2au = 1.8897161646320724
    main(ang2au)


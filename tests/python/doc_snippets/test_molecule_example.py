import chemist
import unittest

class TestStringMethods(unittest.TestCase):
    def testMolecule(self):
        # Begin constructors
        # Default constructor, no atoms, charge = 0.0, multiplicity = 1
        m0 = chemist.Molecule()

        # PyBind11 doesn't support constructors with an initializer list 
        # Therefore, using the push_back method to add atoms to the molecule
        a_H1 = chemist.Atom("H", 1, 1.0079, 0.0, 0.0, 0.0)
        a_H2 = chemist.Atom("H", 1, 1.0079, 1.0, 0.0, 0.0)
        m1 = chemist.Molecule()
        m1.push_back(a_H1)
        m1.push_back(a_H2)

        # Creating Molecule with a specific charge, multiplicity, and nuclei
        n_H1 = chemist.Nucleus("H", 1, 1.0079, 0.0, 0.0, 0.0)
        n_H2 = chemist.Nucleus("H", 1, 1.0079, 1.0, 0.0, 0.0)
        # Constructing a Nuclei object by pushing back Nucleus objects
        nuclei = chemist.Nuclei()
        nuclei.push_back(n_H1)
        nuclei.push_back(n_H2)
        m2 = chemist.Molecule(1, 2, nuclei)
        # End constructors

        # Begin properties
        # Accessing the size (number of atoms)
        m1_size = m1.size()
        assert m1_size == 2
        assert m0.size() == 0

        # Accessing the charge
        m1_charge = m1.charge()
        assert m0.charge() == 0.0
        assert m1.charge() == 0.0
        assert m2.charge() == 1.0

        # Accessing the multiplicity
        assert m0.multiplicity() == 1
        assert m1.multiplicity() == 1

        # Accessing the number of electrons
        assert m0.n_electrons() == 0
        assert m1.n_electrons() == 2
        assert m2.n_electrons() == 1

        # Comparing Molecule objects
        assert m0 != m1
        assert m1 != m2
        assert m2 == m2  # This might be a typo in the original C++ code. m2 is being compared to itself.

        # Modifying the properties of a Molecule object
        m1.set_charge(1)
        m1.set_multiplicity(2)
        assert m1 == m2

        # We can add atoms to a Molecule object        
        a_O1 = chemist.Atom("O", 8, 15.9994, 2.0, 0.0, 0.0)
        m1.push_back(a_O1)
        assert m1.size() == 3
        assert m1.n_electrons() == 9

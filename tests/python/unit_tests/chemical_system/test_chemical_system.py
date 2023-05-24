import chemist
import unittest

class TestChemicalSystem(unittest.TestCase):
    def test_molecule(self):
        self.assertEqual(self.defaulted.molecule,self.default_mol)
        self.assertEqual(self.has_default_mol.molecule, self.default_mol)
        self.assertEqual(self.has_mol.molecule, self.mol)

        # Can write to it
        self.defaulted.molecule = self.mol
        self.assertEqual(self.defaulted.molecule, self.mol)


    def test_comparisons(self):
        # Default vs. default
        other_default = chemist.ChemicalSystem()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs. non-default with default state
        self.assertEqual(self.defaulted, self.has_default_mol)
        self.assertFalse(self.defaulted != self.has_default_mol)

        # Non-default with default state vs. non-default with non-default state
        self.assertNotEqual(self.has_default_mol, self.has_mol)
        self.assertFalse(self.has_default_mol == self.has_mol)


    def setUp(self):
        self.defaulted = chemist.ChemicalSystem()
        self.default_mol = chemist.Molecule()
        self.mol = chemist.Molecule()
        self.mol.push_back(chemist.Atom())
        self.has_default_mol  = chemist.ChemicalSystem(self.default_mol)
        self.has_mol = chemist.ChemicalSystem(self.mol)

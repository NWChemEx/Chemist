from chemist.wavefunction import AOs
from chemist.basis_set import AOBasisSetD
import unittest
from ..test_qm import h2_basis


class TestAOs(unittest.TestCase):

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.defaulted.ao_basis_set, AOBasisSetD())

    def test_value_ctor(self):
        self.assertEqual(self.value.size(), 2)
        self.assertEqual(self.value.ao_basis_set, self.basis)

    def test_ao_basis_set(self):
        self.assertEqual(self.defaulted.ao_basis_set, AOBasisSetD())
        self.assertEqual(self.value.ao_basis_set, self.basis)

        # Can write to it
        self.defaulted.ao_basis_set = self.basis
        self.assertEqual(self.defaulted.ao_basis_set, self.basis)

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.value.size(), 2)

    def test_are_equal(self):
        self.assertTrue(self.defaulted.are_equal(AOs()))
        self.assertFalse(self.value.are_equal(AOs()))

    def test_comparisons(self):
        self.assertEqual(self.defaulted, AOs())  # Default == default
        self.assertNotEqual(self.defaulted, self.value)  # Default != value
        self.assertEqual(self.value, AOs(self.basis))  # value == same value

    def setUp(self):
        self.defaulted = AOs()
        self.basis = h2_basis()
        self.value = AOs(self.basis)

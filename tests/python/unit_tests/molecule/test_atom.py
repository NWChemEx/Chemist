import chemist
import unittest

class TestAtom(unittest.TestCase):
    def test_name(self):
        # Check initial values
        self.assertEqual(self.defaulted.name, '')
        self.assertEqual(self.h.name, 'H')
        self.assertEqual(self.he.name, 'He')

        # Can write to it
        self.defaulted.name = 'foo'
        self.assertEqual(self.defaulted.name, 'foo')


    def test_nucleus(self):
        corr_h_nuke = chemist.Nucleus('H', 1, 1.0, 2.0, 3.0, 4.0)
        corr_he_nuke = chemist.Nucleus('He', 2, 4.0, 3.0, 4.0, 5.0, 1.0)

        # Check initial values
        self.assertEqual(self.defaulted.nucleus, chemist.Nucleus())
        self.assertEqual(self.h.nucleus, corr_h_nuke)
        self.assertEqual(self.he.nucleus, corr_he_nuke)

        # Can write to it
        self.he.nucleus = corr_h_nuke
        self.assertEqual(self.he.nucleus, corr_h_nuke)


    def test_Z(self):
        # Check initial values
        self.assertEqual(self.defaulted.Z, 0)
        self.assertEqual(self.h.Z, 1)
        self.assertEqual(self.he.Z, 2)

        # Can write to it
        self.defaulted.Z = 1
        self.assertEqual(self.defaulted.Z, 1)


    def test_mass(self):
        # Check initial values
        self.assertEqual(self.defaulted.mass, 0.0)
        self.assertEqual(self.h.mass, 1.0)
        self.assertEqual(self.he.mass, 4.0)

        # Can write to it
        self.defaulted.mass = 42.0
        self.assertEqual(self.defaulted.mass, 42.0)


    def test_charge(self):
        # Check initial values
        self.assertEqual(self.defaulted.charge, 0.0)
        self.assertEqual(self.h.charge, 1.0)
        self.assertEqual(self.he.charge, 1.0)

        # Can write to it
        self.defaulted.charge = -42.0
        self.assertEqual(self.defaulted.charge, -42.0)


    def test_coord(self):
        self.assertEqual(self.defaulted.coord(0), 0.0)
        self.assertEqual(self.defaulted.coord(1), 0.0)
        self.assertEqual(self.defaulted.coord(2), 0.0)

        self.assertEqual(self.h.coord(0), 2.0)
        self.assertEqual(self.h.coord(1), 3.0)
        self.assertEqual(self.h.coord(2), 4.0)

        self.assertEqual(self.he.coord(0), 3.0)
        self.assertEqual(self.he.coord(1), 4.0)
        self.assertEqual(self.he.coord(2), 5.0)


    def test_x(self):
        # Check initial values
        self.assertEqual(self.defaulted.x, 0.0)
        self.assertEqual(self.h.x, 2.0)
        self.assertEqual(self.he.x, 3.0)

        # Can write to it
        self.defaulted.x = -1.23
        self.assertEqual(self.defaulted.x, -1.23)


    def test_y(self):
        # Check initial values
        self.assertEqual(self.defaulted.y, 0.0)
        self.assertEqual(self.h.y, 3.0)
        self.assertEqual(self.he.y, 4.0)

        # Can write to it
        self.defaulted.y = -1.23
        self.assertEqual(self.defaulted.y, -1.23)


    def test_z(self):
        # Check initial values
        self.assertEqual(self.defaulted.z, 0.0)
        self.assertEqual(self.h.z, 4.0)
        self.assertEqual(self.he.z, 5.0)

        # Can write to it
        self.defaulted.z = -1.23
        self.assertEqual(self.defaulted.z, -1.23)


    def test_comparisons(self):
        # Default vs. default
        self.assertEqual(self.defaulted, chemist.Atom())
        self.assertFalse(self.defaulted != chemist.Atom())

        # Default vs. non-default
        self.assertNotEqual(self.defaulted, self.h)
        self.assertFalse(self.defaulted == self.h)


        # Same non-default
        other_h = chemist.Atom('H', 1, 1.0, 2.0, 3.0, 4.0, 1.0)
        self.assertEqual(self.h, other_h)
        self.assertFalse(self.h != other_h)

        # Different symbol
        other_h = chemist.Atom('He', 1, 1.0, 2.0, 3.0, 4.0, 1.0)
        self.assertNotEqual(self.h, other_h)
        self.assertFalse(self.h == other_h)

        # Different atomic number
        other_h = chemist.Atom('H', 2, 1.0, 2.0, 3.0, 4.0, 1.0)
        self.assertNotEqual(self.h, other_h)
        self.assertFalse(self.h == other_h)

        # Different mass
        other_h = chemist.Atom('H', 1, 2.0, 2.0, 3.0, 4.0, 1.0)
        self.assertNotEqual(self.h, other_h)
        self.assertFalse(self.h == other_h)

        # Different x-coordinate
        other_h = chemist.Atom('H', 1, 1.0, 3.0, 3.0, 4.0, 1.0)
        self.assertNotEqual(self.h, other_h)
        self.assertFalse(self.h == other_h)

        # Different y-coordinate
        other_h = chemist.Atom('H', 1, 1.0, 2.0, 4.0, 4.0, 1.0)
        self.assertNotEqual(self.h, other_h)
        self.assertFalse(self.h == other_h)

        # Different z-coordinate
        other_h = chemist.Atom('H', 1, 1.0, 2.0, 3.0, 5.0, 1.0)
        self.assertNotEqual(self.h, other_h)
        self.assertFalse(self.h == other_h)

        # Different nuclear charge
        other_h = chemist.Atom('H', 1, 1.0, 2.0, 3.0, 4.0, 2.0)
        self.assertNotEqual(self.h, other_h)
        self.assertFalse(self.h == other_h)


    def setUp(self):
        self.defaulted = chemist.Atom()
        self.h = chemist.Atom('H', 1, 1.0, 2.0, 3.0, 4.0)
        self.he = chemist.Atom('He', 2, 4.0, 3.0, 4.0, 5.0, 1.0)

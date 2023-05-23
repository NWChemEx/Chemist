import chemist
import unittest

class TestNucleus(unittest.TestCase):
    def test_base(self):
        """
        This test makes sure we can access the base class's member
        functions/properties. From unit tests of the PointChargeD
        classes we know the functions work so we're mainly ensuring they
        are accessible.
        """

        # Test the default instance

        # -- PointCharge Base ----------------------
        self.assertEqual(self.defaulted.charge, 0.0)

        # -- Point Base ----------------------------
        self.assertEqual(self.defaulted.x, 0.0)
        self.assertEqual(self.defaulted.y, 0.0)
        self.assertEqual(self.defaulted.z, 0.0)

        for i in range(3):
            self.assertEqual(self.defaulted.coord(i), 0.0)

        self.assertEqual(self.defaulted.magnitude(), 0.0)

        # Difference of two points a the origin
        origin = chemist.PointD()
        self.assertEqual(origin - self.defaulted, origin)

        # Repeat, this time with h
        self.assertEqual(self.h.charge, 1.0)
        self.assertEqual(self.h.x, 0.0)
        self.assertEqual(self.h.y, 0.0)
        self.assertEqual(self.h.z, 0.0)
        self.assertEqual(self.h.coord(0), 0.0)
        self.assertEqual(self.h.coord(1), 0.0)
        self.assertEqual(self.h.coord(2), 0.0)
        self.assertEqual(self.h.magnitude(), 0.0)
        self.assertEqual(origin - self.h, origin)

        # Now he
        self.assertEqual(self.he.charge, 2.0)
        self.assertEqual(self.he.x, 1.0)
        self.assertEqual(self.he.y, 2.0)
        self.assertEqual(self.he.z, 3.0)
        self.assertEqual(self.he.coord(0), 1.0)
        self.assertEqual(self.he.coord(1), 2.0)
        self.assertEqual(self.he.coord(2), 3.0)
        self.assertAlmostEqual(self.he.magnitude(), 14.0**0.5)
        self.assertEqual(origin - self.he, chemist.PointD(-1.0, -2.0, -3.0))

        # Finally u
        self.assertEqual(self.u.charge, 91.0)
        self.assertEqual(self.u.x, 2.0)
        self.assertEqual(self.u.y, 3.0)
        self.assertEqual(self.u.z, 4.0)
        self.assertEqual(self.u.coord(0), 2.0)
        self.assertEqual(self.u.coord(1), 3.0)
        self.assertEqual(self.u.coord(2), 4.0)
        self.assertAlmostEqual(self.u.magnitude(), 29.0**0.5)
        self.assertEqual(origin - self.u, chemist.PointD(-2.0, -3.0, -4.0))


    def test_name(self):
        # Test original values
        self.assertEqual(self.defaulted.name, '')
        self.assertEqual(self.h.name, 'H')
        self.assertEqual(self.he.name, 'He')
        self.assertEqual(self.u.name, 'U')

        # Can change it
        self.defaulted.name = 'Ez'
        self.assertEqual(self.defaulted.name, 'Ez')


    def test_Z(self):
        # Test original values
        self.assertEqual(self.defaulted.Z, 0)
        self.assertEqual(self.h.Z, 1)
        self.assertEqual(self.he.Z, 2)
        self.assertEqual(self.u.Z, 92)

        # Can change it
        self.defaulted.Z = 6
        self.assertEqual(self.defaulted.Z, 6)


    def test_mass(self):
        # Test original values
        self.assertEqual(self.defaulted.mass, 0.0)
        self.assertEqual(self.h.mass, 1.0)
        self.assertEqual(self.he.mass, 4.0)
        self.assertEqual(self.u.mass, 238.0)

        # Can change it
        self.defaulted.mass = 1.234
        self.assertEqual(self.defaulted.mass, 1.234)


    def test_comparisons(self):
        # Default vs. default
        other_default = chemist.Nucleus()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs non-default
        self.assertNotEqual(self.defaulted, self.h)
        self.assertFalse(self.defaulted == self.h)

        # Different name
        h2 = chemist.Nucleus('D', 1, 1.0)
        self.assertNotEqual(self.h, h2)
        self.assertFalse(self.h == h2)

        # Different atomic number
        h2 = chemist.Nucleus('H', 2, 1.0)
        self.assertNotEqual(self.h, h2)
        self.assertFalse(self.h == h2)

        # Different mass
        h2 = chemist.Nucleus('H', 1, 2.0)
        self.assertNotEqual(self.h, h2)
        self.assertFalse(self.h == h2)

        # Different charge
        u2 = chemist.Nucleus("U", 92, 238.0, 2.0, 3.0, 4.0, 92.0)
        self.assertNotEqual(self.u, u2)
        self.assertFalse(self.u == u2)

        # Different x-coordinate
        u2 = chemist.Nucleus("U", 92, 238.0, 3.0, 3.0, 4.0, 91.0)
        self.assertNotEqual(self.u, u2)
        self.assertFalse(self.u == u2)

        # Different y-coordinate
        u2 = chemist.Nucleus("U", 92, 238.0, 2.0, 4.0, 4.0, 91.0)
        self.assertNotEqual(self.u, u2)
        self.assertFalse(self.u == u2)

        # Different z-coordinate
        u2 = chemist.Nucleus("U", 92, 238.0, 2.0, 3.0, 5.0, 91.0)
        self.assertNotEqual(self.u, u2)
        self.assertFalse(self.u == u2)

        # Compare to a point_charge with same state
        self.assertEqual(self.defaulted, chemist.PointChargeD())
        self.assertEqual(chemist.PointChargeD(), self.defaulted)

        # Compare to a point_type with different state
        self.assertNotEqual(self.he, chemist.PointChargeD())
        self.assertNotEqual(chemist.PointChargeD(), self.he)


    def setUp(self):
        self.defaulted = chemist.Nucleus()
        self.h = chemist.Nucleus("H", 1, 1.0)
        self.he = chemist.Nucleus("He", 2, 4.0, 1.0, 2.0, 3.0)
        self.u = chemist.Nucleus("U", 92, 238.0, 2.0, 3.0, 4.0, 91.0)

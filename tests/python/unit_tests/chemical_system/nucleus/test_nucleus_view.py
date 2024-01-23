# Copyright 2023 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import chemist
import unittest

class TestNucleus(unittest.TestCase):
    def test_base(self):
        """
        This test makes sure we can access the base class's member
        functions/properties. From unit tests of the PointChargeViewD
        classes we know the functions work so we're mainly ensuring they
        are accessible.
        """

        # Test the default instance

        # -- PointCharge Base ----------------------
        self.assertEqual(self.defaulted_view.charge, 0.0)

        # -- Point Base ----------------------------
        self.assertEqual(self.defaulted_view.x, 0.0)
        self.assertEqual(self.defaulted_view.y, 0.0)
        self.assertEqual(self.defaulted_view.z, 0.0)

        for i in range(3):
            self.assertEqual(self.defaulted_view.coord(i), 0.0)

        self.assertEqual(self.defaulted_view.magnitude(), 0.0)

        # Repeat, this time with u_view
        self.assertEqual(self.u_view.charge, 91.0)
        self.assertEqual(self.u_view.x, 2.0)
        self.assertEqual(self.u_view.y, 3.0)
        self.assertEqual(self.u_view.z, 4.0)
        self.assertEqual(self.u_view.coord(0), 2.0)
        self.assertEqual(self.u_view.coord(1), 3.0)
        self.assertEqual(self.u_view.coord(2), 4.0)
        self.assertAlmostEqual(self.u_view.magnitude(), 29.0**0.5)


    def test_name(self):
        # Test original values
        self.assertEqual(self.defaulted_view.name, '')
        self.assertEqual(self.u_view.name, 'U')

        # Can change it
        self.defaulted_view.name = 'Ez'
        self.assertEqual(self.defaulted_view.name, 'Ez')

        # Changes object it's a view of
        self.assertEqual(self.defaulted.name, 'Ez')


    def test_Z(self):
        # Test original values
        self.assertEqual(self.defaulted_view.Z, 0)
        self.assertEqual(self.u_view.Z, 92)

        # Can change it
        self.defaulted_view.Z = 6
        self.assertEqual(self.defaulted_view.Z, 6)

        # Changes object it's a view of
        self.assertEqual(self.defaulted.Z, 6)


    def test_mass(self):
        # Test original values
        self.assertEqual(self.defaulted_view.mass, 0.0)
        self.assertEqual(self.u_view.mass, 238.0)

        # Can change it
        self.defaulted_view.mass = 1.234
        self.assertEqual(self.defaulted_view.mass, 1.234)

        # Changes object it's a view of
        self.assertEqual(self.defaulted.mass, 1.234)


    def test_comparisons(self):
        # Default view vs. default view
        other_default = chemist.Nucleus()
        other_defaulted_view = chemist.NucleusView(other_default)
        self.assertEqual(self.defaulted_view, other_defaulted_view)
        self.assertFalse(self.defaulted_view != other_defaulted_view)

        # Defaulted view vs defaulted
        self.assertEqual(self.defaulted_view, self.defaulted)
        self.assertFalse(self.defaulted_view != self.defaulted)

        # Default view vs non-default view
        self.assertNotEqual(self.defaulted_view, self.u_view)
        self.assertFalse(self.defaulted_view == self.u_view)

        # Default view vw non-default
        self.assertNotEqual(self.defaulted_view, self.u)
        self.assertFalse(self.defaulted_view == self.u)

        # Different name
        u2 = chemist.Nucleus("H", 92, 238.0, 2.0, 3.0, 4.0, 91.0)
        u2_view = chemist.NucleusView(u2)
        self.assertNotEqual(self.u_view, u2_view)
        self.assertFalse(self.u_view == u2_view)
        self.assertNotEqual(self.u_view, u2)
        self.assertFalse(self.u_view == u2)
        self.assertNotEqual(u2, self.u_view)
        self.assertFalse(u2 == self.u_view)

        # Different atomic number
        u2 = chemist.Nucleus("U", 91, 238.0, 2.0, 3.0, 4.0, 91.0)
        u2_view = chemist.NucleusView(u2)
        self.assertNotEqual(self.u_view, u2_view)
        self.assertFalse(self.u_view == u2_view)
        self.assertNotEqual(self.u_view, u2)
        self.assertFalse(self.u_view == u2)
        self.assertNotEqual(u2, self.u_view)
        self.assertFalse(u2 == self.u_view)

        # Different mass
        u2 = chemist.Nucleus("U", 92, 231.0, 2.0, 3.0, 4.0, 91.0)
        u2_view = chemist.NucleusView(u2)
        self.assertNotEqual(self.u_view, u2_view)
        self.assertFalse(self.u_view == u2_view)
        self.assertNotEqual(self.u_view, u2)
        self.assertFalse(self.u_view == u2)
        self.assertNotEqual(u2, self.u_view)
        self.assertFalse(u2 == self.u_view)

        # Different charge
        u2 = chemist.Nucleus("U", 92, 238.0, 2.0, 3.0, 4.0, 92.0)
        u2_view = chemist.NucleusView(u2)
        self.assertNotEqual(self.u_view, u2_view)
        self.assertFalse(self.u_view == u2_view)
        self.assertNotEqual(self.u_view, u2)
        self.assertFalse(self.u_view == u2)
        self.assertNotEqual(u2, self.u_view)
        self.assertFalse(u2 == self.u_view)

        # Different x-coordinate
        u2 = chemist.Nucleus("U", 92, 238.0, 3.0, 3.0, 4.0, 91.0)
        u2_view = chemist.NucleusView(u2)
        self.assertNotEqual(self.u_view, u2_view)
        self.assertFalse(self.u_view == u2_view)
        self.assertNotEqual(self.u_view, u2)
        self.assertFalse(self.u_view == u2)
        self.assertNotEqual(u2, self.u_view)
        self.assertFalse(u2 == self.u_view)

        # Different y-coordinate
        u2 = chemist.Nucleus("U", 92, 238.0, 2.0, 2.0, 4.0, 91.0)
        u2_view = chemist.NucleusView(u2)
        self.assertNotEqual(self.u_view, u2_view)
        self.assertFalse(self.u_view == u2_view)
        self.assertNotEqual(self.u_view, u2)
        self.assertFalse(self.u_view == u2)
        self.assertNotEqual(u2, self.u_view)
        self.assertFalse(u2 == self.u_view)

        # Different z-coordinate
        u2 = chemist.Nucleus("U", 92, 238.0, 2.0, 3.0, 3.0, 91.0)
        u2_view = chemist.NucleusView(u2)
        self.assertNotEqual(self.u_view, u2_view)
        self.assertFalse(self.u_view == u2_view)
        self.assertNotEqual(self.u_view, u2)
        self.assertFalse(self.u_view == u2)
        self.assertNotEqual(u2, self.u_view)
        self.assertFalse(u2 == self.u_view)

        # Compare to a point_charge view with same state
        q0 = chemist.PointChargeD()
        q0_view = chemist.PointChargeViewD(q0)
        self.assertEqual(self.defaulted_view, q0_view)
        self.assertEqual(q0_view, self.defaulted_view)

        # Compare to a point_type with different state
        self.assertNotEqual(self.u_view, q0_view)
        self.assertNotEqual(q0_view, self.u_view)


    def test_str(self):
        # Default
        self.assertEqual(str(self.defaulted_view), ' 0.000000000000000 0.000000000000000 0.000000000000000')

        # Has value
        self.assertEqual(str(self.u_view), 'U 2.000000000000000 3.000000000000000 4.000000000000000')

        
    def setUp(self):
        self.defaulted = chemist.Nucleus()
        self.defaulted_view = chemist.NucleusView(self.defaulted)
        self.u = chemist.Nucleus("U", 92, 238.0, 2.0, 3.0, 4.0, 91.0)
        self.u_view = chemist.NucleusView(self.u)

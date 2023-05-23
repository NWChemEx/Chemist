import chemist
import unittest

def make_test_charges(charges_type):
    class TestCharges(unittest.TestCase):
        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.has_value.empty())


        def test_push_back(self):
            # Sanity check
            self.assertEqual(self.defaulted.size(), 0)

            # Add q0
            self.defaulted.push_back(self.q0)
            self.assertEqual(self.defaulted.size(), 1)
            self.assertEqual(self.defaulted.at(0), self.q0)

            # Add q1
            self.defaulted.push_back(self.q1)
            self.assertEqual(self.defaulted.size(), 2)
            self.assertEqual(self.defaulted.at(0), self.q0)
            self.assertEqual(self.defaulted.at(1), self.q1)
            self.assertEqual(self.defaulted, self.has_value)


        def test_at(self):
            # Check values
            q0 = self.has_value.at(0)
            q1 = self.has_value.at(1)
            self.assertEqual(q0, self.q0)
            self.assertEqual(q1, self.q1)

            # Are views
            q0.x = 42.0
            self.assertEqual(self.has_value.at(0).x, 42.0)


        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.has_value.size(), 2)


        def test_comparisons(self):
            # Default vs default
            other_default = charges_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Different number of points
            other_default.push_back(self.q0)
            self.assertNotEqual(self.has_value, other_default)
            self.assertFalse(self.has_value == other_default)

            # Same (and has points)
            other_default.push_back(self.q1)
            self.assertEqual(self.has_value, other_default)
            self.assertFalse(self.has_value != other_default)

            # Different order
            diff_order = charges_type()
            diff_order.push_back(self.q1)
            diff_order.push_back(self.q0)
            self.assertNotEqual(self.has_value, diff_order)
            self.assertFalse(self.has_value == diff_order)


        def setUp(self):
            if charges_type == chemist.ChargesF:
                self.charge_type = chemist.PointChargeF
            else:
                self.charge_type = chemist.PointChargeD

            self.defaulted = charges_type()
            self.q0 = self.charge_type()
            self.q1 = self.charge_type(1.0, 2.0, 3.0, 4.0)
            self.has_value = charges_type()
            self.has_value.push_back(self.q0)
            self.has_value.push_back(self.q1)


    return TestCharges

class TestChargesF(make_test_charges(chemist.ChargesF)):
    pass

class TestChargesD(make_test_charges(chemist.ChargesD)):
    pass

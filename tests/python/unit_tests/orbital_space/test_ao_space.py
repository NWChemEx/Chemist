import unittest
import chemist

class TestAOSpace(unittest.TestCase):
    def test_aospace(self):
        # Test initialization
        ao_space = chemist.AOSpaceD()
        self.assertIsNotNone(ao_space)

        # Test size
        size = ao_space.size()
        self.assertEqual(size, 0)

        # Test equality
        ao_space2 = chemist.AOSpaceD()
        self.assertEqual(ao_space, ao_space2)

        # Test inequality
        ao_space3 = chemist.AOSpaceF()
        self.assertNotEqual(ao_space, ao_space3)

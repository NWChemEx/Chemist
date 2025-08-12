# Copyright 2024 NWChemEx Community
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

import unittest

from chemist import Nuclei, Nucleus
from chemist.fragmenting import Cap, CapSet, FragmentedNuclei


class TestFragmentedNuclei(unittest.TestCase):
    def test_ctors(self):
        self.assertEqual(self.defaulted.supersystem(), self.empty_ss)
        self.assertEqual(self.defaulted.size(), 0)

        self.assertEqual(self.no_frags.supersystem(), self.ss)
        self.assertEqual(self.no_frags.size(), 0)

        self.assertEqual(self.disjoint.supersystem(), self.ss)
        self.assertEqual(self.disjoint.size(), 3)
        self.assertEqual(self.disjoint.at(0), self.frag0)
        self.assertEqual(self.disjoint.at(1), self.frag1)
        self.assertEqual(self.disjoint.at(2), self.frag2)

        self.assertEqual(self.nondisjoint.supersystem(), self.ss)
        self.assertEqual(self.nondisjoint.size(), 2)
        self.assertEqual(self.nondisjoint.at(0), self.frag01)
        self.assertEqual(self.nondisjoint.at(1), self.frag12)

        self.assertEqual(self.disjoint_caps.supersystem(), self.ss)
        self.assertEqual(self.disjoint_caps.size(), 3)
        self.assertEqual(self.disjoint_caps.at(0), self.capped_frag0)
        self.assertEqual(self.disjoint_caps.at(1), self.capped_frag1)
        self.assertEqual(self.disjoint_caps.at(2), self.frag2)

        self.assertEqual(self.nondisjoint_caps.supersystem(), self.ss)
        self.assertEqual(self.nondisjoint_caps.size(), 2)
        self.assertEqual(self.nondisjoint_caps.at(0), self.frag01)
        self.assertEqual(self.nondisjoint_caps.at(1), self.capped_frag12)

    def test_supersystem(self):
        self.assertEqual(self.defaulted.supersystem(), self.empty_ss)
        self.assertEqual(self.no_frags.supersystem(), self.ss)
        self.assertEqual(self.disjoint.supersystem(), self.ss)
        self.assertEqual(self.nondisjoint.supersystem(), self.ss)
        self.assertEqual(self.disjoint_caps.supersystem(), self.ss)
        self.assertEqual(self.nondisjoint_caps.supersystem(), self.ss)

    def test_insert(self):
        # No supersystem, so can't insert
        self.assertRaises(RuntimeError, self.defaulted.insert, [0])

        # Fragment index is out of range
        self.assertRaises(RuntimeError, self.disjoint.insert, [9])

        # Can insert by index
        self.no_frags.insert([0])
        self.assertEqual(self.no_frags.size(), 1)
        self.assertEqual(self.no_frags.at(0), self.frag0)

        # Can insert by value
        self.no_frags.insert([self.ss.at(1), self.ss.at(2)])
        self.assertEqual(self.no_frags.size(), 2)
        self.assertEqual(self.no_frags.at(1), self.frag12)

    def test_nuclear_indices(self):
        self.assertRaises(IndexError, self.no_frags.nuclear_indices, 0)

        self.assertEqual(self.disjoint.nuclear_indices(0), [0])
        self.assertEqual(self.disjoint.nuclear_indices(1), [1])
        self.assertEqual(self.disjoint.nuclear_indices(2), [2])

        self.assertEqual(self.nondisjoint_caps.nuclear_indices(0), [0, 1])
        self.assertEqual(self.nondisjoint_caps.nuclear_indices(1), [1, 2])

    def test_cap_set(self):
        self.assertEqual(self.defaulted.cap_set(), self.empty_caps)
        self.assertEqual(self.no_frags.cap_set(), self.empty_caps)
        self.assertEqual(self.disjoint.cap_set(), self.empty_caps)
        self.assertEqual(self.nondisjoint.cap_set(), self.empty_caps)
        self.assertEqual(self.disjoint_caps.cap_set(), self.caps)
        self.assertEqual(self.nondisjoint_caps.cap_set(), self.caps)

    def test_add_cap(self):
        self.no_frags.add_cap(self.c01)
        self.no_frags.add_cap(self.c10)
        self.assertEqual(self.no_frags.cap_set(), self.caps)

    def test_at(self):
        self.assertEqual(self.disjoint.at(0), self.frag0)
        self.assertEqual(self.disjoint.at(1), self.frag1)
        self.assertEqual(self.disjoint.at(2), self.frag2)

        self.assertEqual(self.nondisjoint.at(0), self.frag01)
        self.assertEqual(self.nondisjoint.at(1), self.frag12)

        self.assertEqual(self.disjoint_caps.at(0), self.capped_frag0)
        self.assertEqual(self.disjoint_caps.at(1), self.capped_frag1)
        self.assertEqual(self.disjoint_caps.at(2), self.frag2)

        self.assertEqual(self.nondisjoint_caps.at(0), self.frag01)
        self.assertEqual(self.nondisjoint_caps.at(1), self.capped_frag12)

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.no_frags.size(), 0)
        self.assertEqual(self.disjoint.size(), 3)
        self.assertEqual(self.nondisjoint.size(), 2)
        self.assertEqual(self.disjoint_caps.size(), 3)
        self.assertEqual(self.nondisjoint_caps.size(), 2)

    def test_comparisons(self):
        # Default vs default
        rhs = FragmentedNuclei()
        self.assertEqual(self.defaulted, rhs)

        # Default vs empty superset
        rhs = FragmentedNuclei(self.empty_ss)
        self.assertEqual(self.defaulted, rhs)

        # Default vs empty
        self.assertNotEqual(self.defaulted, self.no_frags)

        # Default vs value
        self.assertNotEqual(self.defaulted, self.disjoint)

        # Empty vs empty
        rhs = FragmentedNuclei(self.ss)
        self.assertEqual(self.no_frags, rhs)

        # Empty vs value
        self.assertNotEqual(self.no_frags, self.disjoint)

        # Value vs value
        rhs = FragmentedNuclei(self.ss, [[0], [1], [2]])
        self.assertEqual(self.disjoint, rhs)

        # Different fragments
        self.assertNotEqual(self.disjoint, self.nondisjoint)

        # Different caps
        self.assertNotEqual(self.disjoint, self.disjoint_caps)

    def setUp(self):
        self.h0 = Nucleus("H", 1, 1.0, 2.0, 3.0, 4.0)
        self.h1 = Nucleus("H", 1, 1.0, 5.0, 6.0, 7.0)
        self.h2 = Nucleus("H", 1, 1.0, 8.0, 9.0, 0.0)
        self.ss = Nuclei([self.h0, self.h1, self.h2])
        self.empty_ss = Nuclei()

        self.c01 = Cap(0, 1, self.h1)
        self.c10 = Cap(1, 0, self.h0)
        self.caps = CapSet(self.c01, self.c10)
        self.empty_caps = CapSet()

        # Corr fragments
        self.frag0 = Nuclei([self.h0])
        self.capped_frag0 = Nuclei([self.h0, self.h1])
        self.frag01 = Nuclei([self.h0, self.h1])
        self.frag1 = Nuclei([self.h1])
        self.capped_frag1 = Nuclei([self.h1, self.h0])
        self.frag12 = Nuclei([self.h1, self.h2])
        self.capped_frag12 = Nuclei([self.h1, self.h2, self.h0])
        self.frag2 = Nuclei([self.h2])

        l012 = [[0], [1], [2]]
        l01_12 = [[0, 1], [1, 2]]
        self.defaulted = FragmentedNuclei()
        self.no_frags = FragmentedNuclei(self.ss)
        self.disjoint = FragmentedNuclei(self.ss, l012)
        self.nondisjoint = FragmentedNuclei(self.ss, l01_12)
        self.disjoint_caps = FragmentedNuclei(self.ss, l012, self.caps)
        self.nondisjoint_caps = FragmentedNuclei(self.ss, l01_12, self.caps)

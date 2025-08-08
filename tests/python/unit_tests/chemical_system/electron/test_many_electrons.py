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

import unittest

import chemist


class TestManyElectrons(unittest.TestCase):
    def test_default_ctor(self):
        self.assertEqual(self.defaulted.size(), 0)

    def test_value_ctor(self):
        self.assertEqual(self.value.size(), 2)
        self.assertEqual(self.value.at(0), chemist.Electron())
        self.assertEqual(self.value.at(1), chemist.Electron())

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.value.size(), 2)

    def test_at(self):
        self.assertEqual(self.value.at(0), chemist.Electron())
        self.assertEqual(self.value.at(1), chemist.Electron())

    def test_value_equal(self):
        self.assertEqual(self.defaulted, chemist.ManyElectrons())
        self.assertEqual(self.value, chemist.ManyElectrons(2))

    def test_different(self):
        self.assertNotEqual(self.defaulted, self.value)
        self.assertNotEqual(self.value, chemist.ManyElectrons(3))

    def setUp(self):
        self.defaulted = chemist.ManyElectrons()
        self.value = chemist.ManyElectrons(2)

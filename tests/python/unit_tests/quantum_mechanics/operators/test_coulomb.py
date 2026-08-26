# Copyright 2026 NWChemEx-Project
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

import numpy as np
from chemist import (
    DecomposableDensity,
    Density,
    Electron,
    ManyElectrons,
    Nuclei,
    Nucleus,
)
from chemist.basis_set import AOBasisSetD
from chemist.qm_operator import Coulomb, Kinetic, OperatorBase
from chemist.wavefunction import AOs
from tensorwrapper import Tensor


class TestCoulombDispatch(unittest.TestCase):
    """Tests the single Coulomb class Python sees. See TestKineticDispatch."""

    def setUp(self):
        self.tensor = Tensor(np.array([[1.0, 2.0], [3.0, 4.0]]))
        self.aos = AOs(AOBasisSetD())
        self.density = Density(self.tensor, self.aos)
        self.decomp_density = DecomposableDensity(
            self.tensor, self.aos, self.tensor
        )

    def test_dispatches_on_particles(self):
        e = Electron()
        many = ManyElectrons(4)
        nuclei = Nuclei()
        cases = [
            (e, e, "_CoulombElectronElectron"),
            (many, many, "_CoulombManyElectronsManyElectrons"),
            (e, self.density, "_CoulombElectronDensityElectron"),
            (many, self.density, "_CoulombManyElectronsDensityElectron"),
            (e, nuclei, "_CoulombElectronNuclei"),
            (many, nuclei, "_CoulombManyElectronsNuclei"),
            (nuclei, nuclei, "_CoulombNucleiNuclei"),
        ]
        for lhs, rhs, name in cases:
            with self.subTest(name=name):
                self.assertEqual(type(Coulomb(lhs, rhs)).__name__, name)

    def test_decomposable_density_beats_density(self):
        # N.B. DecomposableDensity derives from Density, so a dispatcher that
        #      checked Density first would silently build the plain-density
        #      instantiation here.
        self.assertEqual(
            type(Coulomb(Electron(), self.decomp_density)).__name__,
            "_CoulombElectronDecomposableDensityElectron",
        )
        self.assertEqual(
            type(Coulomb(ManyElectrons(4), self.decomp_density)).__name__,
            "_CoulombManyElectronsDecomposableDensityElectron",
        )

    def test_default_is_electron_electron(self):
        self.assertEqual(type(Coulomb()).__name__, "_CoulombElectronElectron")
        self.assertEqual(Coulomb(), Coulomb(Electron(), Electron()))

    def test_isinstance(self):
        v_ee = Coulomb(Electron(), Electron())
        self.assertIsInstance(v_ee, Coulomb)
        self.assertIsInstance(v_ee, OperatorBase)
        self.assertIsInstance(Coulomb(Nuclei(), Nuclei()), Coulomb)

        self.assertNotIsInstance(Kinetic(), Coulomb)
        self.assertNotIsInstance(Electron(), Coulomb)

    def test_unsupported_combination_throws(self):
        # Nuclei only interact with Nuclei.
        with self.assertRaisesRegex(TypeError, "a Nuclei with a Electron"):
            Coulomb(Nuclei(), Electron())

        # A Nucleus is not one of the supported particles.
        nucleus = Nucleus("H", 1, 1.0, 2.0, 3.0, 4.0, 5.0)
        with self.assertRaisesRegex(TypeError, "a Nucleus with a Nucleus"):
            Coulomb(nucleus, nucleus)

    def test_wrong_number_of_particles_throws(self):
        with self.assertRaisesRegex(TypeError, "takes two particles"):
            Coulomb(Electron())


class TestCoulomb(unittest.TestCase):
    def setUp(self):
        # Input Values
        self.nucleus = Nucleus("H", 1, 1.0, 2.0, 3.0, 4.0, 5.0)
        self.nuclei = Nuclei()
        self.nuclei.push_back(self.nucleus)
        self.many_elec = ManyElectrons(4)
        self.tensor = Tensor(np.array([[1.0, 2.0], [3.0, 4.0]]))
        self.basis = AOBasisSetD()
        self.aos = AOs(self.basis)
        self.density = Density(self.tensor, self.aos)
        self.decomp_density = DecomposableDensity(
            self.tensor, self.aos, self.tensor
        )

        # Test objects
        self.default1 = Coulomb(Electron(), Density())
        self.default2 = Coulomb(ManyElectrons(), DecomposableDensity())
        self.default3 = Coulomb(Nuclei(), Nuclei())
        self.has_value1 = Coulomb(Electron(), self.density)
        self.has_value2 = Coulomb(self.many_elec, self.decomp_density)
        self.has_value3 = Coulomb(self.nuclei, self.nuclei)

    def test_particles(self):
        self.assertEqual(self.default1.lhs_particle, Electron())
        self.assertEqual(self.default1.rhs_particle, Density())
        self.assertEqual(self.default2.lhs_particle, ManyElectrons())
        self.assertEqual(self.default2.rhs_particle, DecomposableDensity())
        self.assertEqual(self.default3.lhs_particle, Nuclei())
        self.assertEqual(self.default3.rhs_particle, Nuclei())

        # Check each particle type for writing
        self.default1.rhs_particle = self.density
        self.assertEqual(self.default1.rhs_particle, self.density)
        self.default2.lhs_particle = self.many_elec
        self.assertEqual(self.default2.lhs_particle, self.many_elec)
        self.default2.rhs_particle = self.decomp_density
        self.assertEqual(self.default2.rhs_particle, self.decomp_density)
        self.default3.lhs_particle = self.nuclei
        self.assertEqual(self.default3.lhs_particle, self.nuclei)

        # # Should not be able to set incompatible particle types
        with self.assertRaises(TypeError):
            self.default1.lhs_particle = self.many_elec
        with self.assertRaises(TypeError):
            self.default1.rhs_particle = self.many_elec

    def test_comparison(self):
        self.assertTrue(self.default1 == Coulomb(Electron(), Density()))
        self.assertTrue(
            self.default2 == Coulomb(ManyElectrons(), DecomposableDensity())
        )
        self.assertTrue(self.default3 == Coulomb(Nuclei(), Nuclei()))

        self.assertTrue(self.has_value1 == Coulomb(Electron(), self.density))
        self.assertTrue(
            self.has_value2 == Coulomb(self.many_elec, self.decomp_density)
        )
        self.assertTrue(self.has_value3 == Coulomb(self.nuclei, self.nuclei))

        self.assertFalse(self.default1 == self.default2)
        self.assertFalse(self.default1 == self.has_value1)
        self.assertFalse(self.default2 == self.has_value2)
        self.assertFalse(self.default3 == self.has_value3)

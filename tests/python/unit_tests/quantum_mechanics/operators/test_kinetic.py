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

from chemist import Electron, ManyElectrons, Nuclei, Nucleus
from chemist.qm_operator import Coulomb, Kinetic, OperatorBase


class TestKineticDispatch(unittest.TestCase):
    """Tests the single Kinetic class Python sees.

    C++'s Kinetic is templated on the type of the particle. Python instead gets
    one class, which picks the instantiation the particle implies. The
    instantiations stay exported, under underscore-prefixed names, so that
    these tests can name the type they expect; nothing else in Python is
    expected to name them.
    """

    def test_dispatches_on_particle(self):
        cases = [
            (Electron(), "_KineticElectron"),
            (ManyElectrons(4), "_KineticManyElectrons"),
            (Nucleus("H", 1, 1.0, 2.0, 3.0, 4.0, 5.0), "_KineticNucleus"),
            (Nuclei(), "_KineticNuclei"),
        ]
        for particle, name in cases:
            with self.subTest(particle=type(particle).__name__):
                self.assertEqual(type(Kinetic(particle)).__name__, name)

    def test_default_is_electron(self):
        self.assertEqual(type(Kinetic()).__name__, "_KineticElectron")
        self.assertEqual(Kinetic(), Kinetic(Electron()))

    def test_isinstance(self):
        t_e = Kinetic(Electron())
        self.assertIsInstance(t_e, Kinetic)
        self.assertIsInstance(t_e, OperatorBase)
        self.assertIsInstance(Kinetic(Nuclei()), Kinetic)

        # The instance check consults the instantiations, so it must not be
        # true for everything.
        self.assertNotIsInstance(Coulomb(), Kinetic)
        self.assertNotIsInstance(Electron(), Kinetic)

    def test_instantiations_are_registered_not_derived(self):
        """The instantiations must be virtual subclasses, never real ones.

        Kinetic dispatches by returning a _KineticElectron from __new__.
        CPython skips __init__ only because that object fails
        PyObject_TypeCheck against Kinetic, which is an MRO-based subtype test
        and is unaffected by the registration that makes isinstance work. Make
        an instantiation an actual subclass and pybind11's __init__ starts
        running a second time on an already-constructed object.
        """
        impl = type(Kinetic())

        # Registered, so isinstance and issubclass both say yes ...
        self.assertTrue(issubclass(impl, Kinetic))

        # ... but it is not a real subclass, which is what keeps __init__ from
        # running twice.
        self.assertNotIn(Kinetic, impl.__mro__)

    def test_init_runs_once_with_our_arguments(self):
        # A second __init__ would re-run pybind11's ctor on the live object.
        # Constructing with a non-default particle and checking it survived is
        # the observable half of that.
        many = ManyElectrons(4)
        self.assertEqual(Kinetic(many).particle, many)

    def test_bad_particle_throws(self):
        with self.assertRaisesRegex(TypeError, "kinetic energy of a str"):
            Kinetic("not a particle")

    def test_too_many_particles_throws(self):
        with self.assertRaisesRegex(TypeError, "at most one particle"):
            Kinetic(Electron(), Electron())

    def test_keyword_args_throw(self):
        with self.assertRaisesRegex(TypeError, "keyword arguments"):
            Kinetic(particle=Electron())


class TestKinetic(unittest.TestCase):
    def setUp(self):
        self.many_elec = ManyElectrons(4)
        self.kinetic_elec = Kinetic()
        self.kinetic_many1 = Kinetic(ManyElectrons())
        self.kinetic_many2 = Kinetic(self.many_elec)

    def test_particle(self):
        self.assertEqual(self.kinetic_elec.particle, Electron())
        self.assertEqual(self.kinetic_many1.particle, ManyElectrons())
        self.assertEqual(self.kinetic_many2.particle, self.many_elec)

        # Check for writing
        self.kinetic_many1.particle = self.many_elec
        self.assertEqual(self.kinetic_many1.particle, self.many_elec)

        # Should not be able to set incompatible particle types
        with self.assertRaises(TypeError):
            self.kinetic_elec.particle = self.many_elec

    def test_comparison(self):
        self.assertTrue(self.kinetic_elec == Kinetic())
        self.assertTrue(self.kinetic_many1 == Kinetic(ManyElectrons()))
        self.assertTrue(self.kinetic_many2 == Kinetic(self.many_elec))

        self.assertFalse(self.kinetic_elec == self.kinetic_many1)
        self.assertFalse(self.kinetic_many1 == self.kinetic_many2)

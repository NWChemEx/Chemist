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

import chemist.braket as braket
from chemist import DecomposableDensity, Electron, Nuclei
from chemist.braket import BraKet, TensorRepresentation
from chemist.qm_operator import (
    CoreHamiltonian,
    Coulomb,
    DensityCMOsElectron,
    Exchange,
    ExchangeCorrelation,
    Fock,
    Hamiltonian,
    Identity,
    Kinetic,
    xc_functional,
)

from ..test_qm import h2_aos, h2_determinant


class TestBraKetConversions(unittest.TestCase):
    """Tests that one Python BraKet resolves to the right C++ instantiation.

    Each chemist::braket::BraKet<Bra, Op, Ket> Chemist instantiates is exported
    under a private, underscore-prefixed name with a ctor taking the erased
    BraKet, plus a pybind11 implicit conversion. Nothing in Python is expected
    to name those classes; they exist so that pybind11 has a target type when
    PluginPlay converts a Python-provided input for a module. Calling them
    directly, as these tests do, is the same conversion PluginPlay performs.
    """

    def setUp(self):
        self.aos = h2_aos()
        self.aos2 = (self.aos, self.aos)
        self.psi = h2_determinant()

        self.rho = DecomposableDensity()

        # N.B. Built through the collapsed names, which is the point: an
        #      operator Python made without naming an instantiation still
        #      resolves to the instantiation each property type wants.
        self.t_e = Kinetic(Electron())
        self.v_ee = Coulomb(Electron(), Electron())

    def _round_trip(self, cls, bra, op, ket):
        """Converts and checks the pieces survived."""
        erased = BraKet(bra, op, ket)
        concrete = cls(erased)
        self.assertIsInstance(concrete, TensorRepresentation)
        self.assertEqual(concrete.bra, bra)
        self.assertEqual(concrete.op, op)
        self.assertEqual(concrete.ket, ket)
        return concrete

    def test_one_electron_operators(self):
        cases = [
            (braket._BraKet_aos_s_e_aos, Identity()),
            (braket._BraKet_aos_t_e_aos, self.t_e),
            (
                braket._BraKet_aos_v_en_aos,
                Coulomb(Electron(), Nuclei()),
            ),
            (braket._BraKet_aos_h_e_aos, CoreHamiltonian()),
            (braket._BraKet_aos_f_e_aos, Fock()),
            (
                braket._BraKet_aos_j_e_aos,
                Coulomb(Electron(), self.rho),
            ),
            (
                braket._BraKet_aos_k_e_aos,
                Exchange(Electron(), self.rho),
            ),
            (
                braket._BraKet_aos_xc_e_aos,
                ExchangeCorrelation(xc_functional.NONE, Electron(), self.rho),
            ),
            (braket._BraKet_aos_rho_e_cmos_aos, DensityCMOsElectron()),
        ]
        for cls, op in cases:
            with self.subTest(cls=cls.__name__):
                self._round_trip(cls, self.aos, op, self.aos)

    def test_electron_repulsion_integrals(self):
        self._round_trip(braket._BraKet_ERI2, self.aos, self.v_ee, self.aos)
        self._round_trip(braket._BraKet_ERI3, self.aos, self.v_ee, self.aos2)
        self._round_trip(braket._BraKet_ERI4, self.aos2, self.v_ee, self.aos2)

    def test_scf_energy(self):
        self._round_trip(
            braket._BraKet_ESCF_CMOs, self.psi, Hamiltonian(), self.psi
        )

    def test_widens_to_operator_base(self):
        """Any operator must convert to the erased-operator instantiation.

        simde.aos_op_base_aos is the property type every AO integral driver
        dispatches from: it takes a BraKet whose operator type is the abstract
        OperatorBase, and the module recovers the concrete operator with an
        OperatorVisitor. Without this conversion that entry point would be
        unreachable from Python.
        """
        for op in (Identity(), self.t_e, self.v_ee, Fock(), CoreHamiltonian()):
            with self.subTest(op=type(op).__name__):
                erased = BraKet(self.aos, op, self.aos)
                concrete = braket._BraKet_aos_op_base_aos(erased)
                self.assertTrue(concrete.op.are_equal(op))

    def test_mismatched_operator_is_rejected(self):
        erased = BraKet(self.aos, self.t_e, self.aos)
        with self.assertRaises(RuntimeError):
            braket._BraKet_ERI2(erased)

    def test_mismatched_ket_is_rejected(self):
        # Right operator, wrong number of centers.
        erased = BraKet(self.aos, self.v_ee, self.aos)
        with self.assertRaises(RuntimeError):
            braket._BraKet_ERI3(erased)

    def test_mismatched_bra_is_rejected(self):
        erased = BraKet(self.psi, Hamiltonian(), self.psi)
        with self.assertRaises(RuntimeError):
            braket._BraKet_aos_op_base_aos(erased)

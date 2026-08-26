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

from chemist import Electron, Nuclei
from chemist.braket import BraKet, TensorRepresentation
from chemist.qm_operator import Coulomb, Kinetic

from ..test_qm import h2_aos, h2_determinant


class TestBraKet(unittest.TestCase):
    """Tests the single, erased BraKet class Python sees.

    Chemist's C++ BraKet is templated on the types of the bra, the operator,
    and the ket. Python instead gets one class, which holds the three pieces
    and works out the C++ instantiation when something asks for one. See
    TestBraKetConversions for that half.
    """

    def setUp(self):
        self.aos = h2_aos()
        self.t_e = Kinetic(Electron())
        self.v_en = Coulomb(Electron(), Nuclei())
        self.v_ee = Coulomb(Electron(), Electron())
        self.t = BraKet(self.aos, self.t_e, self.aos)

    def test_value_ctor(self):
        self.assertEqual(self.t.bra, self.aos)
        self.assertEqual(self.t.op, self.t_e)
        self.assertEqual(self.t.ket, self.aos)

    def test_keyword_args(self):
        by_keyword = BraKet(bra=self.aos, op=self.t_e, ket=self.aos)
        self.assertEqual(by_keyword, self.t)

    def test_pair_bra_and_ket(self):
        # The three- and four-center integrals put a pair of spaces in the bra
        # and/or the ket; from Python that is a two-element tuple.
        eri3 = BraKet(self.aos, self.v_ee, (self.aos, self.aos))
        self.assertEqual(eri3.ket, (self.aos, self.aos))

        eri4 = BraKet((self.aos, self.aos), self.v_ee, (self.aos, self.aos))
        self.assertEqual(eri4.bra, (self.aos, self.aos))

    def test_wavefunction_bra_and_ket(self):
        psi = h2_determinant()
        escf = BraKet(psi, self.v_ee, psi)
        self.assertEqual(escf.bra, psi)
        self.assertEqual(escf.ket, psi)

    def test_is_a_tensor_representation(self):
        self.assertIsInstance(self.t, TensorRepresentation)

    def test_comparisons(self):
        self.assertEqual(self.t, BraKet(self.aos, self.t_e, self.aos))
        self.assertNotEqual(self.t, BraKet(self.aos, self.v_en, self.aos))
        self.assertNotEqual(
            self.t, BraKet(self.aos, self.t_e, (self.aos, self.aos))
        )

    def test_repr(self):
        # N.B. The operator really is a _KineticElectron; type_name strips the
        #      leading underscore so the private instantiation names the
        #      dispatching Kinetic class picks do not leak into the repr.
        self.assertEqual(repr(self.t), "BraKet(AOs|KineticElectron|AOs)")

    def test_bad_bra_throws(self):
        with self.assertRaisesRegex(RuntimeError, "bra of a BraKet"):
            BraKet(42, self.t_e, self.aos)

    def test_bad_op_throws(self):
        with self.assertRaisesRegex(RuntimeError, "operator of a BraKet"):
            BraKet(self.aos, self.aos, self.aos)

    def test_bad_ket_throws(self):
        with self.assertRaisesRegex(RuntimeError, "ket of a BraKet"):
            BraKet(self.aos, self.t_e, "not a space")

    def test_wrong_length_tuple_throws(self):
        with self.assertRaisesRegex(RuntimeError, "ket of a BraKet"):
            BraKet(self.aos, self.t_e, (self.aos, self.aos, self.aos))

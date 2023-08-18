#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from chemist import AOBasisSetF, AOBasisSetD
from chemist import AtomicBasisSetF, AtomicBasisSetD
from chemist import ShellF, ShellD
from chemist import ContractedGaussianF, ContractedGaussianD
from chemist import PrimitiveF, PrimitiveD
from chemist import PointF, PointD
from chemist import ShellType
import unittest


def make_aobs_test_case(aobs_type):
    """
    The test cases for AOBasisSetF and AOBasisSetD are basically the same aside
    from the types of the class. This function essentially templates the test 
    case on the type of the point.

    :param point_type: Either ``AOBasisSetF`` or ``AOBasisSetD``
    """

    class TestAOBasisSet(unittest.TestCase):
        def test_ctor(self):
            self.assertEqual(self.defaulted.n_shells(), 0)
            self.assertEqual(self.defaulted.n_aos(), 0)
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertTrue(self.defaulted.empty())
            self.assertEqual(self.defaulted.size(), 0)

        def test_add_center(self):
            self.defaulted.add_center(self.abs)
            self.assertEqual(self.defaulted.n_shells(), 1)
            self.assertEqual(self.defaulted.n_aos(), 1)
            self.assertEqual(self.defaulted.n_primitives(), 1)
            self.assertFalse(self.defaulted.empty())
            self.assertEqual(self.defaulted.size(), 1)
            self.assertEqual(self.defaulted.at(0), self.abs)

        def test_shell_range(self):
            self.assertEqual(self.has_abs.shell_range(0), (0, 1))

        def test_max_l(self):
            self.assertEqual(self.has_abs.max_l(), 0)

        def test_n_shells(self):
            self.assertEqual(self.defaulted.n_shells(), 0)
            self.assertEqual(self.has_abs.n_shells(), 1)

        def test_shell(self):
            self.assertEqual(self.has_abs.shell(0), self.shell_type(
                self.pure, 0, [0.0], [0.1], 1.0, 1.1, 1.2))

        def test_primitive_range(self):
            self.assertEqual(self.has_abs.primitive_range(0), (0, 1))

        def test_n_aos(self):
            self.assertEqual(self.defaulted.n_aos(), 0)
            self.assertEqual(self.has_abs.n_aos(), 1)

        def test_n_primitives(self):
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertEqual(self.has_abs.n_primitives(), 1)

        def test_primitive(self):
            self.assertEqual(self.has_abs.primitive(0),
                             self.prim_type(0.0, 0.1, 1.0, 1.1, 1.2))

        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.has_abs.empty())

        def test_at(self):
            self.assertEqual(self.has_abs.at(0), self.abs)

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.has_abs.size(), 1)

        def test_addition(self):
            pass

        def test_addition_inplace(self):
            pass

        def test_comparisons(self):
            # Default

            # Non-default

            # Different atomic basis set
            pass

        def setUp(self):
            # "typedefs"
            if aobs_type == AOBasisSetF:
                self.center_type = PointF
                self.prim_type = PrimitiveF
                self.cg_type = ContractedGaussianF
                self.shell_type = ShellF
                self.abs_type = AtomicBasisSetF
            else:
                self.center_type = PointD
                self.prim_type = PrimitiveD
                self.cg_type = ContractedGaussianD
                self.shell_type = ShellD
                self.abs_type = AtomicBasisSetD

            # Some inputs
            self.pure = ShellType.pure
            self.shell = self.shell_type(
                self.pure, 0, [0.0], [0.1], 0.2, 0.3, 0.4)
            self.abs = self.abs_type(
                "name", 1, self.center_type(1.0, 1.1, 1.2), [self.shell])

            # The instances being tested
            self.defaulted = aobs_type()
            self.has_abs = aobs_type()
            self.has_abs.add_center(self.abs)

    return TestAOBasisSet


class TestAOBasisSetF(make_aobs_test_case(AOBasisSetF)):
    pass


class TestAOBasisSetD(make_aobs_test_case(AOBasisSetD)):
    pass

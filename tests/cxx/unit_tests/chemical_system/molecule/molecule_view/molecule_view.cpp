#include "../../../test_helpers.hpp"
#include <chemist/chemical_system/molecule/molecule_view/molecule_view.hpp>

using namespace chemist;

using types2test = std::tuple<Molecule, const Molecule>;

TEMPLATE_LIST_TEST_CASE("MoleculeView", "", types2test) {
    using view_type        = MoleculeView<TestType>;
    using molecule_type    = typename view_type::molecule_type;
    using nuclei_reference = typename view_type::nuclei_reference;
    using nuclei_type = typename nuclei_reference::nuclei_traits::nuclei_type;
}

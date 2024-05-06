#include <catch2/catch.hpp>
#include <chemist/chemical_system/molcule_view/detail_/molecule_view_pimpl.hpp>

using namespace chemist;

using types2test = std::tuple<Molecule, const Molecule>;

TEMPLATE_LIST_TEST_CASE("MoleculeViewPIMPL", "", types2test) {
    using pimpl_type       = detail_::MoleculeViewPIMPL<TestType>;
    using nuclei_reference = typename pimpl_type::nuclei_reference;
}

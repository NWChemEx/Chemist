#pragma once
#include <chemist/nucleus/nuclei.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

class FragmentedNuclei : public utilities::IndexableContainerBase<FragmentedNuclei>{

public:
    using supersystem_type = Nuclei;

    explicit FragmentedNuclei(supersystem_type supersystem);

};

}
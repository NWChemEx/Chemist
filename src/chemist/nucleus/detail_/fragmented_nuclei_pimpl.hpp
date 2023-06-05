#pragma once
#include <chemist/nucleus/fragmented_nuclei.hpp>

namespace chemist::detail_ {

class FragmentedNucleiPIMPL {
public:
    using parent_type = FragmentedNuclei;

    using supersystem_type = parent_type::supersystem_type;

    using const_supersystem_pointer = std::shared_ptr<const supersystem_type>;

    using size_type = supersystem_type::size_type;

    explicit FragmentedNucleiPIMPL(const_supserystem_pointer supersystem) :
      m_supersystem_(supersystem) {}

private:
    const_supserystem_pointer m_supersystem_;

    std::vector<std::set<size_type>> m_frags_;
};

} // namespace chemist::detail_
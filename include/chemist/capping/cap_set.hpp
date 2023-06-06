#pragma once
#include <chemist/capping/cap.hpp>
#include <utilities/containers/indexable_container_base.hpp>
#include <vector>

namespace chemist {

class CapSet : public utilities::IndexableContainerBase<CapSet> {
private:
    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<CapSet>;

public:
    using value_type = Cap;

    using reference = value_type&;

    using const_reference = const value_type&;

    using atom_type = value_type::atom_type;

    using atom_index_type = value_type::size_type;

    using size_type = typename base_type::size_type;

    void push_back(value_type cap) { m_caps_.push_back(std::move(cap)); }

    template<typename... Args>
    void add_cap(atom_index_type anchor, atom_index_type replaced,
                 Args&&... atoms) {
        push_back(value_type(anchor, replaced, std::forward<Args>(atoms)...));
    }

private:
    /// Container the caps are stored in
    using cap_set = std::vector<value_type>;

    /// Used to implement operator[]/at
    reference at_(size_type i) noexcept { return m_caps_[i]; }

    /// Used to implement operator[] const/at const
    const_reference at_(size_type i) const noexcept { return m_caps_[i]; }

    /// Used to implement size()
    size_type size_() const noexcept { return m_caps_.size(); }

    cap_set m_caps_;
};

} // namespace chemist

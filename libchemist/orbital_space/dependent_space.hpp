#pragma once

namespace libchemist::orbital_space {

template<typename BaseType>
class DependentSpace : public BaseType {
public:
    /** @brief The SparseMap from the independent space to members of this space
     *
     * @return The internal SparseMap instance in a read/write format.
     * @throw none No throw guarantee.
     */
    auto& sparse_map() { return m_sm_; }

    const auto& sparse_map() const { return m_sm_; }

private:
    SparseMap m_sm_;
};

} // namespace libchemist::orbital_space

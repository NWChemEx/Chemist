#pragma once
#include "libchemist/orbital_space/orbital_space_class.hpp"
#include "libchemist/orbital_space/orthogonal_space.hpp"
#include "libchemist/sparse_map/sparse_map.hpp"

namespace libchemist {

template<typename BaseSpace>
class DependentOrbitalSpace : public BaseSpace {
public:
    template<typename...Args>
    DependentOrbitalSpace(sparse_map::SparseMap sm, Args&&...args);

    /** @brief The SparseMap from the independent space to this space
     *
     * @return The internal SparseMap instance in a read/write format.
     * @throw none No throw guarantee.
     */
    auto& sparse_map() { return m_sm_; }

    const auto& sparse_map() const { return m_sm_; }
private:
    sparse_map::SparseMap m_sm_;
};

template<typename T>
using DependentOrbitals =
  DependentOrbitalSpace<OrbitalSpace<T, type::tensor_of_tensors<T>>>;

template<typename T>
using DependentOrthogonalSpace =
  DependentOrbitalSpace<OrthogonalSpace<T, type::tensor_of_tensors<T>>>;

//----------------------------- Implementations --------------------------------
template<typename BaseSpace>
template<typename...Args>
DependentOrbitalSpace<BaseSpace>::
  DependentOrbitalSpace(sparse_map::SparseMap sm, Args&&...args) :
 m_sm_(std::move(sm)), BaseSpace(std::forward<Args>(args)...) {}


} // namespace libchemist

#include "chemist/basis_set_map.hpp"
#include <utilities/iter_tools/enumerate.hpp>

namespace chemist {
/*
using size_type = std::size_t;

template<typename T>
BasisSetMap<T>::BasisSetMap(const BasisSetMap& rhs) :
  a2s_vec(rhs.a2s_vec),
  s2ao_vec(rhs.s2ao_vec),
  ao2s_vec(rhs.ao2s_vec),
  s2a_vec(rhs.s2a_vec) {}

template<typename T>
BasisSetMap<T>::BasisSetMap() = default;

template<typename T>
BasisSetMap<T>::BasisSetMap(BasisSetMap&& rhs) noexcept :
  a2s_vec(rhs.a2s_vec),
  s2ao_vec(rhs.s2ao_vec),
  ao2s_vec(rhs.ao2s_vec),
  s2a_vec(rhs.s2a_vec) {}

template<typename T>
BasisSetMap<T>& BasisSetMap<T>::operator=(const BasisSetMap& rhs) {
    return *this = std::move(BasisSetMap(rhs));
}

template<typename T>
BasisSetMap<T>& BasisSetMap<T>::operator=(BasisSetMap&& rhs) noexcept {
    if(this != &rhs) {
        a2s_vec  = rhs.a2s_vec;
        s2ao_vec = rhs.s2ao_vec;
        ao2s_vec = rhs.ao2s_vec;
        s2a_vec  = rhs.s2a_vec;
    }
    return *this;
}

template<typename T>
BasisSetMap<T>::~BasisSetMap() noexcept = default;

template<typename T>
BasisSetMap<T>::BasisSetMap(const AOBasisSet<T>& bs) {
    size_type a2s_offset  = 0;
    size_type s2ao_offset = 0;

    for(const auto& [atom_idx, center] : utilities::Enumerate(bs)) {
        a2s_vec.push_back(
          std::make_pair(a2s_offset, a2s_offset + center.size()));
        for(const auto& [shell_idx, shell] : utilities::Enumerate(center)) {
            s2a_vec.push_back(atom_idx);
            s2ao_vec.push_back(
              std::make_pair(s2ao_offset, s2ao_offset + shell.size()));
            for(size_type i = 0; i < shell.size(); ++i) {
                ao2s_vec.push_back(shell_idx + a2s_offset);
            }
            s2ao_offset += shell.size();
        }
        a2s_offset += center.size();
    }
}

template class BasisSetMap<double>;
template class BasisSetMap<float>;
*/
} // namespace chemist

#include <chemist/basis_set_map.hpp>

namespace chemist {

using size_type = std::size_t;

BasisSetMap::BasisSetMap(const BasisSetMap& rhs) :
  a2s_vec(rhs.a2s_vec),
  s2ao_vec(rhs.s2ao_vec),
  ao2s_vec(rhs.ao2s_vec),
  s2a_vec(rhs.s2a_vec) {}

BasisSetMap::BasisSetMap() = default;

BasisSetMap::BasisSetMap(BasisSetMap&& rhs) noexcept :
  a2s_vec(rhs.a2s_vec),
  s2ao_vec(rhs.s2ao_vec),
  ao2s_vec(rhs.ao2s_vec),
  s2a_vec(rhs.s2a_vec) {}

BasisSetMap& BasisSetMap::operator=(const BasisSetMap& rhs) {
    return *this = std::move(BasisSetMap(rhs));
}

BasisSetMap& BasisSetMap::operator=(BasisSetMap&& rhs) noexcept {
    if(this != &rhs) {
        a2s_vec  = rhs.a2s_vec;
        s2ao_vec = rhs.s2ao_vec;
        ao2s_vec = rhs.ao2s_vec;
        s2a_vec  = rhs.s2a_vec;
    }
    return *this;
}

BasisSetMap::~BasisSetMap() noexcept = default;

BasisSetMap::BasisSetMap(const AOBasisSet& bs) {
    auto prev_coords = bs[0].center();

    size_type shell_beg   = 0;
    size_type shell_end   = 0;
    size_type s2ao_offset = 0;
    size_type atom        = 0;
    for(auto shell : bs) {
        s2ao_vec.push_back(
          std::make_pair(s2ao_offset, s2ao_offset + shell.size()));

        for(size_type idx = s2ao_offset; idx < s2ao_offset + shell.size();
            idx++) {
            ao2s_vec.push_back(shell_end);
        }

        s2ao_offset += shell.size();
        if(shell.center() != prev_coords) {
            a2s_vec.push_back(std::make_pair(shell_beg, shell_end));
            prev_coords = shell.center();
            shell_beg   = shell_end;
            ++atom;
        }
        s2a_vec.push_back(atom);
        shell_end++;
    }
    a2s_vec.push_back(std::make_pair(shell_beg, shell_end));
}

} // namespace chemist

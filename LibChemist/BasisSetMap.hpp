#pragma once
#include <LibChemist/AOBasisSet.hpp>
#include <vector>
#include <utility> //std::pair

namespace LibChemist {
  class BasisSetMap {
  public:
    using range = typename std::pair<size_t, size_t>;

    //Constructs vectors that hold the relationships between Atom<->Shell<->AO indices
    BasisSetMap(const AOBasisSet& bs);

    //Given an atom index, returns the range of shell indices corresponding to the atom
    range atom_to_shell(const size_t idx){
      return a2s_vec.at(idx);
    };

    //Given an atom index, returns the range of ao indices corresponding to the atom    
    range atom_to_ao(const size_t idx){
        auto sr = atom_to_shell(idx);
        return std::make_pair(s2ao_vec.at(sr.first).first, s2ao_vec.at(sr.second-1).second);
    }

    //Given a shell index, returns the range of ao indices corresponding to the shell        
    range shell_to_ao(const size_t idx){
        return s2ao_vec.at(idx);
    }

    //Given a shell index, returns the index of the atom that the shell belongs to        
    size_t shell_to_atom(const size_t idx){
        return s2a_vec.at(idx);
    }

    //Given an ao index, returns the index of the shell that the ao belongs to            
    size_t ao_to_shell(const size_t idx){
        return ao2s_vec.at(idx);
    }

    //Given an ao index, returns the index of the atom that the ao belongs to                
    size_t ao_to_atom(const size_t idx){
        const size_t shell_idx = ao_to_shell(idx);
        return shell_to_atom(shell_idx);
    }

private:
    std::vector<range> a2s_vec;
    std::vector<range> s2ao_vec;
    std::vector<size_t> ao2s_vec;
    std::vector<size_t> s2a_vec;

};    

} // namespace LibChemist

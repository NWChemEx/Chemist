#include <LibChemist/BasisSetMap.hpp>

namespace LibChemist {
  
  BasisSetMap::BasisSetMap(const AOBasisSet& bs){
      auto prev_coords=bs[0].center();
      
      size_t shell_beg=0;
      size_t shell_end=0;
      size_t s2ao_offset=0;
      size_t atom = 0;
      for(auto shell : bs){
	s2ao_vec.push_back(std::make_pair(s2ao_offset, s2ao_offset+shell.size()));
	
	for(size_t idx = s2ao_offset; idx<s2ao_offset+shell.size(); idx++ )
	  {
	    ao2s_vec.push_back(shell_end);
	  }
	
	
	s2ao_offset+=shell.size();
	if (shell.center() != prev_coords){
	  a2s_vec.push_back(std::make_pair(shell_beg, shell_end));
	  prev_coords=shell.center();
	  shell_beg=shell_end;
	  ++atom;
	}
	s2a_vec.push_back(atom);
	shell_end++;
      }
      a2s_vec.push_back(std::make_pair(shell_beg, shell_end));
      
  }

} // namespace LibChemist

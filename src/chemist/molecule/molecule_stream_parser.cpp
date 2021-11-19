// #include "chemist/molecule/molecule_stream_parser.hpp"
// #include <cmath>    //For lround
// #include <iostream> //debug
// #include <regex>
// #include <sstream>

// namespace chemist {

// using action_type = MoleculeStreamParser::action_type;
// using data_type   = MoleculeStreamParser::data_type;
// using return_type = std::map<data_type, std::vector<double>>;
// using Molecule    = chemist::Molecule;

// namespace detail_ {
// struct atom {
//     double Z = 0.0;
//     std::array<double, 3> xyz;
// };

// void commit_atom(chemist::Molecule& rv, atom& a,
//                  const chemist::PeriodicTable& pt) {
//     if(a.Z != 0.0) {
//         auto temp     = pt.get_atom(a.Z);
//         temp.coords() = a.xyz;
//         rv.push_back(temp);
//     }
//     a = atom();
// }

// void parse(const return_type& data, atom& a) {
//     if(data.count(data_type::AtNum)) a.Z = data.at(data_type::AtNum)[0];
//     if(data.count(data_type::x)) a.xyz[0] = data.at(data_type::x)[0];
//     if(data.count(data_type::y)) a.xyz[1] = data.at(data_type::y)[0];
//     if(data.count(data_type::z)) a.xyz[2] = data.at(data_type::z)[0];
// }

// } // end namespace detail_

// Molecule parse_molecule_stream(std::istream& is,
//                                const MoleculeStreamParser& parser,
//                                const chemist::PeriodicTable& pt) {
//     Molecule rv;
//     detail_::atom a;
//     double charge{0.0};
//     double mult{1.0};
//     while(is) {
//         std::string line;
//         std::getline(is, line);
//         switch(parser.worth_parsing(line)) {
//             case(action_type::none): {
//                 break;
//             } // Junk line
//             case(action_type::new_atom): {
//                 detail_::commit_atom(rv, a, pt);
//                 // Intentional fall_through
//             }
//             case(action_type::same_atom): {
//                 auto data = parser.parse(line, pt);
//                 detail_::parse(data, a);
//                 break;
//             }
//             default: {
//                 auto data = parser.parse(line, pt);
//                 if(data.count(data_type::charge))
//                     charge = data.at(data_type::charge)[0];
//                 if(data.count(data_type::multiplicity))
//                     mult = data.at(data_type::multiplicity)[0];
//                 break;
//             }
//         }
//     }
//     detail_::commit_atom(rv, a, pt);
//     rv.multiplicity() = mult;
//     rv.charge()       = charge;
//     long nelectrons   = -1 * std::lround(charge);
//     for(const auto& ai : rv) nelectrons += ai.Z();
//     const long nopen   = std::lround(mult) - 1;
//     const long nclosed = nelectrons - nopen;
//     if(nclosed % 2) {
//         auto msg = "Charge: " + std::to_string(charge) +
//                    "Multiplicity: " + std::to_string(mult) +
//                    "not possible for " + std::to_string(nelectrons) +
//                    " system.";
//         throw std::domain_error(msg);
//     }
//     return rv;
// }

// // charge and multiplicity
// static const std::regex xyz_cm("^\\s*-?\\d+.?\\d*\\s+\\d+.?\\d*\\s*$");
// static const std::regex xyz_atom(
//   "^\\s*[a-zA-Z]+\\s*(?:-?\\d*.\\d+(?:(?:e|E)(?:-|\\+)?\\d+)?\\s*){3}$");

// action_type XYZParser::worth_parsing(const std::string& line) const {
//     if(std::regex_search(line, xyz_cm))
//         return action_type::overall_system;
//     else if(std::regex_search(line, xyz_atom))
//         return action_type::new_atom;
//     return action_type::none;
// }

// return_type XYZParser::parse(const std::string& line,
//                              const chemist::PeriodicTable& pt) const {
//     return_type rv;

//     std::stringstream tokenizer(line);
//     if(std::regex_search(line, xyz_cm)) {
//         double temp;
//         tokenizer >> temp;
//         rv[data_type::charge].push_back(temp);
//         tokenizer >> temp;
//         rv[data_type::multiplicity].push_back(temp);
//     } else if(std::regex_search(line, xyz_atom)) {
//         std::string sym;
//         tokenizer >> sym;
//         double temp = pt.sym_2_Z(sym);
//         rv[data_type::AtNum].push_back(temp);
//         tokenizer >> temp;
//         rv[data_type::x].push_back(temp);
//         tokenizer >> temp;
//         rv[data_type::y].push_back(temp);
//         tokenizer >> temp;
//         rv[data_type::z].push_back(temp);
//     }

//     return rv;
// }

// } // namespace chemist

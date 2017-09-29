#include "LibChemist/ShellTypes.hpp"
#include <unordered_map>

namespace LibChemist{
namespace detail_ {

static const std::unordered_map<std::string, int> toi_{ { "spdfgh", -5 },
                                                        { "spdfg",  -4 },
                                                        { "spdf",   -3 },
                                                        { "spd",    -2 },
                                                        { "sp",     -1 },
                                                        { "s",       0 },
                                                        { "p",       1 },
                                                        { "d",       2 },
                                                        { "f",       3 },
                                                        { "g",       4 },
                                                        { "h",       5 },
                                                        { "i",       6 },
                                                        { "j",       7 },
                                                        { "k",       8 },
                                                        { "l",       9 },
                                                        { "m",      10 },
                                                        { "n",      11 },
                                                        { "o",      12 },
                                                        { "q",      13 },
                                                        { "r",      14 },
                                                        { "t",      15 },
                                                        { "u",      16 },
                                                        { "v",      17 },
                                                        { "w",      18 },
                                                        { "x",      19 },
                                                        { "y",      20 },
                                                        { "z",      21 } };

static const std::unordered_map<int, std::string> tostr_{ { -5, "spdfgh"},
                                                          { -4, "spdfg"},
                                                          { -3, "spdf"},
                                                          { -2, "spd"},
                                                          { -1, "sp"},
                                                          {  0, "s"},
                                                          {  1, "p"},
                                                          {  2, "d"},
                                                          {  3, "f"},
                                                          {  4, "g"},
                                                          {  5, "h"},
                                                          {  6, "i"},
                                                          {  7, "j"},
                                                          {  8, "k"},
                                                          {  9, "l"},
                                                          { 10, "m"},
                                                          { 11, "n"},
                                                          { 12, "o"},
                                                          { 13, "q"},
                                                          { 14, "r"},
                                                          { 15, "t"},
                                                          { 16, "u"},
                                                          { 17, "v"},
                                                          { 18, "w"},
                                                          { 19, "x"},
                                                          { 20, "y"},
                                                          { 21, "z"} };
}//End namespace detail_

int am_str2int(const std::string& am)
{
    return detail_::toi_.at(am);
}

std::string am_int2str(int am)
{
    return detail_::tostr_.at(am);
}

size_t am_2int(int l, size_t i)
{
    //Combined AMs are such that the i-th character is the angular momentum
    //letter we need.
    if(l<0)
    {
        char am=am_int2str(l)[i];
        return am_str2int(std::string(1,am));
    }
    return l;
}


}//End namespace LibChemist

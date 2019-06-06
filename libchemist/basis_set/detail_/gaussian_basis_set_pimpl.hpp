#pragma once
#include "libchemist/basis_set/detail_/utility.hpp"
#include "libchemist/types.hpp"

namespace libchemist::basis_set::detail_ {

template<typename T = double>
class GaussianBasisSetPIMPL {
public:
    GaussianBasisSetPIMPL() : m_shell_off_(1, 0), m_prim_off_(1, 0) {}

    /// Totals
    ///@{
    /// The total number of centers
    type::size ncenters() const noexcept { return m_shell_off_.size() - 1; }

    /// The total number of shells
    type::size nshells() const noexcept { return m_prim_off_.size() - 1; }

    /// The total number of AOs
    type::size naos() const noexcept;

    /// The total number of unique primitives
    type::size nunique_prims() const noexcept { return m_cs_.size(); }
    ///@}

    auto center(type::size i);
    auto center(type::size i) const;

    /// Totals per center
    ///@{
    /// The total number of shells in center @p c
    type::size nshells(type::size c) const;

    /// The total number of AOs in center @p c
    type::size naos(type::size c) const;

    /// The total number of unique primitives in center @p c
    type::size nunique_prims(type::size c) const;
    ///@}

    /// Totals per AO
    ///@{
    /// The total number of AOs in center @p c's shell @p s
    type::size naos(type::size c, type::size s) const;

    /// The total number of unique primitives in center @p c, shell @p s
    type::size nunique_prims(type::size c, type::size s) const;
    ///@}

    /// The total number of unique primitives in center @p c, shell @p s, AO @p
    /// a
    type::size nunique_prims(type::size c, type::size s, type::size a) const;

    /// The purity of center @p c's shell @p s
    bool pure(type::size c, type::size s) const;

    /// The angular momentum of center @p c's shell @p s
    auto& l(type::size c, type::size s);

    /// The angular momentum of center @p c's shell @p s
    auto& l(type::size c, type::size s) const;

    /// The center of center @p c
    auto center(type::size c) const;

    /// The center of center @p c, shell @p s
    auto center(type::size c, type::size) const;

    /// The center of center @p c, shell @p s, ao @p a
    auto center(type::size c, type::size, type::size) const;

    /// The center of center @p c, shell @p s, ao @p a, prim @p p
    auto center(type::size c, type::size, type::size, type::size) const;

    /// The coefficient of center @p c, shell @p s, ao @p a, prim @p p
    auto& coefficient(type::size c, type::size s, type::size a, type::size p);

    /// The coefficient of center @p c, shell @p s, ao @p a, prim @p p
    auto& coefficient(type::size c, type::size s, type::size a,
                      type::size p) const;

    /// The exponentt of center @p c, shell @p s, ao @p a, prim @p p
    auto& exponent(type::size c, type::size s, type::size a, type::size p);

    /// The exponent of center @p c, shell @p s, ao @p a, prim @p p
    auto& exponent(type::size c, type::size s, type::size a,
                   type::size p) const;

    void add_center(type::coord<T> r, type::size nshells);

    void add_shell(bool pure, type::angular_momentum l, std::vector<T> cs,
                   std::vector<T> as);

private:
    /// Validates center index
    void check_center_(type::size c) const;
    /// Validates shell index
    void check_shell_(type::size c, type::size s) const;
    /// Validates AO index
    void check_ao_(type::size c, type::size s, type::size a) const;
    /// Validates primitive index
    void check_prim_(type::size c, type::size s, type::size a,
                     type::size p) const;

    /// 1-D index for the @p s-th shell of center @p c
    auto shell_idx_(type::size c, type::size s) const;
    /// 1-D index for the @p p-th primitive of the @p s-th shell of center @p c
    auto prim_idx_(type::size c, type::size s, type::size, type::size p) const;

    /// The centers' x-coordinate
    std::vector<T> m_x_;
    /// The centers' y-coordinate
    std::vector<T> m_y_;
    /// The centers/ z-coordinate
    std::vector<T> m_z_;
    /// i/i+1 element is where center i's shells start/end
    std::vector<type::size> m_shell_off_;
    /// The angular momentum of each shell
    std::vector<type::angular_momentum> m_ls_;
    /// The purity of each shell
    std::vector<unsigned short> m_pure_;
    /// i/i+1 element is where shell i's primitives start/end
    std::vector<type::size> m_prim_off_;
    /// The contraction coefficient of each primitive
    std::vector<T> m_cs_;
    /// The exponents of each primitive
    std::vector<T> m_as_;
};

//---------------------------------Implementations------------------------------
#define CLASS_TYPE GaussianBasisSetPIMPL<T>

template<typename T>
type::size CLASS_TYPE::nshells(type::size c) const {
    check_center_(c);
    return m_shell_off_[c + 1] - m_shell_off_[c];
}

template<typename T>
type::size CLASS_TYPE::naos() const noexcept {
    type::size rv = 0;
    for(type::size i = 0; i < ncenters(); ++i) rv += naos(i);
    return rv;
}

template<typename T>
type::size CLASS_TYPE::naos(type::size c) const {
    // c checked by nshells
    type::size rv = 0;
    for(type::size i = 0; i < nshells(c); ++i) rv += naos(c, i);
    return rv;
}

template<typename T>
type::size CLASS_TYPE::naos(type::size c, type::size s) const {
    // c s checked by l
    const auto am = l(c, s);
    return pure(c, s) ? 2 * am + 1 : detail_::bc(am + 2, 2);
}

template<typename T>
type::size CLASS_TYPE::nunique_prims(type::size c) const {
    // c checked by nshells
    type::size rv = 0;
    for(type::size i = 0; i < nshells(c); ++i) rv += nunique_prims(c, i);
    return rv;
}

template<typename T>
type::size CLASS_TYPE::nunique_prims(type::size c, type::size s) const {
    // c, s checked by nunique_prims
    return nunique_prims(c, s, 0);
}

template<typename T>
type::size CLASS_TYPE::nunique_prims(type::size c, type::size s,
                                     type::size a) const {
    check_ao_(c, s, a);
    const auto si = shell_idx_(c, s);
    return m_prim_off_[si + 1] - m_prim_off_[si];
}

template<typename T>
bool CLASS_TYPE::pure(type::size c, type::size s) const {
    // c, s checked by shell_idx_
    return m_pure_[shell_idx_(c, s)] == 1;
}

template<typename T>
auto& CLASS_TYPE::l(type::size c, type::size s) {
    // c s checked by shell_idx_
    return m_ls_[shell_idx_(c, s)];
}

template<typename T>
auto& CLASS_TYPE::l(type::size c, type::size s) const {
    // c s checked by shell_idx_
    return m_ls_[shell_idx_(c, s)];
}

template<typename T>
auto& CLASS_TYPE::coefficient(type::size c, type::size s, type::size a,
                              type::size p) {
    // c, s, a, p checked by prim_idx_
    return m_cs_[prim_idx_(c, s, a, p)];
}

template<typename T>
auto& CLASS_TYPE::coefficient(type::size c, type::size s, type::size a,
                              type::size p) const {
    // c, s, a, p checked by prim_idx_
    return m_cs_[prim_idx_(c, s, a, p)];
}

template<typename T>
auto& CLASS_TYPE::exponent(type::size c, type::size s, type::size a,
                           type::size p) {
    // c, s, a, p checked by prim_idx_
    return m_as_[prim_idx_(c, s, a, p)];
}

template<typename T>
auto& CLASS_TYPE::exponent(type::size c, type::size s, type::size a,
                           type::size p) const {
    // c, s, a, p checked by prim_idx_
    return m_as_[prim_idx_(c, s, a, p)];
}

template<typename T>
auto CLASS_TYPE::center(type::size c) const {
    check_center_(c);
    return type::coord<T>{m_x_[c], m_y_[c], m_z_[c]};
}

template<typename T>
auto CLASS_TYPE::center(type::size c, type::size s) const {
    check_shell_(c, s);
    return center(c);
}

template<typename T>
auto CLASS_TYPE::center(type::size c, type::size s, type::size a) const {
    check_ao_(c, s, a);
    return center(c);
}

template<typename T>
auto CLASS_TYPE::center(type::size c, type::size s, type::size a,
                        type::size p) const {
    check_prim_(c, s, a, p);
    return center(c);
}

template<typename T>
void CLASS_TYPE::add_center(type::coord<T> r, type::size n) {
    if(nshells() != m_shell_off_.back())
        throw std::runtime_error("Did not finish last center!");

    m_x_.push_back(r[0]);
    m_y_.push_back(r[1]);
    m_z_.push_back(r[2]);
    m_shell_off_.push_back(m_shell_off_.back() + n);
}

template<typename T>
void CLASS_TYPE::add_shell(bool pure, type::angular_momentum l,
                           std::vector<T> cs, std::vector<T> as) {
    if(nshells() == m_shell_off_.back())
        throw std::runtime_error("Added more shells than specified");
    m_pure_.push_back(pure);
    m_ls_.push_back(l);
    m_prim_off_.push_back(m_prim_off_.back() + cs.size());
    for(auto x : cs) m_cs_.push_back(x);
    for(auto x : as) m_as_.push_back(x);
}

template<typename T>
void CLASS_TYPE::check_center_(type::size c) const {
    if(c >= ncenters())
        throw std::out_of_range("Requested center is > ncenters()");
}

template<typename T>
void CLASS_TYPE::check_shell_(type::size c, type::size s) const {
    if(s >= nshells(c))
        throw std::out_of_range("Requested shell is > nshells(c)");
}

template<typename T>
void CLASS_TYPE::check_ao_(type::size c, type::size s, type::size a) const {
    if(a >= naos(c, s)) throw std::out_of_range("Requested AO is > naos(c, s)");
}

template<typename T>
void CLASS_TYPE::check_prim_(type::size c, type::size s, type::size a,
                             type::size p) const {
    check_ao_(c, s, a);
    if(p >= nunique_prims(c, s, a))
        throw std::out_of_range("Requested prim is > nunique_prims(c, s, a)");
}

template<typename T>
auto CLASS_TYPE::shell_idx_(type::size c, type::size s) const {
    check_shell_(c, s);
    return m_shell_off_[c] + s;
}

template<typename T>
auto CLASS_TYPE::prim_idx_(type::size c, type::size s, type::size a,
                           type::size p) const {
    check_prim_(c, s, a, p);
    return m_prim_off_[shell_idx_(c, s)] + p;
}

#undef CLASS_TYPE

} // namespace libchemist::basis_set::detail_

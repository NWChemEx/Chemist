#pragma once

namespace libchemist::detail_ {

template<typename T>
class ShellPIMPL {
private:
    utilities::OwnOrBorrow<bool> m_pure_;
    utilities::OwnOrBorrow<int> m_l_;
};

template<typename T, typename Container>
class ShellPIMPL_ {
private:
    Container
};

template<

} // namespace libchemist::detail_

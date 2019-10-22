#pragma once

namespace libchemist::detail_ {

template<typename T>
class AOBasisSetPIMPL {
public:
    using size_type = std::size_t;

    size_type size() const noexcept { return m_centers_.size(); }
    void add_center(Center<T> c) { m_centers_.emplace_back(std::move(c)); }
    Center<T>& at(size_type i) { return m_centers_[i]; }
    const Center<T>& at(size_type i) const { return m_centers_[i]; }

private:
    std::vector<Center<T>> m_centers_;
};

} // namespace libchemist::detail_

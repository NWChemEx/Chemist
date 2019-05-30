
template<typename T>
class GaussianShell {
    const auto& coef(type::size ao, type::size prim) const;
    const auto& center(type::size ao, type::size prim) const {
        return m_center_;
    }
    auto& coef(type::size prim);

private:
};

template<typename T>
class GaussianCenter {};

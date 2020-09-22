#pragma once

namespace libchemist::sparse_map {

/** @brief Thin wrapper class around a multi-rank index.
 *
 *
 */
class Index{
public:
    using size_type  = std::size_t;
    using index_type = std::vector<size_type>;

    Index() = default;
    template<typename...Args>
    Index(size_type i0, Args&&...args);
    Index(std::initializer_list<size_type> il) : m_index_(il) {}
private:
    index_type m_index_;
};

/** @brief Strong type signaling that a tuple of integers should be regarded as
 *         the index of an element in the tensor.
 *
 */
class ElementIndex : public Index {};

/** @brief Strong type signaling that a tuple of integers should be regarded as
 *         the index of a tile in the tensor.
 */
class TileIndex : public Index{};

// --------------------------- Implementations ---------------------------------

template<typename...Args>
Index::Index(size_type i0, Args&&...args) :
 Index({i0, size_type(std::forward<Args>(args))...}) {}



}

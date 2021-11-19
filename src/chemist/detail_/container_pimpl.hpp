#pragma once
#include <memory>

namespace chemist::detail_ {

/**
 * @brief A PIMPL base class suitable for use by a container-like class
 *
 * This class defines a class that PIMPLs for container-like classes can inherit
 * from.  The container class makes all calls through the public API of this
 * class and the actual PIMPL variations implement the virtual functions to make
 * it all happen.
 *
 * @tparam T The type of the class using the PIMPL
 */
template<typename T>
class ContainerPIMPL {
public:
    /// Get some types from the class we're ultimately implementing.
    ///@{
    using value_type     = typename T::value_type;
    using reference      = typename T::reference;
    using size_type      = typename T::size_type;
    using iterator       = typename T::iterator;
    using const_iterator = typename T::const_iterator;
    ///@}

    /// Default CTor and DTor, no state so do nothing
    ///@{
    ContainerPIMPL()          = default;
    virtual ~ContainerPIMPL() = default;
    ///@}

    /// Performs a polymorphic deep copy of the class
    std::unique_ptr<ContainerPIMPL<T>> clone() const { return clone_(); }

    /// The following are the core methods of an STL-like container and the
    // public API of this class
    ///@{
    reference at(size_type i) { return at_(i); }
    size_type size() const noexcept { return size_(); }
    void push_back(value_type v) { push_back_(std::move(v)); }
    iterator begin() noexcept { return begin_(); }
    const_iterator begin() const noexcept { return begin_(); }
    iterator end() noexcept { return end_(); }
    const_iterator end() const noexcept { return end_(); }
    ///@}
protected:
    /// Protected to avoid slicing, but to make sure it's usable by derived
    /// class
    ///@{
    ContainerPIMPL(const ContainerPIMPL& rhs) = default;
    ContainerPIMPL& operator=(const ContainerPIMPL& rhs) = default;
    ///@}

    /// Deleted to avoid slicing
    ///@{
    ContainerPIMPL(ContainerPIMPL&& rhs) = delete;
    ContainerPIMPL& operator=(ContainerPIMPL&& rhs) = delete;
    ///@}
private:
    /// The hooks for your derived class to implement
    ///@{
    virtual std::unique_ptr<ContainerPIMPL<T>> clone_() const = 0;
    virtual reference at_(size_type i)                        = 0;
    virtual size_type size_() const noexcept                  = 0;
    virtual void push_back_(value_type v)                     = 0;
    virtual iterator begin_() noexcept                        = 0;
    virtual const_iterator begin_() const noexcept            = 0;
    virtual iterator end_() noexcept                          = 0;
    virtual const_iterator end_() const noexcept              = 0;
    ///@}
};

} // namespace chemist::detail_

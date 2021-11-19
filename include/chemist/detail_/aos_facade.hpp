#pragma once
#include <array>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <vector>

/**
 * @file aos_facade.hpp
 *
 * This file contains a class AoSFacade, which aids in implementing an
 * API that is array of structures (AoS) like when the data is actually stored
 * as a structure of arrays (SoA). To this end all data is stored in shared
 * buffers of which each element and the AoSFacade has a reference to (ensuring
 * the buffers are not deleted from under any of them).  Elements then store
 * their offsets in the buffer (as well as their spans) and return their values
 * by basic pointer arithmetic.
 */

namespace chemist::detail_ {
/// Forward declare the AoSFacade class for friendship reasons
template<typename T>
class AoSFacade;

/**
 * @brief The class that wraps an element within the AoSFacade
 *
 * User declared classes can not be inserted directly into an array with the
 * guarantee that the members will be contiguous.  This is because the class
 * is opaque.  The AoSElement is meant to hold the state of the class (the
 * actual class's accessors then being a thin wrapper over the AoSElement) in a
 * manner allowing for the fields to be stored contiguously.
 *
 * @tparam Fields The types of the fields associated with this element.
 */
template<typename... Fields>
class AoSElement {
    /// The type of this class
    using my_type = AoSElement<Fields...>;

    /// The type of a class storing one of the fields contiguously
    template<typename T>
    using vector_type = std::vector<T>;

    /// The type of a shared_ptr to a contiguous buffer
    template<typename T>
    using shared_vector = std::shared_ptr<vector_type<T>>;

    /// The type of the object holding all the shared buffers
    using buffer_type = std::tuple<shared_vector<Fields>...>;

public:
    /// The type of a counting number
    using size_type = std::size_t;

    /// Helper for getting the I -th field's type
    template<size_type FieldI>
    using value_type = std::tuple_element_t<FieldI, std::tuple<Fields...>>;

    /// Helper for getting the the I-th field's type as a reference
    template<size_type FieldI>
    using reference = value_type<FieldI>&;

    /// Helper for getting the I-th field's type as a const reference
    template<size_type FieldI>
    using const_reference = const value_type<FieldI>&;

    /// Helper for getting the I-th field's type as a pointer
    template<size_type FieldI>
    using pointer = value_type<FieldI>*;

    /// Helper for getting the I-th field's type as a const pointer
    template<size_type FieldI>
    using const_pointer = const value_type<FieldI>*;

    /// The total number of fields
    constexpr static size_type n_fields = sizeof...(Fields);

    /**
     * @brief Creates an empty AoSElement instance
     *
     * The resulting class has memory allocated to hold an std::vector instance
     * for each field.  Said vectors, however, have no elements in them.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        space for the buffers.  Strong throw guarantee.
     */
    AoSElement() : AoSElement(allocate<0>()) {}

    /**
     * @defgroup Copy/Move CTors and Assignment Operators
     *
     * @brief CTors and operators for deep-copying or taking ownership of
     * another AoSElement instance.
     *
     * The biggest gotcha with this class is that the copy CTor is a deep copy.
     * What this means is that deep copying an AoSElement will sever any link it
     * has with the corresponding AoSFacade.  This is to prevent const issues
     * from arising.
     *
     * @param[in] rhs The instance to copy/take ownership of.  If @p rhs is
     *            moved, after the move it will be in a valid, but otherwise
     *            undefined state.
     * @return Assignment operators return the same instance now containing
     *         a the state of @p rhs.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy @p rhs.
     *         Strong throw guarantee.  Move Ctor/assignment are no throw
     *         guarantee.
     */
    ///@{
    AoSElement(const AoSElement& rhs) : AoSElement() { copy_impl<0>(rhs); }
    AoSElement(AoSElement&& rhs) = default;
    AoSElement& operator         =(const AoSElement& rhs) {
        return *this = std::move(AoSElement(rhs));
    }
    AoSElement& operator=(AoSElement&& rhs) = default;
    ///@}

    /// Default dtor
    ~AoSElement() = default;

    /**
     * @brief Creates a new AoSElement using an existing buffer
     *
     * This Ctor is the one used by the AoSFacade class to ensure that the
     * elements are using a common buffer.  It is assumed that the value of this
     * element will be appended on to the end of the buffer by subsequent
     * `add_value` calls.
     *
     * @param buffer The buffer to put this element's state in.  Shared pointers
     *        are assumed allocated already.
     *
     * @throw None No throw guarantee.
     */
    explicit AoSElement(buffer_type buffer) noexcept :
      me_(get_size<0>(buffer)), first_not_me_(me_), buffer_(buffer) {}

    /**
     * @brief Adds a value to a particular field.
     *
     * Since the data is ultimately stored contiguously the values for each
     * element must be together.  The easiest way to ensure that is if we assume
     * that the current element is the last element and we are thus adding to
     * the end of the array.  This function assumes this and will throw if that
     * is not the case.
     *
     * @tparam FieldI The number of the field to add the value to. Should be in
     *         the range [0, n_fields).
     *
     * @param value The value to add to field.
     * @throw std::logic_error if the last value in the buffer are not
     *        associated with this field.  Strong throw guarantee.
     * @throw std::bad_alloc if there is insufficient memory to add the
     *        additional value to the buffer.  Strong throw guarantee.
     */
    template<size_type FieldI>
    void insert(const value_type<FieldI>& value) {
        auto pfield = std::get<FieldI>(buffer_);
        if(pfield->size() != first_not_me_[FieldI])
            throw std::logic_error("I don't own all elements to end of buffer");
        pfield->push_back(value);
        ++first_not_me_[FieldI];
    }

    /**
     * @brief Returns the number of values associated with @p FieldI
     * @tparam FieldI The number of the field we want the size of.
     * @return The size of the requested field
     * @throw None no throw guarantee.
     */
    template<size_type FieldI>
    size_type size() const noexcept {
        return first_not_me_[FieldI] - me_[FieldI];
    }

    /**
     * @defgroup Accessors for getting the value of a field
     *
     * The at/get accessors return the requested value by reference/pointer.
     * The resulting reference/pointer is both readable and writable if the
     * current instance is non-const and only readable if the instance is const.
     * Either way, the result is guaranteed to be contiguous so that the
     * following will always compile and work as expected:
     *
     * ```.cpp
     * AoSElement<double> ei;
     * //Fill ei
     * double* pfield = ei.get<0>();
     * for(size_type i = 0; i < ei.size<0>(); ++i)
     *     assert(pfield[i] == ei.at<0>(i));
     * ```
     *
     * @warning Functions in this section perform no bounds checks.
     *
     * @tparam FieldI The number of the field we are accessing.  Should be in
     *         the range [0, n_fields).
     * @param[in] i The component of the field.  Should be in the range
     *           [0, size<FieldI>())
     *
     * @throw None no throw guarantee.
     */
    ///@{
    template<size_type FieldI>
    reference<FieldI> at(size_type i = 0) noexcept {
        return std::get<FieldI>(buffer_)->operator[](std::get<FieldI>(me_) + i);
    }

    template<size_type FieldI>
    const_reference<FieldI> at(size_type i = 0) const noexcept {
        return const_cast<my_type&>(*this).at<FieldI>(i);
    }

    template<size_type FieldI>
    pointer<FieldI> get(size_type i = 0) noexcept {
        const size_type me = std::get<FieldI>(me_);
        return &(std::get<FieldI>(buffer_)->operator[](me + i));
    }

    template<size_type FieldI>
    const_pointer<FieldI> get(size_type i = 0) const noexcept {
        return const_cast<my_type&>(*this).get<FieldI>(i);
    }
    ///@}
private:
    /// Allows the facade to get the buffer and use the private typedefs
    friend class AoSFacade<my_type>;

    /// Type of an array holding a number for each field
    using size_array = std::array<size_type, n_fields>;

    /// Helper for getting the type of a vector filled with the I-th field's
    /// type
    template<size_type FieldI>
    using vector_typeI = vector_type<value_type<FieldI>>;

    /// Recursive function for allocating the tuple
    template<size_type I>
    buffer_type allocate() {
        if constexpr(I == n_fields)
            return buffer_type{};
        else {
            auto rv         = allocate<I + 1>();
            std::get<I>(rv) = std::make_shared<vector_typeI<I>>();
            return rv;
        }
    }

    /// Recursive function for getting the size of each buffer
    template<size_type I>
    size_array get_size(const buffer_type& b) const noexcept {
        if constexpr(I == n_fields)
            return size_array{};
        else {
            auto rv         = get_size<I + 1>(b);
            std::get<I>(rv) = std::get<I>(b)->size();
            return rv;
        }
    }

    /// Recursive function for deep-copying each element of the tuple
    template<size_type I>
    void copy_impl(const my_type& value) {
        if constexpr(I != n_fields) {
            for(size_type i = 0; i < value.size<I>(); ++i)
                insert<I>(value.at<I>(i));
            copy_impl<I + 1>(value);
        }
    }

    /// Stores the index each field begins at
    size_array me_ = {};

    /// Stores the index just past where each field ends
    size_array first_not_me_ = {};

    /// All of the buffers
    buffer_type buffer_;
};

/// Primary template, only instantiated when T is an AoSElement
template<typename T>
class AoSFacade;

/**
 * @brief Specialization of AoSFacade to an array of AoSElements.
 *
 * The AoSFacade class implements the guts of a class, C, that wants to behave
 * like an AoS, but have the members of the structures be contiguous.  More
 * specifically, the AoSFacade holds an array of AoSElements which can be used
 * as the state of the structures that are to be returned from C.
 *
 * @tparam Fields The types of the fields stored within the elements.
 */
template<typename... Fields>
class AoSFacade<AoSElement<Fields...>> {
private:
    /// The type of this container
    using my_type = AoSFacade<AoSElement<Fields...>>;

public:
    /// The type of the elements stored in this container
    using value_type = AoSElement<Fields...>;

    /// The type of a reference to a value
    using reference = value_type&;

    /// The type of a const reference to a value
    using const_reference = const value_type&;

    /// The type of a shared_ptr to a value
    using pointer = std::shared_ptr<value_type>;

    /// The type of a const shared_ptr to a value
    using const_pointer = std::shared_ptr<const value_type>;

    /// The type of a number used for counting
    using size_type = typename value_type::size_type;

    using iterator = typename std::vector<pointer>::iterator;

    /**
     * @brief Makes an empty AoSFacade
     *
     * The resulting AoSFacade instance is ready to have elements inserted into
     * it.  This requires allocating the shared pointers that will store the
     * vectors.  Aside from this no additional memory is allocated.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        buffers.  Strong throw guarantee.
     */
    AoSFacade() : buffer_(value_type().buffer_) {}

    /**
     * @defgroup Copy/Move CTors and Assignment Operators
     *
     * @brief CTors and assignment operators for copying/taking over the state
     * of another AoSFacade instance.
     *
     * Copies of AoSFacade instances are deep copies.
     *
     * @param[in] rhs The instance to copy/move.  If @p rhs is moved from it is
     *            in a valid, but otherwise undefined state.
     *
     * @return Assignment operators return the current instance containing @p
     *         rhs's state
     *
     * @throw std::bad_alloc if there is insufficient memory to copy @p rhs.
     *        Strong throw guarantee.  Move assignment is no throw guarantee.
     */
    ///@{
    AoSFacade(const my_type& rhs) : AoSFacade() {
        for(const auto& ei : rhs.elements_) insert(*ei);
    }
    AoSFacade(my_type&& rhs) = default;
    AoSFacade& operator      =(const my_type& rhs) {
        return *this = std::move(my_type(rhs));
    }
    AoSFacade& operator=(my_type&& rhs) = default;
    ///@}

    /// Default dtor
    ~AoSFacade() = default;

    /**
     * @brief Used to add elements to the array.
     *
     *
     * @param value The value to add.
     * @throw std::bad_alloc if there is insufficient memory to copy the element
     *        or to add it to the buffer.  Strong throw guarantee.
     */
    iterator insert(const value_type& value) {
        value_type copy(buffer_);
        copy.template copy_impl<0>(value);
        elements_.push_back(std::make_shared<value_type>(std::move(copy)));
        return elements_.end() - 1;
    }

    /**
     * @brief Returns the number of elements in the current array.
     *
     * @return The number of elements in the array
     * @throw None No throw guarantee.
     */
    size_type size() const noexcept { return elements_.size(); }

    /**
     * @defgroup Accessors
     *
     * @brief Allow accessing of the elements contained within the array
     *
     * These accessors allow retrieval of read/write elements if the current
     * instance is non-const and read-only elements if the current instance is
     * const.
     *
     * @param[in] i The number of the element to retrieve.  Should be in the
     *            range [0, size()).
     *
     * @return The requested element.
     *
     * @warning These operators do not perform bounds checking.
     * @throw None. No throw guarantee.
     */
    ///@{
    reference at(size_type i) noexcept { return *get(i); }
    pointer get(size_type i) noexcept { return elements_[i]; }

    const_reference at(size_type i) const noexcept { return *get(i); }
    const_pointer get(size_type i) const noexcept {
        return const_cast<my_type&>(*this).get(i);
    }
    ///@}

private:
    /// The type of the buffer
    using buffer_type = typename value_type::buffer_type;

    /// The actual elements of the array with all pointers set-up already
    std::vector<std::shared_ptr<value_type>> elements_;

    /// A copy of the buffer to forward to additional elements
    buffer_type buffer_;
};

} // namespace chemist::detail_

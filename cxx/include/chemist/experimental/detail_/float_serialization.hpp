/*
 * Copyright 2026 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <cereal/types/string.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/types/floating_point.hpp>
#include <optional>
#include <stdexcept>
#include <string>

/** @file float_serialization.hpp
 *
 *  Implements cereal round-tripping of a type-erased wtf::fp::Float.
 *
 *  A Float does not know at compile time which concrete floating-point type
 *  it is holding, so an archive can not simply be handed the value; the
 *  concrete type has to be recorded alongside it and restored on the way back
 *  in. The archive format is therefore:
 *
 *  1. A std::string naming the concrete type. The name comes from
 *     wtf::type_traits::type_name_v, which is standardized by WTF and is thus
 *     the same across compilers (unlike typeid(T).name()).
 *  2. The value itself, serialized as that concrete type.
 *
 *  @note Serializing the Sigma uncertainty-quantification types is not
 *        supported. Sigma's types have no cereal support of their own, and
 *        round-tripping them through a center/radius pair would silently
 *        discard the correlation information that makes them useful. Trying
 *        to serialize one throws instead of quietly losing data.
 */

namespace chemist::experimental::detail_ {

/// The concrete floating-point types serialization knows how to restore.
using serialization_fp_types = chemist::types::floating_point_types;

/** @brief Writes the concrete type of @p value, then @p value itself.
 *
 *  @tparam Archive The type of the cereal output archive.
 *
 *  @param[in,out] ar The archive to write to.
 *  @param[in] value The type-erased value to serialize.
 *
 *  @throw std::runtime_error if @p value is holding a Sigma
 *                            uncertainty-quantification type, which can not
 *                            be serialized. Weak throw guarantee: @p ar may
 *                            already have had the type name written to it.
 */
template<typename Archive>
void save_float(Archive& ar, const_float_reference value) {
    auto visitor = [&ar](const auto& v) {
        using value_type = std::decay_t<decltype(v)>;
        ar(std::string(wtf::type_traits::type_name_v<value_type>));
        if constexpr(tensorwrapper::types::is_uq_type_v<value_type>) {
            throw std::runtime_error(
              "chemist::experimental: serializing uncertainty-quantification "
              "floating-point types is not supported.");
        } else {
            ar(v);
        }
    };
    wtf::fp::visit_float_view<serialization_fp_types>(visitor, value);
}

namespace detail_impl_ {

/** @brief Deserializes @p ar as a @p T if @p name names @p T.
 *
 *  Helper for load_float. Does nothing if @p rv has already been filled in or
 *  if @p name does not name @p T, which lets the caller simply fold this over
 *  every type it knows about.
 */
template<typename T, typename Archive>
void load_one(Archive& ar, const std::string& name,
              std::optional<float_type>& rv) {
    if(rv.has_value()) return;
    if(name != std::string(wtf::type_traits::type_name_v<T>)) return;
    if constexpr(tensorwrapper::types::is_uq_type_v<T>) {
        throw std::runtime_error(
          "chemist::experimental: deserializing uncertainty-quantification "
          "floating-point types is not supported.");
    } else {
        T value{};
        ar(value);
        rv.emplace(value);
    }
}

/// Folds load_one over every type in the tuple @p TupleType names.
template<typename Archive, typename... Ts>
void load_each(Archive& ar, const std::string& name,
               std::optional<float_type>& rv, const std::tuple<Ts...>*) {
    (load_one<Ts>(ar, name, rv), ...);
}

} // namespace detail_impl_

/** @brief Reads back a value written by save_float.
 *
 *  @tparam Archive The type of the cereal input archive.
 *
 *  @param[in,out] ar The archive to read from.
 *
 *  @return A Float holding the value, restored to the concrete type it was
 *          saved as.
 *
 *  @throw std::runtime_error if the archive names a type chemist does not
 *                            know about, or one which can not be serialized.
 *                            Weak throw guarantee.
 */
template<typename Archive>
float_type load_float(Archive& ar) {
    std::string name;
    ar(name);

    std::optional<float_type> rv;
    const serialization_fp_types* tag = nullptr;
    detail_impl_::load_each(ar, name, rv, tag);

    if(!rv.has_value()) {
        throw std::runtime_error(
          "chemist::experimental: archive names an unknown floating-point "
          "type: " +
          name);
    }
    return std::move(*rv);
}

} // namespace chemist::experimental::detail_

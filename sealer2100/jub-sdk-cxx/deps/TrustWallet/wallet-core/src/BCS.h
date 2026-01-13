// Copyright © 2017-2023 Trust Wallet.
// Created by Clément Doumergue

// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <tuple>
#include <vector>
#include <type_traits>

#include "Data.h"

namespace TW::BCS {

/// Implementation of BCS encoding (as specified by the Diem project, see github.com/diem/bcs#detailed-specifications)

struct Serializer {
    Data bytes;

    void add_byte(uint8_t b) noexcept {
        bytes.push_back(b);
    }

    template <typename Iterator>
    void add_bytes(Iterator first, Iterator last) noexcept {
        std::transform(first, last, std::back_inserter(bytes), [](auto&& c) {
            return static_cast<uint8_t>(c);
        });
    }

    void clear() noexcept {
        bytes.clear();
    }
};

struct uleb128 {
    uint32_t value;
};

namespace details {

// Type traits to check if a type is serializable
template <typename T>
struct is_serializable {
    static constexpr bool value = false;
};

template <>
struct is_serializable<uint8_t> {
    static constexpr bool value = true;
};

template <>
struct is_serializable<uint32_t> {
    static constexpr bool value = true;
};

template <>
struct is_serializable<uint64_t> {
    static constexpr bool value = true;
};

template <>
struct is_serializable<std::string> {
    static constexpr bool value = true;
};

template <>
struct is_serializable<uleb128> {
    static constexpr bool value = true;
};

template <typename T>
struct is_serializable<std::vector<T>> {
    static constexpr bool value = is_serializable<T>::value;
};

template <typename T, typename U>
struct is_serializable<std::pair<T, U>> {
    static constexpr bool value = is_serializable<T>::value && is_serializable<U>::value;
};

template <typename... Ts>
struct is_serializable<std::tuple<Ts...>> {
    static constexpr bool value = true; // Assume all tuple elements are serializable
};

// Helper to serialize integral types
template <typename T, std::size_t... Is>
Serializer& serialize_integral_impl(Serializer& stream, T t, std::index_sequence<Is...>) noexcept {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&t);
    // Add each byte in little-endian order
    int dummy[] = { (stream.add_byte(bytes[Is]), 0)... };
    (void)dummy; // Silence unused variable warning
    return stream;
}

// Helper to serialize tuples
template <typename T, std::size_t... Is>
Serializer& serialize_tuple_impl(Serializer& stream, const T& t, std::index_sequence<Is...>) noexcept {
    int dummy[] = { (stream << std::get<Is>(t), 0)... };
    (void)dummy; // Silence unused variable warning
    return stream;
}

} // namespace details

// Serialization operators
Serializer& operator<<(Serializer& stream, uint8_t b) noexcept;

//template <typename T>
//Serializer& operator<<(Serializer& stream, T t) noexcept {
//    static_assert(details::is_serializable<T>::value, "Type is not serializable");
//    return details::serialize_integral_impl(stream, t, std::make_index_sequence<sizeof(T)>{});
//}

Serializer& operator<<(Serializer& stream, uleb128 t) noexcept;

Serializer& operator<<(Serializer& stream, const std::string& str) noexcept;

template <typename T>
Serializer& operator<<(Serializer& stream, const std::vector<T>& vec) noexcept {
    stream << uleb128{static_cast<uint32_t>(vec.size())};
    for (const auto& item : vec) {
        stream << item;
    }
    return stream;
}

template <typename T, typename U>
Serializer& operator<<(Serializer& stream, const std::pair<T, U>& pair) noexcept {
    return stream << std::make_tuple(pair.first, pair.second);
}

template <typename... Ts>
Serializer& operator<<(Serializer& stream, const std::tuple<Ts...>& t) noexcept {
    return details::serialize_tuple_impl(stream, t, std::make_index_sequence<sizeof...(Ts)>{});
}

} // namespace TW::BCS

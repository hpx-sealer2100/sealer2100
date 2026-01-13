// Copyright © 2017-2023 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <type_traits>

namespace TW {

// 使用 std::enable_if 实现 integral 约束
template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
using integral = T;

// 使用 std::enable_if 实现 floating_point 约束
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
using floating_point = T;

// 或者使用 static_assert 实现约束
template <typename T>
void check_integral() {
    static_assert(std::is_integral<T>::value, "T must be an integral type");
}

template <typename T>
void check_floating_point() {
    static_assert(std::is_floating_point<T>::value, "T must be a floating-point type");
}

} // namespace TW
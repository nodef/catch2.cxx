
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
/** \file
 * This is a convenience header for Catch2's Matcher support. It includes
 * **all** of Catch2 headers related to matchers.
 *
 * Generally the Catch2 users should use specific includes they need,
 * but this header can be used instead for ease-of-experimentation, or
 * just plain convenience, at the cost of increased compilation times.
 *
 * When a new header is added to either the `matchers` folder, or to
 * the corresponding internal subfolder, it should be added here.
 */

#ifndef CATCH_MATCHERS_ALL_HPP_INCLUDED
#define CATCH_MATCHERS_ALL_HPP_INCLUDED

#include "catch_matchers.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_container_properties.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_contains.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_exception.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_floating_point.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_predicate.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_quantifiers.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_range_equals.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_string.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_templated.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_matchers_vector.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_matchers_impl.hpp"  // Adjust to relative path (@wolfram77)

#endif // CATCH_MATCHERS_ALL_HPP_INCLUDED

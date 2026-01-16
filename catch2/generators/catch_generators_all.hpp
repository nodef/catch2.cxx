
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
/** \file
 * This is a convenience header for Catch2's Generator support. It includes
 * **all** of Catch2 headers related to generators.
 *
 * Generally the Catch2 users should use specific includes they need,
 * but this header can be used instead for ease-of-experimentation, or
 * just plain convenience, at the cost of (significantly) increased
 * compilation times.
 *
 * When a new header is added to either the `generators` folder,
 * or to the corresponding internal subfolder, it should be added here.
 */

#ifndef CATCH_GENERATORS_ALL_HPP_INCLUDED
#define CATCH_GENERATORS_ALL_HPP_INCLUDED

#include "catch_generator_exception.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_generators.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_generators_adapters.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_generators_random.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_generators_range.hpp"  // Adjust to relative path (@wolfram77)

#endif // CATCH_GENERATORS_ALL_HPP_INCLUDED

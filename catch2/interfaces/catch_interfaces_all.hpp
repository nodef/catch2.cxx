
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
/** \file
 * This is a convenience header for Catch2's interfaces. It includes
 * **all** of Catch2 headers related to interfaces.
 *
 * Generally the Catch2 users should use specific includes they need,
 * but this header can be used instead for ease-of-experimentation, or
 * just plain convenience, at the cost of somewhat increased compilation
 * times.
 *
 * When a new header is added to either the `interfaces` folder, or to
 * the corresponding internal subfolder, it should be added here.
 */


#ifndef CATCH_INTERFACES_ALL_HPP_INCLUDED
#define CATCH_INTERFACES_ALL_HPP_INCLUDED

#include "catch_interfaces_capture.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_config.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_enum_values_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_exception.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_generatortracker.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_reporter.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_reporter_factory.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_tag_alias_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_test_invoker.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_interfaces_testcase.hpp"  // Adjust to relative path (@wolfram77)

#endif // CATCH_INTERFACES_ALL_HPP_INCLUDED


//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
/** \file
 * This is a convenience header for Catch2's Reporter support. It includes
 * **all** of Catch2 headers related to reporters, including all reporters.
 *
 * Generally the Catch2 users should use specific includes they need,
 * but this header can be used instead for ease-of-experimentation, or
 * just plain convenience, at the cost of (significantly) increased
 * compilation times.
 *
 * When a new header (reporter) is added to either the `reporter` folder,
 * or to the corresponding internal subfolder, it should be added here.
 */

#ifndef CATCH_REPORTERS_ALL_HPP_INCLUDED
#define CATCH_REPORTERS_ALL_HPP_INCLUDED

#include "catch_reporter_automake.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_common_base.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_compact.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_console.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_cumulative_base.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_event_listener.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_helpers.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_json.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_junit.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_multi.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_registrars.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_sonarqube.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_streaming_base.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_tap.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_teamcity.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_xml.hpp"  // Adjust to relative path (@wolfram77)

#endif // CATCH_REPORTERS_ALL_HPP_INCLUDED

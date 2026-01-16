
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include "catch_get_random_seed.hpp"  // Adjust to relative path (@wolfram77)

#include "internal/catch_context.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_config.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
    std::uint32_t getSeed() {
        return getCurrentContext().getConfig()->rngSeed();
    }
}

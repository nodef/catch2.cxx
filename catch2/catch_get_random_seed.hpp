//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_GET_RANDOM_SEED_HPP_INCLUDED
#define CATCH_GET_RANDOM_SEED_HPP_INCLUDED

#include <cstdint>

namespace Catch {
    //! Returns Catch2's current RNG seed.
    std::uint32_t getSeed();
}

#endif // CATCH_GET_RANDOM_SEED_HPP_INCLUDED



// BEGIN Amalgamated content from catch_get_random_seed.cpp (@wolfram77)
#ifndef CATCH_GET_RANDOM_SEED_CPP_INCLUDED
#define CATCH_GET_RANDOM_SEED_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_get_random_seed.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include "internal/catch_context.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_config.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
    std::uint32_t getSeed() {
        return getCurrentContext().getConfig()->rngSeed();
    }
}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_GET_RANDOM_SEED_CPP_INCLUDED
// END Amalgamated content from catch_get_random_seed.cpp (@wolfram77)

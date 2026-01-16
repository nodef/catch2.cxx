//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_RANDOM_SEED_GENERATION_HPP_INCLUDED
#define CATCH_RANDOM_SEED_GENERATION_HPP_INCLUDED

#include <cstdint>

namespace Catch {

    enum class GenerateFrom {
        Time,
        RandomDevice,
        //! Currently equivalent to RandomDevice, but can change at any point
        Default
    };

    std::uint32_t generateRandomSeed(GenerateFrom from);

} // end namespace Catch

#endif // CATCH_RANDOM_SEED_GENERATION_HPP_INCLUDED



// BEGIN Amalgamated content from catch_random_seed_generation.cpp (@wolfram77)
#ifndef CATCH_RANDOM_SEED_GENERATION_CPP_INCLUDED
#define CATCH_RANDOM_SEED_GENERATION_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_random_seed_generation.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_random_integer_helpers.hpp"  // Adjust to relative path (@wolfram77)

#include <ctime>
#include <random>

namespace Catch {

    std::uint32_t generateRandomSeed( GenerateFrom from ) {
        switch ( from ) {
        case GenerateFrom::Time:
            return static_cast<std::uint32_t>( std::time( nullptr ) );

        case GenerateFrom::Default:
        case GenerateFrom::RandomDevice: {
            std::random_device rd;
            return Detail::fillBitsFrom<std::uint32_t>( rd );
        }

        default:
            CATCH_ERROR("Unknown generation method");
        }
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_RANDOM_SEED_GENERATION_CPP_INCLUDED
// END Amalgamated content from catch_random_seed_generation.cpp (@wolfram77)

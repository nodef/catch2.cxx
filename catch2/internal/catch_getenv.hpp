//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_GETENV_HPP_INCLUDED
#define CATCH_GETENV_HPP_INCLUDED

namespace Catch {
namespace Detail {

    //! Wrapper over `std::getenv` that compiles on UWP (and always returns nullptr there)
    char const* getEnv(char const* varName);

}
}

#endif // CATCH_GETENV_HPP_INCLUDED



// BEGIN Amalgamated content from catch_getenv.cpp (@wolfram77)
#ifndef CATCH_GETENV_CPP_INCLUDED
#define CATCH_GETENV_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_getenv.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include "catch_platform.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)

#include <cstdlib>

namespace Catch {
    namespace Detail {

#if !defined (CATCH_CONFIG_GETENV)
        char const* getEnv( char const* ) { return nullptr; }
#else

        char const* getEnv( char const* varName ) {
#    if defined( _MSC_VER )
#        pragma warning( push )
#        pragma warning( disable : 4996 ) // use getenv_s instead of getenv
#    endif

            return std::getenv( varName );

#    if defined( _MSC_VER )
#        pragma warning( pop )
#    endif
        }
#endif
} // namespace Detail
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_GETENV_CPP_INCLUDED
// END Amalgamated content from catch_getenv.cpp (@wolfram77)

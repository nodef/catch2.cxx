//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_UNCAUGHT_EXCEPTIONS_HPP_INCLUDED
#define CATCH_UNCAUGHT_EXCEPTIONS_HPP_INCLUDED

namespace Catch {
    bool uncaught_exceptions();
} // end namespace Catch

#endif // CATCH_UNCAUGHT_EXCEPTIONS_HPP_INCLUDED



// BEGIN Amalgamated content from catch_uncaught_exceptions.cpp (@wolfram77)
#ifndef CATCH_UNCAUGHT_EXCEPTIONS_CPP_INCLUDED
#define CATCH_UNCAUGHT_EXCEPTIONS_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_uncaught_exceptions.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_config_uncaught_exceptions.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_user_config.hpp"  // Adjust to relative path (@wolfram77)

#include <exception>

namespace Catch {
    bool uncaught_exceptions() {
#if defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
        return false;
#elif defined(CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS)
        return std::uncaught_exceptions() > 0;
#else
        return std::uncaught_exception();
#endif
  }
} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_UNCAUGHT_EXCEPTIONS_CPP_INCLUDED
// END Amalgamated content from catch_uncaught_exceptions.cpp (@wolfram77)

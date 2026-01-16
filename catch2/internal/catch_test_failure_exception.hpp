//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEST_FAILURE_EXCEPTION_HPP_INCLUDED
#define CATCH_TEST_FAILURE_EXCEPTION_HPP_INCLUDED

namespace Catch {

    //! Used to signal that an assertion macro failed
    struct TestFailureException{};
    //! Used to signal that the remainder of a test should be skipped
    struct TestSkipException {};

    /**
     * Outlines throwing of `TestFailureException` into a single TU
     *
     * Also handles `CATCH_CONFIG_DISABLE_EXCEPTIONS` for callers.
     */
    [[noreturn]] void throw_test_failure_exception();

    /**
     * Outlines throwing of `TestSkipException` into a single TU
     *
     * Also handles `CATCH_CONFIG_DISABLE_EXCEPTIONS` for callers.
     */
    [[noreturn]] void throw_test_skip_exception();

} // namespace Catch

#endif // CATCH_TEST_FAILURE_EXCEPTION_HPP_INCLUDED



// BEGIN Amalgamated content from catch_test_failure_exception.cpp (@wolfram77)
#ifndef CATCH_TEST_FAILURE_EXCEPTION_CPP_INCLUDED
#define CATCH_TEST_FAILURE_EXCEPTION_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_test_failure_exception.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_user_config.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    void throw_test_failure_exception() {
#if !defined( CATCH_CONFIG_DISABLE_EXCEPTIONS )
        throw TestFailureException{};
#else
        CATCH_ERROR( "Test failure requires aborting test!" );
#endif
    }

    void throw_test_skip_exception() {
#if !defined( CATCH_CONFIG_DISABLE_EXCEPTIONS )
        throw Catch::TestSkipException();
#else
        CATCH_ERROR( "Explicitly skipping tests during runtime requires exceptions" );
#endif
    }

} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TEST_FAILURE_EXCEPTION_CPP_INCLUDED
// END Amalgamated content from catch_test_failure_exception.cpp (@wolfram77)

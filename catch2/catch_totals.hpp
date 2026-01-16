//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TOTALS_HPP_INCLUDED
#define CATCH_TOTALS_HPP_INCLUDED

#include <cstdint>

namespace Catch {

    struct Counts {
        Counts operator - ( Counts const& other ) const;
        Counts& operator += ( Counts const& other );

        std::uint64_t total() const;
        bool allPassed() const;
        bool allOk() const;

        std::uint64_t passed = 0;
        std::uint64_t failed = 0;
        std::uint64_t failedButOk = 0;
        std::uint64_t skipped = 0;
    };

    struct Totals {

        Totals operator - ( Totals const& other ) const;
        Totals& operator += ( Totals const& other );

        Totals delta( Totals const& prevTotals ) const;

        Counts assertions;
        Counts testCases;
    };
}

#endif // CATCH_TOTALS_HPP_INCLUDED



// BEGIN Amalgamated content from catch_totals.cpp (@wolfram77)
#ifndef CATCH_TOTALS_CPP_INCLUDED
#define CATCH_TOTALS_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_totals.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

namespace Catch {

    Counts Counts::operator - ( Counts const& other ) const {
        Counts diff;
        diff.passed = passed - other.passed;
        diff.failed = failed - other.failed;
        diff.failedButOk = failedButOk - other.failedButOk;
        diff.skipped = skipped - other.skipped;
        return diff;
    }

    Counts& Counts::operator += ( Counts const& other ) {
        passed += other.passed;
        failed += other.failed;
        failedButOk += other.failedButOk;
        skipped += other.skipped;
        return *this;
    }

    std::uint64_t Counts::total() const {
        return passed + failed + failedButOk + skipped;
    }
    bool Counts::allPassed() const {
        return failed == 0 && failedButOk == 0 && skipped == 0;
    }
    bool Counts::allOk() const {
        return failed == 0;
    }

    Totals Totals::operator - ( Totals const& other ) const {
        Totals diff;
        diff.assertions = assertions - other.assertions;
        diff.testCases = testCases - other.testCases;
        return diff;
    }

    Totals& Totals::operator += ( Totals const& other ) {
        assertions += other.assertions;
        testCases += other.testCases;
        return *this;
    }

    Totals Totals::delta( Totals const& prevTotals ) const {
        Totals diff = *this - prevTotals;
        if( diff.assertions.failed > 0 )
            ++diff.testCases.failed;
        else if( diff.assertions.failedButOk > 0 )
            ++diff.testCases.failedButOk;
        else if ( diff.assertions.skipped > 0 )
            ++ diff.testCases.skipped;
        else
            ++diff.testCases.passed;
        return diff;
    }

}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TOTALS_CPP_INCLUDED
// END Amalgamated content from catch_totals.cpp (@wolfram77)

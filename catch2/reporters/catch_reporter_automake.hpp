//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_AUTOMAKE_HPP_INCLUDED
#define CATCH_REPORTER_AUTOMAKE_HPP_INCLUDED

#include "catch_reporter_streaming_base.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch {

    class AutomakeReporter final : public StreamingReporterBase {
    public:
        // GCC5 compat: we cannot use inherited constructor, because it
        //              doesn't implement backport of P0136
        AutomakeReporter( ReporterConfig&& _config ):
            StreamingReporterBase( CATCH_MOVE( _config ) ) {
            m_preferences.shouldReportAllAssertionStarts = false;
        }

        ~AutomakeReporter() override;

        static std::string getDescription() {
            using namespace std::string_literals;
            return "Reports test results in the format of Automake .trs files"s;
        }

        void testCaseEnded(TestCaseStats const& _testCaseStats) override;
        void skipTest(TestCaseInfo const& testInfo) override;
    };

} // end namespace Catch

#endif // CATCH_REPORTER_AUTOMAKE_HPP_INCLUDED



// BEGIN Amalgamated content from catch_reporter_automake.cpp (@wolfram77)
#ifndef CATCH_REPORTER_AUTOMAKE_CPP_INCLUDED
#define CATCH_REPORTER_AUTOMAKE_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_reporter_automake.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "../catch_test_case_info.hpp"  // Adjust to relative path (@wolfram77)

#include <ostream>

namespace Catch {

    AutomakeReporter::~AutomakeReporter() = default;

    void AutomakeReporter::testCaseEnded(TestCaseStats const& _testCaseStats) {
        // Possible values to emit are PASS, XFAIL, SKIP, FAIL, XPASS and ERROR.
        m_stream << ":test-result: ";
        if ( _testCaseStats.totals.testCases.skipped > 0 ) {
            m_stream << "SKIP";
        } else if (_testCaseStats.totals.assertions.allPassed()) {
            m_stream << "PASS";
        } else if (_testCaseStats.totals.assertions.allOk()) {
            m_stream << "XFAIL";
        } else {
            m_stream << "FAIL";
        }
        m_stream << ' ' << _testCaseStats.testInfo->name << '\n';
        StreamingReporterBase::testCaseEnded(_testCaseStats);
    }

    void AutomakeReporter::skipTest(TestCaseInfo const& testInfo) {
        m_stream << ":test-result: SKIP " << testInfo.name << '\n';
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_REPORTER_AUTOMAKE_CPP_INCLUDED
// END Amalgamated content from catch_reporter_automake.cpp (@wolfram77)

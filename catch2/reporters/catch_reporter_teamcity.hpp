//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_TEAMCITY_HPP_INCLUDED
#define CATCH_REPORTER_TEAMCITY_HPP_INCLUDED

#include "catch_reporter_streaming_base.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_timer.hpp"  // Adjust to relative path (@wolfram77)

#include <cstring>

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    class TeamCityReporter final : public StreamingReporterBase {
    public:
        TeamCityReporter( ReporterConfig&& _config )
        :   StreamingReporterBase( CATCH_MOVE(_config) )
        {
            m_preferences.shouldRedirectStdOut = true;
            m_preferences.shouldReportAllAssertionStarts = false;
        }

        ~TeamCityReporter() override;

        static std::string getDescription() {
            using namespace std::string_literals;
            return "Reports test results as TeamCity service messages"s;
        }

        void testRunStarting( TestRunInfo const& runInfo ) override;
        void testRunEnded( TestRunStats const& runStats ) override;


        void assertionEnded(AssertionStats const& assertionStats) override;

        void sectionStarting(SectionInfo const& sectionInfo) override {
            m_headerPrintedForThisSection = false;
            StreamingReporterBase::sectionStarting( sectionInfo );
        }

        void testCaseStarting(TestCaseInfo const& testInfo) override;

        void testCaseEnded(TestCaseStats const& testCaseStats) override;

    private:
        void printSectionHeader(std::ostream& os);

        bool m_headerPrintedForThisSection = false;
        Timer m_testTimer;
    };

} // end namespace Catch

#ifdef __clang__
#   pragma clang diagnostic pop
#endif

#endif // CATCH_REPORTER_TEAMCITY_HPP_INCLUDED



// BEGIN Amalgamated content from catch_reporter_teamcity.cpp (@wolfram77)
#ifndef CATCH_REPORTER_TEAMCITY_CPP_INCLUDED
#define CATCH_REPORTER_TEAMCITY_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_reporter_teamcity.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include "catch_reporter_helpers.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_string_manip.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_textflow.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_test_case_info.hpp"  // Adjust to relative path (@wolfram77)

#include <cassert>
#include <ostream>

namespace Catch {

    namespace {
        // if string has a : in first line will set indent to follow it on
        // subsequent lines
        void printHeaderString(std::ostream& os, std::string const& _string, std::size_t indent = 0) {
            std::size_t i = _string.find(": ");
            if (i != std::string::npos)
                i += 2;
            else
                i = 0;
            os << TextFlow::Column(_string)
                  .indent(indent + i)
                  .initialIndent(indent) << '\n';
        }

        std::string escape(StringRef str) {
            std::string escaped = static_cast<std::string>(str);
            replaceInPlace(escaped, "|", "||");
            replaceInPlace(escaped, "'", "|'");
            replaceInPlace(escaped, "\n", "|n");
            replaceInPlace(escaped, "\r", "|r");
            replaceInPlace(escaped, "[", "|[");
            replaceInPlace(escaped, "]", "|]");
            return escaped;
        }
    } // end anonymous namespace


    TeamCityReporter::~TeamCityReporter() = default;

    void TeamCityReporter::testRunStarting( TestRunInfo const& runInfo ) {
        m_stream << "##teamcity[testSuiteStarted name='" << escape( runInfo.name )
               << "']\n";
    }

    void TeamCityReporter::testRunEnded( TestRunStats const& runStats ) {
        m_stream << "##teamcity[testSuiteFinished name='"
               << escape( runStats.runInfo.name ) << "']\n";
    }

    void TeamCityReporter::assertionEnded(AssertionStats const& assertionStats) {
        AssertionResult const& result = assertionStats.assertionResult;
        if ( !result.isOk() ||
             result.getResultType() == ResultWas::ExplicitSkip ) {

            ReusableStringStream msg;
            if (!m_headerPrintedForThisSection)
                printSectionHeader(msg.get());
            m_headerPrintedForThisSection = true;

            msg << result.getSourceInfo() << '\n';

            switch (result.getResultType()) {
            case ResultWas::ExpressionFailed:
                msg << "expression failed";
                break;
            case ResultWas::ThrewException:
                msg << "unexpected exception";
                break;
            case ResultWas::FatalErrorCondition:
                msg << "fatal error condition";
                break;
            case ResultWas::DidntThrowException:
                msg << "no exception was thrown where one was expected";
                break;
            case ResultWas::ExplicitFailure:
                msg << "explicit failure";
                break;
            case ResultWas::ExplicitSkip:
                msg << "explicit skip";
                break;

                // We shouldn't get here because of the isOk() test
            case ResultWas::Ok:
            case ResultWas::Info:
            case ResultWas::Warning:
                CATCH_ERROR("Internal error in TeamCity reporter");
                // These cases are here to prevent compiler warnings
            case ResultWas::Unknown:
            case ResultWas::FailureBit:
            case ResultWas::Exception:
                CATCH_ERROR("Not implemented");
            }
            if (assertionStats.infoMessages.size() == 1)
                msg << " with message:";
            if (assertionStats.infoMessages.size() > 1)
                msg << " with messages:";
            for (auto const& messageInfo : assertionStats.infoMessages)
                msg << "\n  \"" << messageInfo.message << '"';


            if (result.hasExpression()) {
                msg <<
                    "\n  " << result.getExpressionInMacro() << "\n"
                    "with expansion:\n"
                    "  " << result.getExpandedExpression() << '\n';
            }

            if ( result.getResultType() == ResultWas::ExplicitSkip ) {
                m_stream << "##teamcity[testIgnored";
            } else if ( currentTestCaseInfo->okToFail() ) {
                msg << "- failure ignore as test marked as 'ok to fail'\n";
                m_stream << "##teamcity[testIgnored";
            } else {
                m_stream << "##teamcity[testFailed";
            }
            m_stream << " name='" << escape( currentTestCaseInfo->name ) << '\''
                     << " message='" << escape( msg.str() ) << '\'' << "]\n";
        }
        m_stream.flush();
    }

    void TeamCityReporter::testCaseStarting(TestCaseInfo const& testInfo) {
        m_testTimer.start();
        StreamingReporterBase::testCaseStarting(testInfo);
        m_stream << "##teamcity[testStarted name='"
            << escape(testInfo.name) << "']\n";
        m_stream.flush();
    }

    void TeamCityReporter::testCaseEnded(TestCaseStats const& testCaseStats) {
        StreamingReporterBase::testCaseEnded(testCaseStats);
        auto const& testCaseInfo = *testCaseStats.testInfo;
        if (!testCaseStats.stdOut.empty())
            m_stream << "##teamcity[testStdOut name='"
            << escape(testCaseInfo.name)
            << "' out='" << escape(testCaseStats.stdOut) << "']\n";
        if (!testCaseStats.stdErr.empty())
            m_stream << "##teamcity[testStdErr name='"
            << escape(testCaseInfo.name)
            << "' out='" << escape(testCaseStats.stdErr) << "']\n";
        m_stream << "##teamcity[testFinished name='"
            << escape(testCaseInfo.name) << "' duration='"
            << m_testTimer.getElapsedMilliseconds() << "']\n";
        m_stream.flush();
    }

    void TeamCityReporter::printSectionHeader(std::ostream& os) {
        assert(!m_sectionStack.empty());

        if (m_sectionStack.size() > 1) {
            os << lineOfChars('-') << '\n';

            std::vector<SectionInfo>::const_iterator
                it = m_sectionStack.begin() + 1, // Skip first section (test case)
                itEnd = m_sectionStack.end();
            for (; it != itEnd; ++it)
                printHeaderString(os, it->name);
            os << lineOfChars('-') << '\n';
        }

        SourceLineInfo lineInfo = m_sectionStack.front().lineInfo;

        os << lineInfo << '\n';
        os << lineOfChars('.') << "\n\n";
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_REPORTER_TEAMCITY_CPP_INCLUDED
// END Amalgamated content from catch_reporter_teamcity.cpp (@wolfram77)

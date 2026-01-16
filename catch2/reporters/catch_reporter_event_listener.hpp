//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_EVENT_LISTENER_HPP_INCLUDED
#define CATCH_REPORTER_EVENT_LISTENER_HPP_INCLUDED

#include "../interfaces/catch_interfaces_reporter.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    /**
     * Base class to simplify implementing listeners.
     *
     * Provides empty default implementation for all IEventListener member
     * functions, so that a listener implementation can pick which
     * member functions it actually cares about.
     */
    class EventListenerBase : public IEventListener {
    public:
        using IEventListener::IEventListener;

        void reportInvalidTestSpec( StringRef unmatchedSpec ) override;
        void fatalErrorEncountered( StringRef error ) override;

        void benchmarkPreparing( StringRef name ) override;
        void benchmarkStarting( BenchmarkInfo const& benchmarkInfo ) override;
        void benchmarkEnded( BenchmarkStats<> const& benchmarkStats ) override;
        void benchmarkFailed( StringRef error ) override;

        void assertionStarting( AssertionInfo const& assertionInfo ) override;
        void assertionEnded( AssertionStats const& assertionStats ) override;

        void listReporters(
            std::vector<ReporterDescription> const& descriptions ) override;
        void listListeners(
            std::vector<ListenerDescription> const& descriptions ) override;
        void listTests( std::vector<TestCaseHandle> const& tests ) override;
        void listTags( std::vector<TagInfo> const& tagInfos ) override;

        void noMatchingTestCases( StringRef unmatchedSpec ) override;
        void testRunStarting( TestRunInfo const& testRunInfo ) override;
        void testCaseStarting( TestCaseInfo const& testInfo ) override;
        void testCasePartialStarting( TestCaseInfo const& testInfo,
                                      uint64_t partNumber ) override;
        void sectionStarting( SectionInfo const& sectionInfo ) override;
        void sectionEnded( SectionStats const& sectionStats ) override;
        void testCasePartialEnded( TestCaseStats const& testCaseStats,
                                   uint64_t partNumber ) override;
        void testCaseEnded( TestCaseStats const& testCaseStats ) override;
        void testRunEnded( TestRunStats const& testRunStats ) override;
        void skipTest( TestCaseInfo const& testInfo ) override;
    };

} // end namespace Catch

#endif // CATCH_REPORTER_EVENT_LISTENER_HPP_INCLUDED



// BEGIN Amalgamated content from catch_reporter_event_listener.cpp (@wolfram77)
#ifndef CATCH_REPORTER_EVENT_LISTENER_CPP_INCLUDED
#define CATCH_REPORTER_EVENT_LISTENER_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_reporter_event_listener.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

namespace Catch {

    void EventListenerBase::fatalErrorEncountered( StringRef ) {}

    void EventListenerBase::benchmarkPreparing( StringRef ) {}
    void EventListenerBase::benchmarkStarting( BenchmarkInfo const& ) {}
    void EventListenerBase::benchmarkEnded( BenchmarkStats<> const& ) {}
    void EventListenerBase::benchmarkFailed( StringRef ) {}

    void EventListenerBase::assertionStarting( AssertionInfo const& ) {}

    void EventListenerBase::assertionEnded( AssertionStats const& ) {}
    void EventListenerBase::listReporters(
        std::vector<ReporterDescription> const& ) {}
    void EventListenerBase::listListeners(
        std::vector<ListenerDescription> const& ) {}
    void EventListenerBase::listTests( std::vector<TestCaseHandle> const& ) {}
    void EventListenerBase::listTags( std::vector<TagInfo> const& ) {}
    void EventListenerBase::noMatchingTestCases( StringRef ) {}
    void EventListenerBase::reportInvalidTestSpec( StringRef ) {}
    void EventListenerBase::testRunStarting( TestRunInfo const& ) {}
    void EventListenerBase::testCaseStarting( TestCaseInfo const& ) {}
    void EventListenerBase::testCasePartialStarting(TestCaseInfo const&, uint64_t) {}
    void EventListenerBase::sectionStarting( SectionInfo const& ) {}
    void EventListenerBase::sectionEnded( SectionStats const& ) {}
    void EventListenerBase::testCasePartialEnded(TestCaseStats const&, uint64_t) {}
    void EventListenerBase::testCaseEnded( TestCaseStats const& ) {}
    void EventListenerBase::testRunEnded( TestRunStats const& ) {}
    void EventListenerBase::skipTest( TestCaseInfo const& ) {}
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_REPORTER_EVENT_LISTENER_CPP_INCLUDED
// END Amalgamated content from catch_reporter_event_listener.cpp (@wolfram77)

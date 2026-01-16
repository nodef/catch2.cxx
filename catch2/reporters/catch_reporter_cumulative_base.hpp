//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_CUMULATIVE_BASE_HPP_INCLUDED
#define CATCH_REPORTER_CUMULATIVE_BASE_HPP_INCLUDED

#include "catch_reporter_common_base.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_optional.hpp"  // Adjust to relative path (@wolfram77)

#include <string>
#include <vector>

namespace Catch {

    namespace Detail {

        //! Represents either an assertion or a benchmark result to be handled by cumulative reporter later
        class AssertionOrBenchmarkResult {
            // This should really be a variant, but this is much faster
            // to write and the data layout here is already terrible
            // enough that we do not have to care about the object size.
            Optional<AssertionStats> m_assertion;
            Optional<BenchmarkStats<>> m_benchmark;
        public:
            AssertionOrBenchmarkResult(AssertionStats const& assertion);
            AssertionOrBenchmarkResult(BenchmarkStats<> const& benchmark);

            bool isAssertion() const;
            bool isBenchmark() const;

            AssertionStats const& asAssertion() const;
            BenchmarkStats<> const& asBenchmark() const;
        };
    }

    /**
     * Utility base for reporters that need to handle all results at once
     *
     * It stores tree of all test cases, sections and assertions, and after the
     * test run is finished, calls into `testRunEndedCumulative` to pass the
     * control to the deriving class.
     *
     * If you are deriving from this class and override any testing related
     * member functions, you should first call into the base's implementation to
     * avoid breaking the tree construction.
     *
     * Due to the way this base functions, it has to expand assertions up-front,
     * even if they are later unused (e.g. because the deriving reporter does
     * not report successful assertions, or because the deriving reporter does
     * not use assertion expansion at all). Derived classes can use two
     * customization points, `m_shouldStoreSuccesfulAssertions` and
     * `m_shouldStoreFailedAssertions`, to disable the expansion and gain extra
     * performance. **Accessing the assertion expansions if it wasn't stored is
     * UB.**
     */
    class CumulativeReporterBase : public ReporterBase {
    public:
        template<typename T, typename ChildNodeT>
        struct Node {
            explicit Node( T const& _value ) : value( _value ) {}

            using ChildNodes = std::vector<Detail::unique_ptr<ChildNodeT>>;
            T value;
            ChildNodes children;
        };
        struct SectionNode {
            explicit SectionNode(SectionStats const& _stats) : stats(_stats) {}

            bool operator == (SectionNode const& other) const {
                return stats.sectionInfo.lineInfo == other.stats.sectionInfo.lineInfo;
            }

            bool hasAnyAssertions() const;

            SectionStats stats;
            std::vector<Detail::unique_ptr<SectionNode>> childSections;
            std::vector<Detail::AssertionOrBenchmarkResult> assertionsAndBenchmarks;
            std::string stdOut;
            std::string stdErr;
        };


        using TestCaseNode = Node<TestCaseStats, SectionNode>;
        using TestRunNode = Node<TestRunStats, TestCaseNode>;

        // GCC5 compat: we cannot use inherited constructor, because it
        //              doesn't implement backport of P0136
        CumulativeReporterBase(ReporterConfig&& _config):
            ReporterBase(CATCH_MOVE(_config))
        {}
        ~CumulativeReporterBase() override;

        void benchmarkPreparing( StringRef ) override {}
        void benchmarkStarting( BenchmarkInfo const& ) override {}
        void benchmarkEnded( BenchmarkStats<> const& benchmarkStats ) override;
        void benchmarkFailed( StringRef ) override {}

        void noMatchingTestCases( StringRef ) override {}
        void reportInvalidTestSpec( StringRef ) override {}
        void fatalErrorEncountered( StringRef /*error*/ ) override {}

        void testRunStarting( TestRunInfo const& ) override {}

        void testCaseStarting( TestCaseInfo const& ) override {}
        void testCasePartialStarting( TestCaseInfo const&, uint64_t ) override {}
        void sectionStarting( SectionInfo const& sectionInfo ) override;

        void assertionStarting( AssertionInfo const& ) override {}

        void assertionEnded( AssertionStats const& assertionStats ) override;
        void sectionEnded( SectionStats const& sectionStats ) override;
        void testCasePartialEnded( TestCaseStats const&, uint64_t ) override {}
        void testCaseEnded( TestCaseStats const& testCaseStats ) override;
        void testRunEnded( TestRunStats const& testRunStats ) override;
        //! Customization point: called after last test finishes (testRunEnded has been handled)
        virtual void testRunEndedCumulative() = 0;

        void skipTest(TestCaseInfo const&) override {}

    protected:
        //! Should the cumulative base store the assertion expansion for successful assertions?
        bool m_shouldStoreSuccesfulAssertions = true;
        //! Should the cumulative base store the assertion expansion for failed assertions?
        bool m_shouldStoreFailedAssertions = true;

        // We need lazy construction here. We should probably refactor it
        // later, after the events are redone.
        //! The root node of the test run tree.
        Detail::unique_ptr<TestRunNode> m_testRun;

    private:
        // Note: We rely on pointer identity being stable, which is why
        //       we store pointers to the nodes rather than the values.
        std::vector<Detail::unique_ptr<TestCaseNode>> m_testCases;
        // Root section of the _current_ test case
        Detail::unique_ptr<SectionNode> m_rootSection;
        // Deepest section of the _current_ test case
        SectionNode* m_deepestSection = nullptr;
        // Stack of _active_ sections in the _current_ test case
        std::vector<SectionNode*> m_sectionStack;
    };

} // end namespace Catch

#endif // CATCH_REPORTER_CUMULATIVE_BASE_HPP_INCLUDED



// BEGIN Amalgamated content from catch_reporter_cumulative_base.cpp (@wolfram77)
#ifndef CATCH_REPORTER_CUMULATIVE_BASE_CPP_INCLUDED
#define CATCH_REPORTER_CUMULATIVE_BASE_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_reporter_cumulative_base.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include "../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <algorithm>
#include <cassert>

namespace Catch {
    namespace {
        struct BySectionInfo {
            BySectionInfo( SectionInfo const& other ): m_other( other ) {}
            BySectionInfo( BySectionInfo const& other ) = default;
            bool operator()(
                Detail::unique_ptr<CumulativeReporterBase::SectionNode> const&
                    node ) const {
                return (
                    ( node->stats.sectionInfo.name == m_other.name ) &&
                    ( node->stats.sectionInfo.lineInfo == m_other.lineInfo ) );
            }
            void operator=( BySectionInfo const& ) = delete;

        private:
            SectionInfo const& m_other;
        };

    } // namespace

    namespace Detail {
        AssertionOrBenchmarkResult::AssertionOrBenchmarkResult(
            AssertionStats const& assertion ):
            m_assertion( assertion ) {}

        AssertionOrBenchmarkResult::AssertionOrBenchmarkResult(
            BenchmarkStats<> const& benchmark ):
            m_benchmark( benchmark ) {}

        bool AssertionOrBenchmarkResult::isAssertion() const {
            return m_assertion.some();
        }
        bool AssertionOrBenchmarkResult::isBenchmark() const {
            return m_benchmark.some();
        }

        AssertionStats const& AssertionOrBenchmarkResult::asAssertion() const {
            assert(m_assertion.some());

            return *m_assertion;
        }
        BenchmarkStats<> const& AssertionOrBenchmarkResult::asBenchmark() const {
            assert(m_benchmark.some());

            return *m_benchmark;
        }

    }

    CumulativeReporterBase::~CumulativeReporterBase() = default;

    void CumulativeReporterBase::benchmarkEnded(BenchmarkStats<> const& benchmarkStats) {
        m_sectionStack.back()->assertionsAndBenchmarks.emplace_back(benchmarkStats);
    }

    void
    CumulativeReporterBase::sectionStarting( SectionInfo const& sectionInfo ) {
        // We need a copy, because SectionStats expect to take ownership
        SectionStats incompleteStats( SectionInfo(sectionInfo), Counts(), 0, false );
        SectionNode* node;
        if ( m_sectionStack.empty() ) {
            if ( !m_rootSection ) {
                m_rootSection =
                    Detail::make_unique<SectionNode>( incompleteStats );
            }
            node = m_rootSection.get();
        } else {
            SectionNode& parentNode = *m_sectionStack.back();
            auto it = std::find_if( parentNode.childSections.begin(),
                                    parentNode.childSections.end(),
                                    BySectionInfo( sectionInfo ) );
            if ( it == parentNode.childSections.end() ) {
                auto newNode =
                    Detail::make_unique<SectionNode>( incompleteStats );
                node = newNode.get();
                parentNode.childSections.push_back( CATCH_MOVE( newNode ) );
            } else {
                node = it->get();
            }
        }

        m_deepestSection = node;
        m_sectionStack.push_back( node );
    }

    void CumulativeReporterBase::assertionEnded(
        AssertionStats const& assertionStats ) {
        assert( !m_sectionStack.empty() );
        // AssertionResult holds a pointer to a temporary DecomposedExpression,
        // which getExpandedExpression() calls to build the expression string.
        // Our section stack copy of the assertionResult will likely outlive the
        // temporary, so it must be expanded or discarded now to avoid calling
        // a destroyed object later.
        if ( m_shouldStoreFailedAssertions &&
             !assertionStats.assertionResult.isOk() ) {
            static_cast<void>(
                assertionStats.assertionResult.getExpandedExpression() );
        }
        if ( m_shouldStoreSuccesfulAssertions &&
             assertionStats.assertionResult.isOk() ) {
            static_cast<void>(
                assertionStats.assertionResult.getExpandedExpression() );
        }
        SectionNode& sectionNode = *m_sectionStack.back();
        sectionNode.assertionsAndBenchmarks.emplace_back( assertionStats );
    }

    void CumulativeReporterBase::sectionEnded( SectionStats const& sectionStats ) {
        assert( !m_sectionStack.empty() );
        SectionNode& node = *m_sectionStack.back();
        node.stats = sectionStats;
        m_sectionStack.pop_back();
    }

    void CumulativeReporterBase::testCaseEnded(
        TestCaseStats const& testCaseStats ) {
        auto node = Detail::make_unique<TestCaseNode>( testCaseStats );
        assert( m_sectionStack.size() == 0 );
        node->children.push_back( CATCH_MOVE(m_rootSection) );
        m_testCases.push_back( CATCH_MOVE(node) );

        assert( m_deepestSection );
        m_deepestSection->stdOut = testCaseStats.stdOut;
        m_deepestSection->stdErr = testCaseStats.stdErr;
    }


    void CumulativeReporterBase::testRunEnded( TestRunStats const& testRunStats ) {
        assert(!m_testRun && "CumulativeReporterBase assumes there can only be one test run");
        m_testRun = Detail::make_unique<TestRunNode>( testRunStats );
        m_testRun->children.swap( m_testCases );
        testRunEndedCumulative();
    }

    bool CumulativeReporterBase::SectionNode::hasAnyAssertions() const {
        return std::any_of(
            assertionsAndBenchmarks.begin(),
            assertionsAndBenchmarks.end(),
            []( Detail::AssertionOrBenchmarkResult const& res ) {
                return res.isAssertion();
            } );
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_REPORTER_CUMULATIVE_BASE_CPP_INCLUDED
// END Amalgamated content from catch_reporter_cumulative_base.cpp (@wolfram77)

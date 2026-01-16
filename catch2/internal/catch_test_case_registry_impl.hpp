//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
#define CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED

#include "../interfaces/catch_interfaces_testcase.hpp"  // Adjust to relative path (@wolfram77)
#include "../interfaces/catch_interfaces_config.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)

#include <vector>

namespace Catch {

    class IConfig;
    class ITestInvoker;
    class TestCaseHandle;
    class TestSpec;

    std::vector<TestCaseHandle> sortTests( IConfig const& config, std::vector<TestCaseHandle> const& unsortedTestCases );

    bool isThrowSafe( TestCaseHandle const& testCase, IConfig const& config );

    std::vector<TestCaseHandle> filterTests( std::vector<TestCaseHandle> const& testCases, TestSpec const& testSpec, IConfig const& config );
    std::vector<TestCaseHandle> const& getAllTestCasesSorted( IConfig const& config );

    class TestRegistry : public ITestCaseRegistry {
    public:
        void registerTest( Detail::unique_ptr<TestCaseInfo> testInfo, Detail::unique_ptr<ITestInvoker> testInvoker );

        std::vector<TestCaseInfo*> const& getAllInfos() const override;
        std::vector<TestCaseHandle> const& getAllTests() const override;
        std::vector<TestCaseHandle> const& getAllTestsSorted( IConfig const& config ) const override;

        ~TestRegistry() override; // = default

    private:
        std::vector<Detail::unique_ptr<TestCaseInfo>> m_owned_test_infos;
        // Keeps a materialized vector for `getAllInfos`.
        // We should get rid of that eventually (see interface note)
        std::vector<TestCaseInfo*> m_viewed_test_infos;

        std::vector<Detail::unique_ptr<ITestInvoker>> m_invokers;
        std::vector<TestCaseHandle> m_handles;
        mutable TestRunOrder m_currentSortOrder = TestRunOrder::Declared;
        mutable std::vector<TestCaseHandle> m_sortedFunctions;
    };

    ///////////////////////////////////////////////////////////////////////////


} // end namespace Catch


#endif // CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED



// BEGIN Amalgamated content from catch_test_case_registry_impl.cpp (@wolfram77)
#ifndef CATCH_TEST_CASE_REGISTRY_IMPL_CPP_INCLUDED
#define CATCH_TEST_CASE_REGISTRY_IMPL_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_test_case_registry_impl.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "../interfaces/catch_interfaces_config.hpp"  // Adjust to relative path (@wolfram77)
#include "../interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_sharding.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_test_case_info.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_test_spec.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_test_case_info_hasher.hpp"  // Adjust to relative path (@wolfram77)

#include <algorithm>
#include <set>

namespace Catch {

    namespace {
        static void enforceNoDuplicateTestCases(
            std::vector<TestCaseHandle> const& tests ) {
            auto testInfoCmp = []( TestCaseInfo const* lhs,
                                   TestCaseInfo const* rhs ) {
                return *lhs < *rhs;
            };
            std::set<TestCaseInfo const*, decltype( testInfoCmp )&> seenTests(
                testInfoCmp );
            for ( auto const& test : tests ) {
                const auto infoPtr = &test.getTestCaseInfo();
                const auto prev = seenTests.insert( infoPtr );
                CATCH_ENFORCE( prev.second,
                               "error: test case \""
                                   << infoPtr->name << "\", with tags \""
                                   << infoPtr->tagsAsString()
                                   << "\" already defined.\n"
                                   << "\tFirst seen at "
                                   << ( *prev.first )->lineInfo << "\n"
                                   << "\tRedefined at " << infoPtr->lineInfo );
            }
        }

        static bool matchTest( TestCaseHandle const& testCase,
                               TestSpec const& testSpec,
                               IConfig const& config ) {
            return testSpec.matches( testCase.getTestCaseInfo() ) &&
                   isThrowSafe( testCase, config );
        }

    } // end unnamed namespace

    std::vector<TestCaseHandle> sortTests( IConfig const& config, std::vector<TestCaseHandle> const& unsortedTestCases ) {
        switch (config.runOrder()) {
        case TestRunOrder::Declared:
            return unsortedTestCases;

        case TestRunOrder::LexicographicallySorted: {
            std::vector<TestCaseHandle> sorted = unsortedTestCases;
            std::sort(
                sorted.begin(),
                sorted.end(),
                []( TestCaseHandle const& lhs, TestCaseHandle const& rhs ) {
                    return lhs.getTestCaseInfo() < rhs.getTestCaseInfo();
                }
            );
            return sorted;
        }
        case TestRunOrder::Randomized: {
            using TestWithHash = std::pair<TestCaseInfoHasher::hash_t, TestCaseHandle>;

            TestCaseInfoHasher h{ config.rngSeed() };
            std::vector<TestWithHash> indexed_tests;
            indexed_tests.reserve(unsortedTestCases.size());

            for (auto const& handle : unsortedTestCases) {
                indexed_tests.emplace_back(h(handle.getTestCaseInfo()), handle);
            }

            std::sort( indexed_tests.begin(),
                       indexed_tests.end(),
                       []( TestWithHash const& lhs, TestWithHash const& rhs ) {
                           if ( lhs.first == rhs.first ) {
                               return lhs.second.getTestCaseInfo() <
                                      rhs.second.getTestCaseInfo();
                           }
                           return lhs.first < rhs.first;
                       } );

            std::vector<TestCaseHandle> randomized;
            randomized.reserve(indexed_tests.size());

            for (auto const& indexed : indexed_tests) {
                randomized.push_back(indexed.second);
            }

            return randomized;
        }
        }

        CATCH_INTERNAL_ERROR("Unknown test order value!");
    }

    bool isThrowSafe( TestCaseHandle const& testCase, IConfig const& config ) {
        return !testCase.getTestCaseInfo().throws() || config.allowThrows();
    }

    std::vector<TestCaseHandle> filterTests( std::vector<TestCaseHandle> const& testCases, TestSpec const& testSpec, IConfig const& config ) {
        std::vector<TestCaseHandle> filtered;
        filtered.reserve( testCases.size() );
        for (auto const& testCase : testCases) {
            if ((!testSpec.hasFilters() && !testCase.getTestCaseInfo().isHidden()) ||
                (testSpec.hasFilters() && matchTest(testCase, testSpec, config))) {
                filtered.push_back(testCase);
            }
        }
        return createShard(filtered, config.shardCount(), config.shardIndex());
    }
    std::vector<TestCaseHandle> const& getAllTestCasesSorted( IConfig const& config ) {
        return getRegistryHub().getTestCaseRegistry().getAllTestsSorted( config );
    }

    TestRegistry::~TestRegistry() = default;

    void TestRegistry::registerTest(Detail::unique_ptr<TestCaseInfo> testInfo, Detail::unique_ptr<ITestInvoker> testInvoker) {
        m_handles.emplace_back(testInfo.get(), testInvoker.get());
        m_viewed_test_infos.push_back(testInfo.get());
        m_owned_test_infos.push_back(CATCH_MOVE(testInfo));
        m_invokers.push_back(CATCH_MOVE(testInvoker));
    }

    std::vector<TestCaseInfo*> const& TestRegistry::getAllInfos() const {
        return m_viewed_test_infos;
    }

    std::vector<TestCaseHandle> const& TestRegistry::getAllTests() const {
        return m_handles;
    }
    std::vector<TestCaseHandle> const& TestRegistry::getAllTestsSorted( IConfig const& config ) const {
        if( m_sortedFunctions.empty() )
            enforceNoDuplicateTestCases( m_handles );

        if(  m_currentSortOrder != config.runOrder() || m_sortedFunctions.empty() ) {
            m_sortedFunctions = sortTests( config, m_handles );
            m_currentSortOrder = config.runOrder();
        }
        return m_sortedFunctions;
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TEST_CASE_REGISTRY_IMPL_CPP_INCLUDED
// END Amalgamated content from catch_test_case_registry_impl.cpp (@wolfram77)

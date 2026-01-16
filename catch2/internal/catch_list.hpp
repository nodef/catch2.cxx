//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_LIST_HPP_INCLUDED
#define CATCH_LIST_HPP_INCLUDED

#include "catch_stringref.hpp"  // Adjust to relative path (@wolfram77)

#include <set>
#include <string>


namespace Catch {

    class IEventListener;
    class Config;


    struct ReporterDescription {
        std::string name, description;
    };
    struct ListenerDescription {
        StringRef name;
        std::string description;
    };

    struct TagInfo {
        void add(StringRef spelling);
        std::string all() const;

        std::set<StringRef> spellings;
        std::size_t count = 0;
    };

    bool list( IEventListener& reporter, Config const& config );

} // end namespace Catch

#endif // CATCH_LIST_HPP_INCLUDED



// BEGIN Amalgamated content from catch_list.cpp (@wolfram77)
#ifndef CATCH_LIST_CPP_INCLUDED
#define CATCH_LIST_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_list.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include "../interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)
#include "../interfaces/catch_interfaces_reporter.hpp"  // Adjust to relative path (@wolfram77)
#include "../interfaces/catch_interfaces_reporter_factory.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_test_case_registry_impl.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reporter_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_case_insensitive_comparisons.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_config.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_test_case_info.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
    namespace {

        void listTests(IEventListener& reporter, IConfig const& config) {
            auto const& testSpec = config.testSpec();
            auto matchedTestCases = filterTests(getAllTestCasesSorted(config), testSpec, config);
            reporter.listTests(matchedTestCases);
        }

        void listTags(IEventListener& reporter, IConfig const& config) {
            auto const& testSpec = config.testSpec();
            std::vector<TestCaseHandle> matchedTestCases = filterTests(getAllTestCasesSorted(config), testSpec, config);

            std::map<StringRef, TagInfo, Detail::CaseInsensitiveLess> tagCounts;
            for (auto const& testCase : matchedTestCases) {
                for (auto const& tagName : testCase.getTestCaseInfo().tags) {
                    auto it = tagCounts.find(tagName.original);
                    if (it == tagCounts.end())
                        it = tagCounts.insert(std::make_pair(tagName.original, TagInfo())).first;
                    it->second.add(tagName.original);
                }
            }

            std::vector<TagInfo> infos; infos.reserve(tagCounts.size());
            for (auto& tagc : tagCounts) {
                infos.push_back(CATCH_MOVE(tagc.second));
            }

            reporter.listTags(infos);
        }

        void listReporters(IEventListener& reporter) {
            std::vector<ReporterDescription> descriptions;

            auto const& factories = getRegistryHub().getReporterRegistry().getFactories();
            descriptions.reserve(factories.size());
            for (auto const& fac : factories) {
                descriptions.push_back({ fac.first, fac.second->getDescription() });
            }

            reporter.listReporters(descriptions);
        }

        void listListeners(IEventListener& reporter) {
            std::vector<ListenerDescription> descriptions;

            auto const& factories =
                getRegistryHub().getReporterRegistry().getListeners();
            descriptions.reserve( factories.size() );
            for ( auto const& fac : factories ) {
                descriptions.push_back( { fac->getName(), fac->getDescription() } );
            }

            reporter.listListeners( descriptions );
        }

    } // end anonymous namespace

    void TagInfo::add( StringRef spelling ) {
        ++count;
        spellings.insert( spelling );
    }

    std::string TagInfo::all() const {
        // 2 per tag for brackets '[' and ']'
        size_t size =  spellings.size() * 2;
        for (auto const& spelling : spellings) {
            size += spelling.size();
        }

        std::string out; out.reserve(size);
        for (auto const& spelling : spellings) {
            out += '[';
            out += spelling;
            out += ']';
        }
        return out;
    }

    bool list( IEventListener& reporter, Config const& config ) {
        bool listed = false;
        if (config.listTests()) {
            listed = true;
            listTests(reporter, config);
        }
        if (config.listTags()) {
            listed = true;
            listTags(reporter, config);
        }
        if (config.listReporters()) {
            listed = true;
            listReporters(reporter);
        }
        if ( config.listListeners() ) {
            listed = true;
            listListeners( reporter );
        }
        return listed;
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_LIST_CPP_INCLUDED
// END Amalgamated content from catch_list.cpp (@wolfram77)

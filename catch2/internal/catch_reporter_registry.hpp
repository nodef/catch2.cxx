//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_REGISTRY_HPP_INCLUDED
#define CATCH_REPORTER_REGISTRY_HPP_INCLUDED

#include "catch_case_insensitive_comparisons.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)

#include <map>
#include <string>
#include <vector>

namespace Catch {

    class IEventListener;
    using IEventListenerPtr = Detail::unique_ptr<IEventListener>;
    class IReporterFactory;
    using IReporterFactoryPtr = Detail::unique_ptr<IReporterFactory>;
    struct ReporterConfig;
    class EventListenerFactory;

    class ReporterRegistry {
        struct ReporterRegistryImpl;
        Detail::unique_ptr<ReporterRegistryImpl> m_impl;

    public:
        ReporterRegistry();
        ~ReporterRegistry(); // = default;

        IEventListenerPtr create( std::string const& name,
                                  ReporterConfig&& config ) const;

        void registerReporter( std::string const& name,
                               IReporterFactoryPtr factory );

        void
        registerListener( Detail::unique_ptr<EventListenerFactory> factory );

        std::map<std::string,
                 IReporterFactoryPtr,
                 Detail::CaseInsensitiveLess> const&
        getFactories() const;

        std::vector<Detail::unique_ptr<EventListenerFactory>> const&
        getListeners() const;
    };

} // end namespace Catch

#endif // CATCH_REPORTER_REGISTRY_HPP_INCLUDED



// BEGIN Amalgamated content from catch_reporter_registry.cpp (@wolfram77)
#ifndef CATCH_REPORTER_REGISTRY_CPP_INCLUDED
#define CATCH_REPORTER_REGISTRY_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include "../interfaces/catch_interfaces_reporter_factory.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)
// #include "catch_reporter_registry.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_automake.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_compact.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_console.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_json.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_junit.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_registrars.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_sonarqube.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_tap.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_teamcity.hpp"  // Adjust to relative path (@wolfram77)
#include "../reporters/catch_reporter_xml.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
    struct ReporterRegistry::ReporterRegistryImpl {
        std::vector<Detail::unique_ptr<EventListenerFactory>> listeners;
        std::map<std::string, IReporterFactoryPtr, Detail::CaseInsensitiveLess>
            factories;
    };

    ReporterRegistry::ReporterRegistry():
        m_impl( Detail::make_unique<ReporterRegistryImpl>() ) {
        // Because it is impossible to move out of initializer list,
        // we have to add the elements manually
        m_impl->factories["Automake"] =
            Detail::make_unique<ReporterFactory<AutomakeReporter>>();
        m_impl->factories["compact"] =
            Detail::make_unique<ReporterFactory<CompactReporter>>();
        m_impl->factories["console"] =
            Detail::make_unique<ReporterFactory<ConsoleReporter>>();
        m_impl->factories["JUnit"] =
            Detail::make_unique<ReporterFactory<JunitReporter>>();
        m_impl->factories["SonarQube"] =
            Detail::make_unique<ReporterFactory<SonarQubeReporter>>();
        m_impl->factories["TAP"] =
            Detail::make_unique<ReporterFactory<TAPReporter>>();
        m_impl->factories["TeamCity"] =
            Detail::make_unique<ReporterFactory<TeamCityReporter>>();
        m_impl->factories["XML"] =
            Detail::make_unique<ReporterFactory<XmlReporter>>();
        m_impl->factories["JSON"] =
            Detail::make_unique<ReporterFactory<JsonReporter>>();
    }

    ReporterRegistry::~ReporterRegistry() = default;

    IEventListenerPtr
    ReporterRegistry::create( std::string const& name,
                              ReporterConfig&& config ) const {
        auto it = m_impl->factories.find( name );
        if ( it == m_impl->factories.end() ) return nullptr;
        return it->second->create( CATCH_MOVE( config ) );
    }

    void ReporterRegistry::registerReporter( std::string const& name,
                                             IReporterFactoryPtr factory ) {
        CATCH_ENFORCE( name.find( "::" ) == name.npos,
                       "'::' is not allowed in reporter name: '" + name +
                           '\'' );
        auto ret = m_impl->factories.emplace( name, CATCH_MOVE( factory ) );
        CATCH_ENFORCE( ret.second,
                       "reporter using '" + name +
                           "' as name was already registered" );
    }
    void ReporterRegistry::registerListener(
        Detail::unique_ptr<EventListenerFactory> factory ) {
        m_impl->listeners.push_back( CATCH_MOVE( factory ) );
    }

    std::map<std::string,
             IReporterFactoryPtr,
             Detail::CaseInsensitiveLess> const&
    ReporterRegistry::getFactories() const {
        return m_impl->factories;
    }

    std::vector<Detail::unique_ptr<EventListenerFactory>> const&
    ReporterRegistry::getListeners() const {
        return m_impl->listeners;
    }
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_REPORTER_REGISTRY_CPP_INCLUDED
// END Amalgamated content from catch_reporter_registry.cpp (@wolfram77)

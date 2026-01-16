#ifndef CATCH_REGISTRY_HUB_CPP_INCLUDED
#define CATCH_REGISTRY_HUB_CPP_INCLUDED
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include "interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)

#include "internal/catch_context.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_test_case_registry_impl.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_reporter_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_exception_translator_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_tag_alias_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_startup_exception_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_singletons.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_enum_values_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_test_case_info.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_noncopyable.hpp"  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_reporter_factory.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <exception>

namespace Catch {

    namespace {

        class RegistryHub : public IRegistryHub,
                            public IMutableRegistryHub,
                            private Detail::NonCopyable {

        public: // IRegistryHub
            RegistryHub() = default;
            ReporterRegistry const& getReporterRegistry() const override {
                return m_reporterRegistry;
            }
            ITestCaseRegistry const& getTestCaseRegistry() const override {
                return m_testCaseRegistry;
            }
            IExceptionTranslatorRegistry const& getExceptionTranslatorRegistry() const override {
                return m_exceptionTranslatorRegistry;
            }
            ITagAliasRegistry const& getTagAliasRegistry() const override {
                return m_tagAliasRegistry;
            }
            StartupExceptionRegistry const& getStartupExceptionRegistry() const override {
                return m_exceptionRegistry;
            }

        public: // IMutableRegistryHub
            void registerReporter( std::string const& name, IReporterFactoryPtr factory ) override {
                m_reporterRegistry.registerReporter( name, CATCH_MOVE(factory) );
            }
            void registerListener( Detail::unique_ptr<EventListenerFactory> factory ) override {
                m_reporterRegistry.registerListener( CATCH_MOVE(factory) );
            }
            void registerTest( Detail::unique_ptr<TestCaseInfo>&& testInfo, Detail::unique_ptr<ITestInvoker>&& invoker ) override {
                m_testCaseRegistry.registerTest( CATCH_MOVE(testInfo), CATCH_MOVE(invoker) );
            }
            void registerTranslator( Detail::unique_ptr<IExceptionTranslator>&& translator ) override {
                m_exceptionTranslatorRegistry.registerTranslator( CATCH_MOVE(translator) );
            }
            void registerTagAlias( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) override {
                m_tagAliasRegistry.add( alias, tag, lineInfo );
            }
            void registerStartupException() noexcept override {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
                m_exceptionRegistry.add(std::current_exception());
#else
                CATCH_INTERNAL_ERROR("Attempted to register active exception under CATCH_CONFIG_DISABLE_EXCEPTIONS!");
#endif
            }
            IMutableEnumValuesRegistry& getMutableEnumValuesRegistry() override {
                return m_enumValuesRegistry;
            }

        private:
            TestRegistry m_testCaseRegistry;
            ReporterRegistry m_reporterRegistry;
            ExceptionTranslatorRegistry m_exceptionTranslatorRegistry;
            TagAliasRegistry m_tagAliasRegistry;
            StartupExceptionRegistry m_exceptionRegistry;
            Detail::EnumValuesRegistry m_enumValuesRegistry;
        };
    }

    using RegistryHubSingleton = Singleton<RegistryHub, IRegistryHub, IMutableRegistryHub>;

    IRegistryHub const& getRegistryHub() {
        return RegistryHubSingleton::get();
    }
    IMutableRegistryHub& getMutableRegistryHub() {
        return RegistryHubSingleton::getMutable();
    }
    void cleanUp() {
        cleanupSingletons();
    }
    std::string translateActiveException() {
        return getRegistryHub().getExceptionTranslatorRegistry().translateActiveException();
    }


} // end namespace Catch
#endif // CATCH_REGISTRY_HUB_CPP_INCLUDED

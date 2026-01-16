//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_EXCEPTION_TRANSLATOR_REGISTRY_HPP_INCLUDED
#define CATCH_EXCEPTION_TRANSLATOR_REGISTRY_HPP_INCLUDED

#include "../interfaces/catch_interfaces_exception.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch {

    class ExceptionTranslatorRegistry : public IExceptionTranslatorRegistry {
    public:
        ~ExceptionTranslatorRegistry() override;
        void registerTranslator( Detail::unique_ptr<IExceptionTranslator>&& translator );
        std::string translateActiveException() const override;

    private:
        ExceptionTranslators m_translators;
    };
}

#endif // CATCH_EXCEPTION_TRANSLATOR_REGISTRY_HPP_INCLUDED



// BEGIN Amalgamated content from catch_exception_translator_registry.cpp (@wolfram77)
#ifndef CATCH_EXCEPTION_TRANSLATOR_REGISTRY_CPP_INCLUDED
#define CATCH_EXCEPTION_TRANSLATOR_REGISTRY_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_exception_translator_registry.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_test_failure_exception.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <exception>

namespace Catch {

#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    namespace {
        static std::string tryTranslators(
            std::vector<
                Detail::unique_ptr<IExceptionTranslator const>> const& translators ) {
            if ( translators.empty() ) {
                std::rethrow_exception( std::current_exception() );
            } else {
                return translators[0]->translate( translators.begin() + 1,
                                                  translators.end() );
            }
        }

    }
#endif //!defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)

    ExceptionTranslatorRegistry::~ExceptionTranslatorRegistry() = default;

    void ExceptionTranslatorRegistry::registerTranslator( Detail::unique_ptr<IExceptionTranslator>&& translator ) {
        m_translators.push_back( CATCH_MOVE( translator ) );
    }

#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    std::string ExceptionTranslatorRegistry::translateActiveException() const {
        // Compiling a mixed mode project with MSVC means that CLR
        // exceptions will be caught in (...) as well. However, these do
        // do not fill-in std::current_exception and thus lead to crash
        // when attempting rethrow.
        // /EHa switch also causes structured exceptions to be caught
        // here, but they fill-in current_exception properly, so
        // at worst the output should be a little weird, instead of
        // causing a crash.
        if ( std::current_exception() == nullptr ) {
            return "Non C++ exception. Possibly a CLR exception.";
        }

        // First we try user-registered translators. If none of them can
        // handle the exception, it will be rethrown handled by our defaults.
        try {
            return tryTranslators(m_translators);
        }
        // To avoid having to handle TFE explicitly everywhere, we just
        // rethrow it so that it goes back up the caller.
        catch( TestFailureException& ) {
            return "{ nested assertion failed }";
        }
        catch( TestSkipException& ) {
            return "{ nested SKIP() called }";
        }
        catch( std::exception const& ex ) {
            return ex.what();
        }
        catch( std::string const& msg ) {
            return msg;
        }
        catch( const char* msg ) {
            return msg;
        }
        catch(...) {
            return "Unknown exception";
        }
    }

#else // ^^ Exceptions are enabled // Exceptions are disabled vv
    std::string ExceptionTranslatorRegistry::translateActiveException() const {
        CATCH_INTERNAL_ERROR("Attempted to translate active exception under CATCH_CONFIG_DISABLE_EXCEPTIONS!");
    }
#endif

}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_EXCEPTION_TRANSLATOR_REGISTRY_CPP_INCLUDED
// END Amalgamated content from catch_exception_translator_registry.cpp (@wolfram77)

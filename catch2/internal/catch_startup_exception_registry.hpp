//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_STARTUP_EXCEPTION_REGISTRY_HPP_INCLUDED
#define CATCH_STARTUP_EXCEPTION_REGISTRY_HPP_INCLUDED


#include <vector>
#include <exception>

namespace Catch {

    class StartupExceptionRegistry {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    public:
        void add(std::exception_ptr const& exception) noexcept;
        std::vector<std::exception_ptr> const& getExceptions() const noexcept;
    private:
        std::vector<std::exception_ptr> m_exceptions;
#endif
    };

} // end namespace Catch

#endif // CATCH_STARTUP_EXCEPTION_REGISTRY_HPP_INCLUDED



// BEGIN Amalgamated content from catch_startup_exception_registry.cpp (@wolfram77)
#ifndef CATCH_STARTUP_EXCEPTION_REGISTRY_CPP_INCLUDED
#define CATCH_STARTUP_EXCEPTION_REGISTRY_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_startup_exception_registry.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_user_config.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    void StartupExceptionRegistry::add( std::exception_ptr const& exception ) noexcept {
        CATCH_TRY {
            m_exceptions.push_back(exception);
        } CATCH_CATCH_ALL {
            // If we run out of memory during start-up there's really not a lot more we can do about it
            std::terminate();
        }
    }

    std::vector<std::exception_ptr> const& StartupExceptionRegistry::getExceptions() const noexcept {
        return m_exceptions;
    }
#endif

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_STARTUP_EXCEPTION_REGISTRY_CPP_INCLUDED
// END Amalgamated content from catch_startup_exception_registry.cpp (@wolfram77)

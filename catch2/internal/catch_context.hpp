//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CONTEXT_HPP_INCLUDED
#define CATCH_CONTEXT_HPP_INCLUDED

#include "catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    class IResultCapture;
    class IConfig;

    class Context {
        IConfig const* m_config = nullptr;
        IResultCapture* m_resultCapture = nullptr;

        CATCH_EXPORT static Context currentContext;
        friend Context& getCurrentMutableContext();
        friend Context const& getCurrentContext();

    public:
        constexpr IResultCapture* getResultCapture() const {
            return m_resultCapture;
        }
        constexpr IConfig const* getConfig() const { return m_config; }
        constexpr void setResultCapture( IResultCapture* resultCapture ) {
            m_resultCapture = resultCapture;
        }
        constexpr void setConfig( IConfig const* config ) { m_config = config; }
    };

    Context& getCurrentMutableContext();

    inline Context const& getCurrentContext() {
        return Context::currentContext;
    }

    class SimplePcg32;
    SimplePcg32& sharedRng();
}

#endif // CATCH_CONTEXT_HPP_INCLUDED



// BEGIN Amalgamated content from catch_context.cpp (@wolfram77)
#ifndef CATCH_CONTEXT_CPP_INCLUDED
#define CATCH_CONTEXT_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_context.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_random_number_generator.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    Context Context::currentContext;

    Context& getCurrentMutableContext() {
        return Context::currentContext;
    }

    SimplePcg32& sharedRng() {
        static SimplePcg32 s_rng;
        return s_rng;
    }

}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_CONTEXT_CPP_INCLUDED
// END Amalgamated content from catch_context.cpp (@wolfram77)

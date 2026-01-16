//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MATCHERS_IMPL_HPP_INCLUDED
#define CATCH_MATCHERS_IMPL_HPP_INCLUDED

#include "../../internal/catch_assertion_handler.hpp"  // Adjust to relative path (@wolfram77)
#include "../../internal/catch_source_line_info.hpp"  // Adjust to relative path (@wolfram77)
#include "../../internal/catch_decomposer.hpp"  // Adjust to relative path (@wolfram77)
#include "../../internal/catch_preprocessor_internal_stringify.hpp"  // Adjust to relative path (@wolfram77)
#include "../../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch {

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wsign-compare"
#    pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsign-compare"
#    pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

    template<typename ArgT, typename MatcherT>
    class MatchExpr : public ITransientExpression {
        ArgT && m_arg;
        MatcherT const& m_matcher;
    public:
        constexpr MatchExpr( ArgT && arg, MatcherT const& matcher )
        :   ITransientExpression{ true, matcher.match( arg ) }, // not forwarding arg here on purpose
            m_arg( CATCH_FORWARD(arg) ),
            m_matcher( matcher )
        {}

        void streamReconstructedExpression( std::ostream& os ) const override {
            os << Catch::Detail::stringify( m_arg )
               << ' '
               << m_matcher.toString();
        }
    };

#ifdef __clang__
#    pragma clang diagnostic pop
#elif defined __GNUC__
#    pragma GCC diagnostic pop
#endif


    namespace Matchers {
        template <typename ArgT>
        class MatcherBase;
    }

    using StringMatcher = Matchers::MatcherBase<std::string>;

    void handleExceptionMatchExpr( AssertionHandler& handler, StringMatcher const& matcher );

    template<typename ArgT, typename MatcherT>
    constexpr MatchExpr<ArgT, MatcherT>
    makeMatchExpr( ArgT&& arg, MatcherT const& matcher ) {
        return MatchExpr<ArgT, MatcherT>( CATCH_FORWARD(arg), matcher );
    }

} // namespace Catch


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( macroName, matcher, resultDisposition, arg ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName##_catch_sr, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(arg) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        INTERNAL_CATCH_TRY { \
            catchAssertionHandler.handleExpr( Catch::makeMatchExpr( arg, matcher ) ); \
        } INTERNAL_CATCH_CATCH( catchAssertionHandler ) \
        catchAssertionHandler.complete(); \
    } while( false )


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_MATCHES( macroName, exceptionType, resultDisposition, matcher, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName##_catch_sr, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(__VA_ARGS__) ", " CATCH_INTERNAL_STRINGIFY(exceptionType) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        if( catchAssertionHandler.allowThrows() ) \
            try { \
                CATCH_INTERNAL_START_WARNINGS_SUPPRESSION \
                CATCH_INTERNAL_SUPPRESS_USELESS_CAST_WARNINGS \
                static_cast<void>(__VA_ARGS__ ); \
                CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION \
                catchAssertionHandler.handleUnexpectedExceptionNotThrown(); \
            } \
            catch( exceptionType const& ex ) { \
                catchAssertionHandler.handleExpr( Catch::makeMatchExpr( ex, matcher ) ); \
            } \
            catch( ... ) { \
                catchAssertionHandler.handleUnexpectedInflightException(); \
            } \
        else \
            catchAssertionHandler.handleThrowingCallSkipped(); \
        catchAssertionHandler.complete(); \
    } while( false )


#endif // CATCH_MATCHERS_IMPL_HPP_INCLUDED



// BEGIN Amalgamated content from catch_matchers_impl.cpp (@wolfram77)
#ifndef CATCH_MATCHERS_IMPL_CPP_INCLUDED
#define CATCH_MATCHERS_IMPL_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_matchers_impl.hpp" // Disable self-include (@wolfram77)
// #include "../catch_matchers.hpp"  // Disable circular include (@wolfram77)
#include "../../interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)
#include "../../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    // This is the general overload that takes a any string matcher
    // There is another overload, in catch_assertionhandler.h/.cpp, that only takes a string and infers
    // the Equals matcher (so the header does not mention matchers)
    void handleExceptionMatchExpr( AssertionHandler& handler, StringMatcher const& matcher ) {
        std::string exceptionMessage = Catch::translateActiveException();
        MatchExpr<std::string, StringMatcher const&> expr( CATCH_MOVE(exceptionMessage), matcher );
        handler.handleExpr( expr );
    }

} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_MATCHERS_IMPL_CPP_INCLUDED
// END Amalgamated content from catch_matchers_impl.cpp (@wolfram77)

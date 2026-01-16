//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_LAZY_EXPR_HPP_INCLUDED
#define CATCH_LAZY_EXPR_HPP_INCLUDED

#include <iosfwd>

namespace Catch {

    class ITransientExpression;

    class LazyExpression {
        friend class AssertionHandler;
        friend struct AssertionStats;
        friend class RunContext;

        ITransientExpression const* m_transientExpression = nullptr;
        bool m_isNegated;
    public:
        constexpr LazyExpression( bool isNegated ):
            m_isNegated(isNegated)
        {}
        constexpr LazyExpression(LazyExpression const& other) = default;
        LazyExpression& operator = ( LazyExpression const& ) = delete;

        constexpr explicit operator bool() const {
            return m_transientExpression != nullptr;
        }

        friend auto operator << ( std::ostream& os, LazyExpression const& lazyExpr ) -> std::ostream&;
    };

} // namespace Catch

#endif // CATCH_LAZY_EXPR_HPP_INCLUDED



// BEGIN Amalgamated content from catch_lazy_expr.cpp (@wolfram77)
#ifndef CATCH_LAZY_EXPR_CPP_INCLUDED
#define CATCH_LAZY_EXPR_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_lazy_expr.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_decomposer.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    auto operator << (std::ostream& os, LazyExpression const& lazyExpr) -> std::ostream& {
        if (lazyExpr.m_isNegated)
            os << '!';

        if (lazyExpr) {
            if (lazyExpr.m_isNegated && lazyExpr.m_transientExpression->isBinaryExpression())
                os << '(' << *lazyExpr.m_transientExpression << ')';
            else
                os << *lazyExpr.m_transientExpression;
        } else {
            os << "{** error - unchecked empty expression requested **}";
        }
        return os;
    }

} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_LAZY_EXPR_CPP_INCLUDED
// END Amalgamated content from catch_lazy_expr.cpp (@wolfram77)

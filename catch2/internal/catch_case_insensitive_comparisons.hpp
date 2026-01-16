//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CASE_INSENSITIVE_COMPARISONS_HPP_INCLUDED
#define CATCH_CASE_INSENSITIVE_COMPARISONS_HPP_INCLUDED

#include "catch_stringref.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
    namespace Detail {
        //! Provides case-insensitive `op<` semantics when called
        struct CaseInsensitiveLess {
            bool operator()( StringRef lhs,
                             StringRef rhs ) const;
        };

        //! Provides case-insensitive `op==` semantics when called
        struct CaseInsensitiveEqualTo {
            bool operator()( StringRef lhs,
                             StringRef rhs ) const;
        };

    } // namespace Detail
} // namespace Catch

#endif // CATCH_CASE_INSENSITIVE_COMPARISONS_HPP_INCLUDED



// BEGIN Amalgamated content from catch_case_insensitive_comparisons.cpp (@wolfram77)
#ifndef CATCH_CASE_INSENSITIVE_COMPARISONS_CPP_INCLUDED
#define CATCH_CASE_INSENSITIVE_COMPARISONS_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_case_insensitive_comparisons.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_string_manip.hpp"  // Adjust to relative path (@wolfram77)

#include <algorithm>

namespace Catch {
    namespace Detail {

        bool CaseInsensitiveLess::operator()( StringRef lhs,
                                              StringRef rhs ) const {
            return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                []( char l, char r ) { return toLower( l ) < toLower( r ); } );
        }

        bool
        CaseInsensitiveEqualTo::operator()( StringRef lhs,
                                            StringRef rhs ) const {
            return std::equal(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                []( char l, char r ) { return toLower( l ) == toLower( r ); } );
        }

    } // namespace Detail
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_CASE_INSENSITIVE_COMPARISONS_CPP_INCLUDED
// END Amalgamated content from catch_case_insensitive_comparisons.cpp (@wolfram77)

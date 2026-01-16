//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_WILDCARD_PATTERN_HPP_INCLUDED
#define CATCH_WILDCARD_PATTERN_HPP_INCLUDED

#include "../catch_case_sensitive.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch
{
    class WildcardPattern {
        enum WildcardPosition {
            NoWildcard = 0,
            WildcardAtStart = 1,
            WildcardAtEnd = 2,
            WildcardAtBothEnds = WildcardAtStart | WildcardAtEnd
        };

    public:

        WildcardPattern( std::string const& pattern, CaseSensitive caseSensitivity );
        bool matches( std::string const& str ) const;

    private:
        std::string normaliseString( std::string const& str ) const;
        CaseSensitive m_caseSensitivity;
        WildcardPosition m_wildcard = NoWildcard;
        std::string m_pattern;
    };
}

#endif // CATCH_WILDCARD_PATTERN_HPP_INCLUDED



// BEGIN Amalgamated content from catch_wildcard_pattern.cpp (@wolfram77)
#ifndef CATCH_WILDCARD_PATTERN_CPP_INCLUDED
#define CATCH_WILDCARD_PATTERN_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_wildcard_pattern.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_string_manip.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    WildcardPattern::WildcardPattern( std::string const& pattern,
                                      CaseSensitive caseSensitivity )
    :   m_caseSensitivity( caseSensitivity ),
        m_pattern( normaliseString( pattern ) )
    {
        if( startsWith( m_pattern, '*' ) ) {
            m_pattern = m_pattern.substr( 1 );
            m_wildcard = WildcardAtStart;
        }
        if( endsWith( m_pattern, '*' ) ) {
            m_pattern = m_pattern.substr( 0, m_pattern.size()-1 );
            m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtEnd );
        }
    }

    bool WildcardPattern::matches( std::string const& str ) const {
        switch( m_wildcard ) {
            case NoWildcard:
                return m_pattern == normaliseString( str );
            case WildcardAtStart:
                return endsWith( normaliseString( str ), m_pattern );
            case WildcardAtEnd:
                return startsWith( normaliseString( str ), m_pattern );
            case WildcardAtBothEnds:
                return contains( normaliseString( str ), m_pattern );
            default:
                CATCH_INTERNAL_ERROR( "Unknown enum" );
        }
    }

    std::string WildcardPattern::normaliseString( std::string const& str ) const {
        return trim( m_caseSensitivity == CaseSensitive::No ? toLower( str ) : str );
    }
}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_WILDCARD_PATTERN_CPP_INCLUDED
// END Amalgamated content from catch_wildcard_pattern.cpp (@wolfram77)

//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_STRINGREF_HPP_INCLUDED
#define CATCH_STRINGREF_HPP_INCLUDED

#include "catch_lifetimebound.hpp"  // Adjust to relative path (@wolfram77)

#include <cstddef>
#include <string>
#include <iosfwd>
#include <cassert>
#include <cstring>

namespace Catch {

    /// A non-owning string class (similar to the forthcoming std::string_view)
    /// Note that, because a StringRef may be a substring of another string,
    /// it may not be null terminated.
    class StringRef {
    public:
        using size_type = std::size_t;
        using const_iterator = const char*;

        static constexpr size_type npos{ static_cast<size_type>( -1 ) };

    private:
        static constexpr char const* const s_empty = "";

        char const* m_start = s_empty;
        size_type m_size = 0;

    public: // construction
        constexpr StringRef() noexcept = default;

        StringRef( char const* rawChars CATCH_ATTR_LIFETIMEBOUND ) noexcept;

        constexpr StringRef( char const* rawChars CATCH_ATTR_LIFETIMEBOUND,
                             size_type size ) noexcept
        :   m_start( rawChars ),
            m_size( size )
        {}

        StringRef(
            std::string const& stdString CATCH_ATTR_LIFETIMEBOUND ) noexcept
        :   m_start( stdString.c_str() ),
            m_size( stdString.size() )
        {}

        explicit operator std::string() const {
            return std::string(m_start, m_size);
        }

    public: // operators
        auto operator == ( StringRef other ) const noexcept -> bool {
            return m_size == other.m_size
                && (std::memcmp( m_start, other.m_start, m_size ) == 0);
        }
        auto operator != (StringRef other) const noexcept -> bool {
            return !(*this == other);
        }

        constexpr auto operator[] ( size_type index ) const noexcept -> char {
            assert(index < m_size);
            return m_start[index];
        }

        bool operator<(StringRef rhs) const noexcept;

    public: // named queries
        constexpr auto empty() const noexcept -> bool {
            return m_size == 0;
        }
        constexpr auto size() const noexcept -> size_type {
            return m_size;
        }

        // Returns a substring of [start, start + length).
        // If start + length > size(), then the substring is [start, size()).
        // If start > size(), then the substring is empty.
        constexpr StringRef substr(size_type start, size_type length) const noexcept {
            if (start < m_size) {
                const auto shortened_size = m_size - start;
                return StringRef(m_start + start, (shortened_size < length) ? shortened_size : length);
            } else {
                return StringRef();
            }
        }

        // Returns the current start pointer. May not be null-terminated.
        constexpr char const* data() const noexcept CATCH_ATTR_LIFETIMEBOUND {
            return m_start;
        }

        constexpr const_iterator begin() const { return m_start; }
        constexpr const_iterator end() const { return m_start + m_size; }


        friend std::string& operator += (std::string& lhs, StringRef rhs);
        friend std::ostream& operator << (std::ostream& os, StringRef str);
        friend std::string operator+(StringRef lhs, StringRef rhs);

        /**
         * Provides a three-way comparison with rhs
         *
         * Returns negative number if lhs < rhs, 0 if lhs == rhs, and a positive
         * number if lhs > rhs
         */
        int compare( StringRef rhs ) const;
    };


    constexpr auto operator ""_sr( char const* rawChars, std::size_t size ) noexcept -> StringRef {
        return StringRef( rawChars, size );
    }
} // namespace Catch

constexpr auto operator ""_catch_sr( char const* rawChars, std::size_t size ) noexcept -> Catch::StringRef {
    return Catch::StringRef( rawChars, size );
}

#endif // CATCH_STRINGREF_HPP_INCLUDED



// BEGIN Amalgamated content from catch_stringref.cpp (@wolfram77)
#ifndef CATCH_STRINGREF_CPP_INCLUDED
#define CATCH_STRINGREF_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_stringref.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include <algorithm>
#include <ostream>
#include <cstring>

namespace Catch {
    StringRef::StringRef( char const* rawChars ) noexcept
    : StringRef( rawChars, std::strlen(rawChars) )
    {}


    bool StringRef::operator<(StringRef rhs) const noexcept {
        if (m_size < rhs.m_size) {
            return strncmp(m_start, rhs.m_start, m_size) <= 0;
        }
        return strncmp(m_start, rhs.m_start, rhs.m_size) < 0;
    }

    int StringRef::compare( StringRef rhs ) const {
        auto cmpResult =
            strncmp( m_start, rhs.m_start, std::min( m_size, rhs.m_size ) );

        // This means that strncmp found a difference before the strings
        // ended, and we can return it directly
        if ( cmpResult != 0 ) {
            return cmpResult;
        }

        // If strings are equal up to length, then their comparison results on
        // their size
        if ( m_size < rhs.m_size ) {
            return -1;
        } else if ( m_size > rhs.m_size ) {
            return 1;
        } else {
            return 0;
        }
    }

    auto operator << ( std::ostream& os, StringRef str ) -> std::ostream& {
        return os.write(str.data(), static_cast<std::streamsize>(str.size()));
    }

    std::string operator+(StringRef lhs, StringRef rhs) {
        std::string ret;
        ret.reserve(lhs.size() + rhs.size());
        ret += lhs;
        ret += rhs;
        return ret;
    }

    auto operator+=( std::string& lhs, StringRef rhs ) -> std::string& {
        lhs.append(rhs.data(), rhs.size());
        return lhs;
    }

} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_STRINGREF_CPP_INCLUDED
// END Amalgamated content from catch_stringref.cpp (@wolfram77)

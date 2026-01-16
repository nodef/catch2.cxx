//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_JSONWRITER_HPP_INCLUDED
#define CATCH_JSONWRITER_HPP_INCLUDED

#include "catch_lifetimebound.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reusable_string_stream.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_stringref.hpp"  // Adjust to relative path (@wolfram77)

#include <cstdint>
#include <sstream>

namespace Catch {
    class JsonObjectWriter;
    class JsonArrayWriter;

    struct JsonUtils {
        static void indent( std::ostream& os, std::uint64_t level );
        static void appendCommaNewline( std::ostream& os,
                                        bool& should_comma,
                                        std::uint64_t level );
    };

    class JsonValueWriter {
    public:
        JsonValueWriter( std::ostream& os CATCH_ATTR_LIFETIMEBOUND );
        JsonValueWriter( std::ostream& os CATCH_ATTR_LIFETIMEBOUND, std::uint64_t indent_level );

        JsonObjectWriter writeObject() &&;
        JsonArrayWriter writeArray() &&;

        template <typename T>
        void write( T const& value ) && {
            writeImpl( value, !std::is_arithmetic<T>::value );
        }
        void write( StringRef value ) &&;
        void write( bool value ) &&;

    private:
        void writeImpl( StringRef value, bool quote );

        // Without this SFINAE, this overload is a better match
        // for `std::string`, `char const*`, `char const[N]` args.
        // While it would still work, it would cause code bloat
        // and multiple iteration over the strings
        template <typename T,
                  typename = typename std::enable_if_t<
                      !std::is_convertible<T, StringRef>::value>>
        void writeImpl( T const& value, bool quote_value ) {
            m_sstream << value;
            writeImpl( m_sstream.str(), quote_value );
        }

        std::ostream& m_os;
        std::stringstream m_sstream;
        std::uint64_t m_indent_level;
    };

    class JsonObjectWriter {
    public:
        JsonObjectWriter( std::ostream& os CATCH_ATTR_LIFETIMEBOUND );
        JsonObjectWriter( std::ostream& os CATCH_ATTR_LIFETIMEBOUND, std::uint64_t indent_level );

        JsonObjectWriter( JsonObjectWriter&& source ) noexcept;
        JsonObjectWriter& operator=( JsonObjectWriter&& source ) = delete;

        ~JsonObjectWriter();

        JsonValueWriter write( StringRef key );

    private:
        std::ostream& m_os;
        std::uint64_t m_indent_level;
        bool m_should_comma = false;
        bool m_active = true;
    };

    class JsonArrayWriter {
    public:
        JsonArrayWriter( std::ostream& os CATCH_ATTR_LIFETIMEBOUND );
        JsonArrayWriter( std::ostream& os CATCH_ATTR_LIFETIMEBOUND, std::uint64_t indent_level );

        JsonArrayWriter( JsonArrayWriter&& source ) noexcept;
        JsonArrayWriter& operator=( JsonArrayWriter&& source ) = delete;

        ~JsonArrayWriter();

        JsonObjectWriter writeObject();
        JsonArrayWriter writeArray();

        template <typename T>
        JsonArrayWriter& write( T const& value ) {
            return writeImpl( value );
        }

        JsonArrayWriter& write( bool value );

    private:
        template <typename T>
        JsonArrayWriter& writeImpl( T const& value ) {
            JsonUtils::appendCommaNewline(
                m_os, m_should_comma, m_indent_level + 1 );
            JsonValueWriter{ m_os }.write( value );

            return *this;
        }

        std::ostream& m_os;
        std::uint64_t m_indent_level;
        bool m_should_comma = false;
        bool m_active = true;
    };

} // namespace Catch

#endif // CATCH_JSONWRITER_HPP_INCLUDED



// BEGIN Amalgamated content from catch_jsonwriter.cpp (@wolfram77)
#ifndef CATCH_JSONWRITER_CPP_INCLUDED
#define CATCH_JSONWRITER_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
// #include "catch_jsonwriter.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_unreachable.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    namespace {
        static bool needsEscape( char c ) {
            return c == '"' || c == '\\' || c == '\b' || c == '\f' ||
                   c == '\n' || c == '\r' || c == '\t';
        }

        static Catch::StringRef makeEscapeStringRef( char c ) {
            if ( c == '"' ) {
                return "\\\""_sr;
            } else if ( c == '\\' ) {
                return "\\\\"_sr;
            } else if ( c == '\b' ) {
                return "\\b"_sr;
            } else if ( c == '\f' ) {
                return "\\f"_sr;
            } else if ( c == '\n' ) {
                return "\\n"_sr;
            } else if ( c == '\r' ) {
                return "\\r"_sr;
            } else if ( c == '\t' ) {
                return "\\t"_sr;
            }
            Catch::Detail::Unreachable();
        }
    } // namespace

    void JsonUtils::indent( std::ostream& os, std::uint64_t level ) {
        for ( std::uint64_t i = 0; i < level; ++i ) {
            os << "  ";
        }
    }
    void JsonUtils::appendCommaNewline( std::ostream& os,
                                        bool& should_comma,
                                        std::uint64_t level ) {
        if ( should_comma ) { os << ','; }
        should_comma = true;
        os << '\n';
        indent( os, level );
    }

    JsonObjectWriter::JsonObjectWriter( std::ostream& os ):
        JsonObjectWriter{ os, 0 } {}

    JsonObjectWriter::JsonObjectWriter( std::ostream& os,
                                        std::uint64_t indent_level ):
        m_os{ os }, m_indent_level{ indent_level } {
        m_os << '{';
    }
    JsonObjectWriter::JsonObjectWriter( JsonObjectWriter&& source ) noexcept:
        m_os{ source.m_os },
        m_indent_level{ source.m_indent_level },
        m_should_comma{ source.m_should_comma },
        m_active{ source.m_active } {
        source.m_active = false;
    }

    JsonObjectWriter::~JsonObjectWriter() {
        if ( !m_active ) { return; }

        m_os << '\n';
        JsonUtils::indent( m_os, m_indent_level );
        m_os << '}';
    }

    JsonValueWriter JsonObjectWriter::write( StringRef key ) {
        JsonUtils::appendCommaNewline(
            m_os, m_should_comma, m_indent_level + 1 );

        m_os << '"' << key << "\": ";
        return JsonValueWriter{ m_os, m_indent_level + 1 };
    }

    JsonArrayWriter::JsonArrayWriter( std::ostream& os ):
        JsonArrayWriter{ os, 0 } {}
    JsonArrayWriter::JsonArrayWriter( std::ostream& os,
                                      std::uint64_t indent_level ):
        m_os{ os }, m_indent_level{ indent_level } {
        m_os << '[';
    }
    JsonArrayWriter::JsonArrayWriter( JsonArrayWriter&& source ) noexcept:
        m_os{ source.m_os },
        m_indent_level{ source.m_indent_level },
        m_should_comma{ source.m_should_comma },
        m_active{ source.m_active } {
        source.m_active = false;
    }
    JsonArrayWriter::~JsonArrayWriter() {
        if ( !m_active ) { return; }

        m_os << '\n';
        JsonUtils::indent( m_os, m_indent_level );
        m_os << ']';
    }

    JsonObjectWriter JsonArrayWriter::writeObject() {
        JsonUtils::appendCommaNewline(
            m_os, m_should_comma, m_indent_level + 1 );
        return JsonObjectWriter{ m_os, m_indent_level + 1 };
    }

    JsonArrayWriter JsonArrayWriter::writeArray() {
        JsonUtils::appendCommaNewline(
            m_os, m_should_comma, m_indent_level + 1 );
        return JsonArrayWriter{ m_os, m_indent_level + 1 };
    }

    JsonArrayWriter& JsonArrayWriter::write( bool value ) {
        return writeImpl( value );
    }

    JsonValueWriter::JsonValueWriter( std::ostream& os ):
        JsonValueWriter{ os, 0 } {}

    JsonValueWriter::JsonValueWriter( std::ostream& os,
                                      std::uint64_t indent_level ):
        m_os{ os }, m_indent_level{ indent_level } {}

    JsonObjectWriter JsonValueWriter::writeObject() && {
        return JsonObjectWriter{ m_os, m_indent_level };
    }

    JsonArrayWriter JsonValueWriter::writeArray() && {
        return JsonArrayWriter{ m_os, m_indent_level };
    }

    void JsonValueWriter::write( Catch::StringRef value ) && {
        writeImpl( value, true );
    }

    void JsonValueWriter::write( bool value ) && {
        writeImpl( value ? "true"_sr : "false"_sr, false );
    }

    void JsonValueWriter::writeImpl( Catch::StringRef value, bool quote ) {
        if ( quote ) { m_os << '"'; }
        size_t current_start = 0;
        for ( size_t i = 0; i < value.size(); ++i ) {
            if ( needsEscape( value[i] ) ) {
                if ( current_start < i ) {
                    m_os << value.substr( current_start, i - current_start );
                }
                m_os << makeEscapeStringRef( value[i] );
                current_start = i + 1;
            }
        }
        if ( current_start < value.size() ) {
            m_os << value.substr( current_start, value.size() - current_start );
        }
        if ( quote ) { m_os << '"'; }
    }

} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_JSONWRITER_CPP_INCLUDED
// END Amalgamated content from catch_jsonwriter.cpp (@wolfram77)

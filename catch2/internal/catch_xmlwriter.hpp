//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_XMLWRITER_HPP_INCLUDED
#define CATCH_XMLWRITER_HPP_INCLUDED

#include "catch_lifetimebound.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_reusable_string_stream.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_stringref.hpp"  // Adjust to relative path (@wolfram77)

#include <iosfwd>
#include <vector>
#include <cstdint>

namespace Catch {
    enum class XmlFormatting : std::uint8_t {
        None = 0x00,
        Indent = 0x01,
        Newline = 0x02,
    };

    constexpr XmlFormatting operator|( XmlFormatting lhs, XmlFormatting rhs ) {
        return static_cast<XmlFormatting>( static_cast<std::uint8_t>( lhs ) |
                                           static_cast<std::uint8_t>( rhs ) );
    }

    constexpr XmlFormatting operator&( XmlFormatting lhs, XmlFormatting rhs ) {
        return static_cast<XmlFormatting>( static_cast<std::uint8_t>( lhs ) &
                                           static_cast<std::uint8_t>( rhs ) );
    }


    /**
     * Helper for XML-encoding text (escaping angle brackets, quotes, etc)
     *
     * Note: doesn't take ownership of passed strings, and thus the
     *       encoded string must outlive the encoding instance.
     */
    class XmlEncode {
    public:
        enum ForWhat { ForTextNodes, ForAttributes };

        constexpr XmlEncode( StringRef str CATCH_ATTR_LIFETIMEBOUND, ForWhat forWhat = ForTextNodes ):
            m_str( str ), m_forWhat( forWhat ) {}


        void encodeTo( std::ostream& os ) const;

        friend std::ostream& operator << ( std::ostream& os, XmlEncode const& xmlEncode );

    private:
        StringRef m_str;
        ForWhat m_forWhat;
    };

    class XmlWriter {
    public:

        class ScopedElement {
        public:
            ScopedElement( XmlWriter* writer CATCH_ATTR_LIFETIMEBOUND, XmlFormatting fmt );

            ScopedElement( ScopedElement&& other ) noexcept;
            ScopedElement& operator=( ScopedElement&& other ) noexcept;

            ~ScopedElement();

            ScopedElement&
            writeText( StringRef text,
                       XmlFormatting fmt = XmlFormatting::Newline |
                                           XmlFormatting::Indent );

            ScopedElement& writeAttribute( StringRef name,
                                           StringRef attribute );
            template <typename T,
                      // Without this SFINAE, this overload is a better match
                      // for `std::string`, `char const*`, `char const[N]` args.
                      // While it would still work, it would cause code bloat
                      // and multiple iteration over the strings
                      typename = typename std::enable_if_t<
                          !std::is_convertible<T, StringRef>::value>>
            ScopedElement& writeAttribute( StringRef name,
                                           T const& attribute ) {
                m_writer->writeAttribute( name, attribute );
                return *this;
            }

        private:
            XmlWriter* m_writer = nullptr;
            XmlFormatting m_fmt;
        };

        XmlWriter( std::ostream& os CATCH_ATTR_LIFETIMEBOUND );
        ~XmlWriter();

        XmlWriter( XmlWriter const& ) = delete;
        XmlWriter& operator=( XmlWriter const& ) = delete;

        XmlWriter& startElement( std::string const& name, XmlFormatting fmt = XmlFormatting::Newline | XmlFormatting::Indent);

        ScopedElement scopedElement( std::string const& name, XmlFormatting fmt = XmlFormatting::Newline | XmlFormatting::Indent);

        XmlWriter& endElement(XmlFormatting fmt = XmlFormatting::Newline | XmlFormatting::Indent);

        //! The attribute content is XML-encoded
        XmlWriter& writeAttribute( StringRef name, StringRef attribute );

        //! Writes the attribute as "true/false"
        XmlWriter& writeAttribute( StringRef name, bool attribute );

        //! The attribute content is XML-encoded
        XmlWriter& writeAttribute( StringRef name, char const* attribute );

        //! The attribute value must provide op<<(ostream&, T). The resulting
        //! serialization is XML-encoded
        template <typename T,
                  // Without this SFINAE, this overload is a better match
                  // for `std::string`, `char const*`, `char const[N]` args.
                  // While it would still work, it would cause code bloat
                  // and multiple iteration over the strings
                  typename = typename std::enable_if_t<
                      !std::is_convertible<T, StringRef>::value>>
        XmlWriter& writeAttribute( StringRef name, T const& attribute ) {
            ReusableStringStream rss;
            rss << attribute;
            return writeAttribute( name, rss.str() );
        }

        //! Writes escaped `text` in a element
        XmlWriter& writeText( StringRef text,
                              XmlFormatting fmt = XmlFormatting::Newline |
                                                  XmlFormatting::Indent );

        //! Writes XML comment as "<!-- text -->"
        XmlWriter& writeComment( StringRef text,
                                 XmlFormatting fmt = XmlFormatting::Newline |
                                                     XmlFormatting::Indent );

        void writeStylesheetRef( StringRef url );

        void ensureTagClosed();

    private:

        void applyFormatting(XmlFormatting fmt);

        void writeDeclaration();

        void newlineIfNecessary();

        bool m_tagIsOpen = false;
        bool m_needsNewline = false;
        std::vector<std::string> m_tags;
        std::string m_indent;
        std::ostream& m_os;
    };

}

#endif // CATCH_XMLWRITER_HPP_INCLUDED



// BEGIN Amalgamated content from catch_xmlwriter.cpp (@wolfram77)
#ifndef CATCH_XMLWRITER_CPP_INCLUDED
#define CATCH_XMLWRITER_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Note: swapping these two includes around causes MSVC to error out
//       while in /permissive- mode. No, I don't know why.
//       Tested on VS 2019, 18.{3, 4}.x
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
// #include "catch_xmlwriter.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include <cstdint>
#include <iomanip>
#include <type_traits>

namespace Catch {

namespace {

    size_t trailingBytes(unsigned char c) {
        if ((c & 0xE0) == 0xC0) {
            return 2;
        }
        if ((c & 0xF0) == 0xE0) {
            return 3;
        }
        if ((c & 0xF8) == 0xF0) {
            return 4;
        }
        CATCH_INTERNAL_ERROR("Invalid multibyte utf-8 start byte encountered");
    }

    uint32_t headerValue(unsigned char c) {
        if ((c & 0xE0) == 0xC0) {
            return c & 0x1F;
        }
        if ((c & 0xF0) == 0xE0) {
            return c & 0x0F;
        }
        if ((c & 0xF8) == 0xF0) {
            return c & 0x07;
        }
        CATCH_INTERNAL_ERROR("Invalid multibyte utf-8 start byte encountered");
    }

    void hexEscapeChar(std::ostream& os, unsigned char c) {
        std::ios_base::fmtflags f(os.flags());
        os << "\\x"_sr
            << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(c);
        os.flags(f);
    }

    constexpr bool shouldNewline(XmlFormatting fmt) {
        return !!(static_cast<std::underlying_type_t<XmlFormatting>>(fmt & XmlFormatting::Newline));
    }

    constexpr bool shouldIndent(XmlFormatting fmt) {
        return !!(static_cast<std::underlying_type_t<XmlFormatting>>(fmt & XmlFormatting::Indent));
    }

} // anonymous namespace

    void XmlEncode::encodeTo( std::ostream& os ) const {
        // Apostrophe escaping not necessary if we always use " to write attributes
        // (see: http://www.w3.org/TR/xml/#syntax)
        size_t last_start = 0;
        auto write_to = [&]( size_t idx ) {
            if ( last_start < idx ) {
                os << m_str.substr( last_start, idx - last_start );
            }
            last_start = idx + 1;
        };

        for ( std::size_t idx = 0; idx < m_str.size(); ++idx ) {
            unsigned char c = static_cast<unsigned char>( m_str[idx] );
            switch ( c ) {
            case '<':
                write_to( idx );
                os << "&lt;"_sr;
                break;
            case '&':
                write_to( idx );
                os << "&amp;"_sr;
                break;

            case '>':
                // See: http://www.w3.org/TR/xml/#syntax
                if ( idx > 2 && m_str[idx - 1] == ']' && m_str[idx - 2] == ']' ) {
                    write_to( idx );
                    os << "&gt;"_sr;
                }
                break;

            case '\"':
                if ( m_forWhat == ForAttributes ) {
                    write_to( idx );
                    os << "&quot;"_sr;
                }
                break;

            default:
                // Check for control characters and invalid utf-8

                // Escape control characters in standard ascii
                // see
                // http://stackoverflow.com/questions/404107/why-are-control-characters-illegal-in-xml-1-0
                if ( c < 0x09 || ( c > 0x0D && c < 0x20 ) || c == 0x7F ) {
                    write_to( idx );
                    hexEscapeChar( os, c );
                    break;
                }

                // Plain ASCII: Write it to stream
                if ( c < 0x7F ) {
                    break;
                }

                // UTF-8 territory
                // Check if the encoding is valid and if it is not, hex escape
                // bytes. Important: We do not check the exact decoded values for
                // validity, only the encoding format First check that this bytes is
                // a valid lead byte: This means that it is not encoded as 1111 1XXX
                // Or as 10XX XXXX
                if ( c < 0xC0 || c >= 0xF8 ) {
                    write_to( idx );
                    hexEscapeChar( os, c );
                    break;
                }

                auto encBytes = trailingBytes( c );
                // Are there enough bytes left to avoid accessing out-of-bounds
                // memory?
                if ( idx + encBytes - 1 >= m_str.size() ) {
                    write_to( idx );
                    hexEscapeChar( os, c );
                    break;
                }
                // The header is valid, check data
                // The next encBytes bytes must together be a valid utf-8
                // This means: bitpattern 10XX XXXX and the extracted value is sane
                // (ish)
                bool valid = true;
                uint32_t value = headerValue( c );
                for ( std::size_t n = 1; n < encBytes; ++n ) {
                    unsigned char nc = static_cast<unsigned char>( m_str[idx + n] );
                    valid &= ( ( nc & 0xC0 ) == 0x80 );
                    value = ( value << 6 ) | ( nc & 0x3F );
                }

                if (
                    // Wrong bit pattern of following bytes
                    ( !valid ) ||
                    // Overlong encodings
                    ( value < 0x80 ) ||
                    ( 0x80 <= value && value < 0x800 && encBytes > 2 ) ||
                    ( 0x800 < value && value < 0x10000 && encBytes > 3 ) ||
                    // Encoded value out of range
                    ( value >= 0x110000 ) ) {
                    write_to( idx );
                    hexEscapeChar( os, c );
                    break;
                }

                // If we got here, this is in fact a valid(ish) utf-8 sequence
                idx += encBytes - 1;
                break;
            }
        }

        write_to( m_str.size() );
    }

    std::ostream& operator << ( std::ostream& os, XmlEncode const& xmlEncode ) {
        xmlEncode.encodeTo( os );
        return os;
    }

    XmlWriter::ScopedElement::ScopedElement( XmlWriter* writer, XmlFormatting fmt )
    :   m_writer( writer ),
        m_fmt(fmt)
    {}

    XmlWriter::ScopedElement::ScopedElement( ScopedElement&& other ) noexcept
    :   m_writer( other.m_writer ),
        m_fmt(other.m_fmt)
    {
        other.m_writer = nullptr;
        other.m_fmt = XmlFormatting::None;
    }
    XmlWriter::ScopedElement& XmlWriter::ScopedElement::operator=( ScopedElement&& other ) noexcept {
        if ( m_writer ) {
            m_writer->endElement();
        }
        m_writer = other.m_writer;
        other.m_writer = nullptr;
        m_fmt = other.m_fmt;
        other.m_fmt = XmlFormatting::None;
        return *this;
    }


    XmlWriter::ScopedElement::~ScopedElement() {
        if (m_writer) {
            m_writer->endElement(m_fmt);
        }
    }

    XmlWriter::ScopedElement&
    XmlWriter::ScopedElement::writeText( StringRef text, XmlFormatting fmt ) {
        m_writer->writeText( text, fmt );
        return *this;
    }

    XmlWriter::ScopedElement&
    XmlWriter::ScopedElement::writeAttribute( StringRef name,
                                              StringRef attribute ) {
        m_writer->writeAttribute( name, attribute );
        return *this;
    }


    XmlWriter::XmlWriter( std::ostream& os ) : m_os( os )
    {
        writeDeclaration();
    }

    XmlWriter::~XmlWriter() {
        while (!m_tags.empty()) {
            endElement();
        }
        newlineIfNecessary();
    }

    XmlWriter& XmlWriter::startElement( std::string const& name, XmlFormatting fmt ) {
        ensureTagClosed();
        newlineIfNecessary();
        if (shouldIndent(fmt)) {
            m_os << m_indent;
            m_indent += "  ";
        }
        m_os << '<' << name;
        m_tags.push_back( name );
        m_tagIsOpen = true;
        applyFormatting(fmt);
        return *this;
    }

    XmlWriter::ScopedElement XmlWriter::scopedElement( std::string const& name, XmlFormatting fmt ) {
        ScopedElement scoped( this, fmt );
        startElement( name, fmt );
        return scoped;
    }

    XmlWriter& XmlWriter::endElement(XmlFormatting fmt) {
        m_indent = m_indent.substr(0, m_indent.size() - 2);

        if( m_tagIsOpen ) {
            m_os << "/>";
            m_tagIsOpen = false;
        } else {
            newlineIfNecessary();
            if (shouldIndent(fmt)) {
                m_os << m_indent;
            }
            m_os << "</" << m_tags.back() << '>';
        }
        m_os << std::flush;
        applyFormatting(fmt);
        m_tags.pop_back();
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( StringRef name,
                                          StringRef attribute ) {
        if( !name.empty() && !attribute.empty() )
            m_os << ' ' << name << "=\"" << XmlEncode( attribute, XmlEncode::ForAttributes ) << '"';
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( StringRef name, bool attribute ) {
        writeAttribute(name, (attribute ? "true"_sr : "false"_sr));
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( StringRef name,
                                          char const* attribute ) {
        writeAttribute( name, StringRef( attribute ) );
        return *this;
    }

    XmlWriter& XmlWriter::writeText( StringRef text, XmlFormatting fmt ) {
        CATCH_ENFORCE(!m_tags.empty(), "Cannot write text as top level element");
        if( !text.empty() ){
            bool tagWasOpen = m_tagIsOpen;
            ensureTagClosed();
            if (tagWasOpen && shouldIndent(fmt)) {
                m_os << m_indent;
            }
            m_os << XmlEncode( text, XmlEncode::ForTextNodes );
            applyFormatting(fmt);
        }
        return *this;
    }

    XmlWriter& XmlWriter::writeComment( StringRef text, XmlFormatting fmt ) {
        ensureTagClosed();
        if (shouldIndent(fmt)) {
            m_os << m_indent;
        }
        m_os << "<!-- " << text << " -->";
        applyFormatting(fmt);
        return *this;
    }

    void XmlWriter::writeStylesheetRef( StringRef url ) {
        m_os << R"(<?xml-stylesheet type="text/xsl" href=")" << url << R"("?>)" << '\n';
    }

    void XmlWriter::ensureTagClosed() {
        if( m_tagIsOpen ) {
            m_os << '>' << std::flush;
            newlineIfNecessary();
            m_tagIsOpen = false;
        }
    }

    void XmlWriter::applyFormatting(XmlFormatting fmt) {
        m_needsNewline = shouldNewline(fmt);
    }

    void XmlWriter::writeDeclaration() {
        m_os << R"(<?xml version="1.0" encoding="UTF-8"?>)" << '\n';
    }

    void XmlWriter::newlineIfNecessary() {
        if( m_needsNewline ) {
            m_os << '\n' << std::flush;
            m_needsNewline = false;
        }
    }
}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_XMLWRITER_CPP_INCLUDED
// END Amalgamated content from catch_xmlwriter.cpp (@wolfram77)

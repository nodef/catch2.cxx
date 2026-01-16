//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEXTFLOW_HPP_INCLUDED
#define CATCH_TEXTFLOW_HPP_INCLUDED

#include "catch_console_width.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <cassert>
#include <string>
#include <vector>

namespace Catch {
    namespace TextFlow {

        class Columns;

        /**
         * Abstraction for a string with ansi escape sequences that
         * automatically skips over escapes when iterating. Only graphical
         * escape sequences are considered.
         *
         * Internal representation:
         * An escape sequence looks like \033[39;49m
         * We need bidirectional iteration and the unbound length of escape
         * sequences poses a problem for operator-- To make this work we'll
         * replace the last `m` with a 0xff (this is a codepoint that won't have
         * any utf-8 meaning).
         */
        class AnsiSkippingString {
            std::string m_string;
            std::size_t m_size = 0;

            // perform 0xff replacement and calculate m_size
            void preprocessString();

        public:
            class const_iterator;
            using iterator = const_iterator;
            // note: must be u-suffixed or this will cause a "truncation of
            // constant value" warning on MSVC
            static constexpr char sentinel = static_cast<char>( 0xffu );

            explicit AnsiSkippingString( std::string const& text );
            explicit AnsiSkippingString( std::string&& text );

            const_iterator begin() const;
            const_iterator end() const;

            size_t size() const { return m_size; }

            std::string substring( const_iterator begin,
                                   const_iterator end ) const;
        };

        class AnsiSkippingString::const_iterator {
            friend AnsiSkippingString;
            struct EndTag {};

            const std::string* m_string;
            std::string::const_iterator m_it;

            explicit const_iterator( const std::string& string, EndTag ):
                m_string( &string ), m_it( string.end() ) {}

            void tryParseAnsiEscapes();
            void advance();
            void unadvance();

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = char;
            using pointer = value_type*;
            using reference = value_type&;
            using iterator_category = std::bidirectional_iterator_tag;

            explicit const_iterator( const std::string& string ):
                m_string( &string ), m_it( string.begin() ) {
                tryParseAnsiEscapes();
            }

            char operator*() const { return *m_it; }

            const_iterator& operator++() {
                advance();
                return *this;
            }
            const_iterator operator++( int ) {
                iterator prev( *this );
                operator++();
                return prev;
            }
            const_iterator& operator--() {
                unadvance();
                return *this;
            }
            const_iterator operator--( int ) {
                iterator prev( *this );
                operator--();
                return prev;
            }

            bool operator==( const_iterator const& other ) const {
                return m_it == other.m_it;
            }
            bool operator!=( const_iterator const& other ) const {
                return !operator==( other );
            }
            bool operator<=( const_iterator const& other ) const {
                return m_it <= other.m_it;
            }

            const_iterator oneBefore() const {
                auto it = *this;
                return --it;
            }
        };

        /**
         * Represents a column of text with specific width and indentation
         *
         * When written out to a stream, it will perform linebreaking
         * of the provided text so that the written lines fit within
         * target width.
         */
        class Column {
            // String to be written out
            AnsiSkippingString m_string;
            // Width of the column for linebreaking
            size_t m_width = CATCH_CONFIG_CONSOLE_WIDTH - 1;
            // Indentation of other lines (including first if initial indent is
            // unset)
            size_t m_indent = 0;
            // Indentation of the first line
            size_t m_initialIndent = std::string::npos;

        public:
            /**
             * Iterates "lines" in `Column` and returns them
             */
            class const_iterator {
                friend Column;
                struct EndTag {};

                Column const& m_column;
                // Where does the current line start?
                AnsiSkippingString::const_iterator m_lineStart;
                // How long should the current line be?
                AnsiSkippingString::const_iterator m_lineEnd;
                // How far have we checked the string to iterate?
                AnsiSkippingString::const_iterator m_parsedTo;
                // Should a '-' be appended to the line?
                bool m_addHyphen = false;

                const_iterator( Column const& column, EndTag ):
                    m_column( column ),
                    m_lineStart( m_column.m_string.end() ),
                    m_lineEnd( column.m_string.end() ),
                    m_parsedTo( column.m_string.end() ) {}

                // Calculates the length of the current line
                void calcLength();

                // Returns current indentation width
                size_t indentSize() const;

                // Creates an indented and (optionally) suffixed string from
                // current iterator position, indentation and length.
                std::string addIndentAndSuffix(
                    AnsiSkippingString::const_iterator start,
                    AnsiSkippingString::const_iterator end ) const;

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = std::string;
                using pointer = value_type*;
                using reference = value_type&;
                using iterator_category = std::forward_iterator_tag;

                explicit const_iterator( Column const& column );

                std::string operator*() const;

                const_iterator& operator++();
                const_iterator operator++( int );

                bool operator==( const_iterator const& other ) const {
                    return m_lineStart == other.m_lineStart &&
                           &m_column == &other.m_column;
                }
                bool operator!=( const_iterator const& other ) const {
                    return !operator==( other );
                }
            };
            using iterator = const_iterator;

            explicit Column( std::string const& text ): m_string( text ) {}
            explicit Column( std::string&& text ):
                m_string( CATCH_MOVE( text ) ) {}

            Column& width( size_t newWidth ) & {
                assert( newWidth > 0 );
                m_width = newWidth;
                return *this;
            }
            Column&& width( size_t newWidth ) && {
                assert( newWidth > 0 );
                m_width = newWidth;
                return CATCH_MOVE( *this );
            }
            Column& indent( size_t newIndent ) & {
                m_indent = newIndent;
                return *this;
            }
            Column&& indent( size_t newIndent ) && {
                m_indent = newIndent;
                return CATCH_MOVE( *this );
            }
            Column& initialIndent( size_t newIndent ) & {
                m_initialIndent = newIndent;
                return *this;
            }
            Column&& initialIndent( size_t newIndent ) && {
                m_initialIndent = newIndent;
                return CATCH_MOVE( *this );
            }

            size_t width() const { return m_width; }
            const_iterator begin() const { return const_iterator( *this ); }
            const_iterator end() const {
                return { *this, const_iterator::EndTag{} };
            }

            friend std::ostream& operator<<( std::ostream& os,
                                             Column const& col );

            friend Columns operator+( Column const& lhs, Column const& rhs );
            friend Columns operator+( Column&& lhs, Column&& rhs );
        };

        //! Creates a column that serves as an empty space of specific width
        Column Spacer( size_t spaceWidth );

        class Columns {
            std::vector<Column> m_columns;

        public:
            class iterator {
                friend Columns;
                struct EndTag {};

                std::vector<Column> const& m_columns;
                std::vector<Column::const_iterator> m_iterators;
                size_t m_activeIterators;

                iterator( Columns const& columns, EndTag );

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = std::string;
                using pointer = value_type*;
                using reference = value_type&;
                using iterator_category = std::forward_iterator_tag;

                explicit iterator( Columns const& columns );

                auto operator==( iterator const& other ) const -> bool {
                    return m_iterators == other.m_iterators;
                }
                auto operator!=( iterator const& other ) const -> bool {
                    return m_iterators != other.m_iterators;
                }
                std::string operator*() const;
                iterator& operator++();
                iterator operator++( int );
            };
            using const_iterator = iterator;

            iterator begin() const { return iterator( *this ); }
            iterator end() const { return { *this, iterator::EndTag() }; }

            friend Columns& operator+=( Columns& lhs, Column const& rhs );
            friend Columns& operator+=( Columns& lhs, Column&& rhs );
            friend Columns operator+( Columns const& lhs, Column const& rhs );
            friend Columns operator+( Columns&& lhs, Column&& rhs );

            friend std::ostream& operator<<( std::ostream& os,
                                             Columns const& cols );
        };

    } // namespace TextFlow
} // namespace Catch
#endif // CATCH_TEXTFLOW_HPP_INCLUDED



// BEGIN Amalgamated content from catch_textflow.cpp (@wolfram77)
#ifndef CATCH_TEXTFLOW_CPP_INCLUDED
#define CATCH_TEXTFLOW_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_textflow.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#include <algorithm>
#include <cstring>
#include <ostream>

namespace {
    bool isWhitespace( char c ) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    bool isBreakableBefore( char c ) {
        static const char chars[] = "[({<|";
        return std::memchr( chars, c, sizeof( chars ) - 1 ) != nullptr;
    }

    bool isBreakableAfter( char c ) {
        static const char chars[] = "])}>.,:;*+-=&/\\";
        return std::memchr( chars, c, sizeof( chars ) - 1 ) != nullptr;
    }

} // namespace

namespace Catch {
    namespace TextFlow {
        void AnsiSkippingString::preprocessString() {
            for ( auto it = m_string.begin(); it != m_string.end(); ) {
                // try to read through an ansi sequence
                while ( it != m_string.end() && *it == '\033' &&
                        it + 1 != m_string.end() && *( it + 1 ) == '[' ) {
                    auto cursor = it + 2;
                    while ( cursor != m_string.end() &&
                            ( isdigit( *cursor ) || *cursor == ';' ) ) {
                        ++cursor;
                    }
                    if ( cursor == m_string.end() || *cursor != 'm' ) {
                        break;
                    }
                    // 'm' -> 0xff
                    *cursor = AnsiSkippingString::sentinel;
                    // if we've read an ansi sequence, set the iterator and
                    // return to the top of the loop
                    it = cursor + 1;
                }
                if ( it != m_string.end() ) {
                    ++m_size;
                    ++it;
                }
            }
        }

        AnsiSkippingString::AnsiSkippingString( std::string const& text ):
            m_string( text ) {
            preprocessString();
        }

        AnsiSkippingString::AnsiSkippingString( std::string&& text ):
            m_string( CATCH_MOVE( text ) ) {
            preprocessString();
        }

        AnsiSkippingString::const_iterator AnsiSkippingString::begin() const {
            return const_iterator( m_string );
        }

        AnsiSkippingString::const_iterator AnsiSkippingString::end() const {
            return const_iterator( m_string, const_iterator::EndTag{} );
        }

        std::string AnsiSkippingString::substring( const_iterator begin,
                                                   const_iterator end ) const {
            // There's one caveat here to an otherwise simple substring: when
            // making a begin iterator we might have skipped ansi sequences at
            // the start. If `begin` here is a begin iterator, skipped over
            // initial ansi sequences, we'll use the true beginning of the
            // string. Lastly: We need to transform any chars we replaced with
            // 0xff back to 'm'
            auto str = std::string( begin == this->begin() ? m_string.begin()
                                                           : begin.m_it,
                                    end.m_it );
            std::transform( str.begin(), str.end(), str.begin(), []( char c ) {
                return c == AnsiSkippingString::sentinel ? 'm' : c;
            } );
            return str;
        }

        void AnsiSkippingString::const_iterator::tryParseAnsiEscapes() {
            // check if we've landed on an ansi sequence, and if so read through
            // it
            while ( m_it != m_string->end() && *m_it == '\033' &&
                    m_it + 1 != m_string->end() &&  *( m_it + 1 ) == '[' ) {
                auto cursor = m_it + 2;
                while ( cursor != m_string->end() &&
                        ( isdigit( *cursor ) || *cursor == ';' ) ) {
                    ++cursor;
                }
                if ( cursor == m_string->end() ||
                     *cursor != AnsiSkippingString::sentinel ) {
                    break;
                }
                // if we've read an ansi sequence, set the iterator and
                // return to the top of the loop
                m_it = cursor + 1;
            }
        }

        void AnsiSkippingString::const_iterator::advance() {
            assert( m_it != m_string->end() );
            m_it++;
            tryParseAnsiEscapes();
        }

        void AnsiSkippingString::const_iterator::unadvance() {
            assert( m_it != m_string->begin() );
            m_it--;
            // if *m_it is 0xff, scan back to the \033 and then m_it-- once more
            // (and repeat check)
            while ( *m_it == AnsiSkippingString::sentinel ) {
                while ( *m_it != '\033' ) {
                    assert( m_it != m_string->begin() );
                    m_it--;
                }
                // if this happens, we must have been a begin iterator that had
                // skipped over ansi sequences at the start of a string
                assert( m_it != m_string->begin() );
                assert( *m_it == '\033' );
                m_it--;
            }
        }

        static bool isBoundary( AnsiSkippingString const& line,
                                AnsiSkippingString::const_iterator it ) {
            return it == line.end() ||
                   ( isWhitespace( *it ) &&
                     !isWhitespace( *it.oneBefore() ) ) ||
                   isBreakableBefore( *it ) ||
                   isBreakableAfter( *it.oneBefore() );
        }

        void Column::const_iterator::calcLength() {
            m_addHyphen = false;
            m_parsedTo = m_lineStart;
            AnsiSkippingString const& current_line = m_column.m_string;

            if ( m_parsedTo == current_line.end() ) {
                m_lineEnd = m_parsedTo;
                return;
            }

            assert( m_lineStart != current_line.end() );
            if ( *m_lineStart == '\n' ) { ++m_parsedTo; }

            const auto maxLineLength = m_column.m_width - indentSize();
            std::size_t lineLength = 0;
            while ( m_parsedTo != current_line.end() &&
                    lineLength < maxLineLength && *m_parsedTo != '\n' ) {
                ++m_parsedTo;
                ++lineLength;
            }

            // If we encountered a newline before the column is filled,
            // then we linebreak at the newline and consider this line
            // finished.
            if ( lineLength < maxLineLength ) {
                m_lineEnd = m_parsedTo;
            } else {
                // Look for a natural linebreak boundary in the column
                // (We look from the end, so that the first found boundary is
                // the right one)
                m_lineEnd = m_parsedTo;
                while ( lineLength > 0 &&
                        !isBoundary( current_line, m_lineEnd ) ) {
                    --lineLength;
                    --m_lineEnd;
                }
                while ( lineLength > 0 &&
                        isWhitespace( *m_lineEnd.oneBefore() ) ) {
                    --lineLength;
                    --m_lineEnd;
                }

                // If we found one, then that is where we linebreak, otherwise
                // we have to split text with a hyphen
                if ( lineLength == 0 ) {
                    m_addHyphen = true;
                    m_lineEnd = m_parsedTo.oneBefore();
                }
            }
        }

        size_t Column::const_iterator::indentSize() const {
            auto initial = m_lineStart == m_column.m_string.begin()
                               ? m_column.m_initialIndent
                               : std::string::npos;
            return initial == std::string::npos ? m_column.m_indent : initial;
        }

        std::string Column::const_iterator::addIndentAndSuffix(
            AnsiSkippingString::const_iterator start,
            AnsiSkippingString::const_iterator end ) const {
            std::string ret;
            const auto desired_indent = indentSize();
            // ret.reserve( desired_indent + (end - start) + m_addHyphen );
            ret.append( desired_indent, ' ' );
            // ret.append( start, end );
            ret += m_column.m_string.substring( start, end );
            if ( m_addHyphen ) { ret.push_back( '-' ); }

            return ret;
        }

        Column::const_iterator::const_iterator( Column const& column ):
            m_column( column ),
            m_lineStart( column.m_string.begin() ),
            m_lineEnd( column.m_string.begin() ),
            m_parsedTo( column.m_string.begin() ) {
            assert( m_column.m_width > m_column.m_indent );
            assert( m_column.m_initialIndent == std::string::npos ||
                    m_column.m_width > m_column.m_initialIndent );
            calcLength();
            if ( m_lineStart == m_lineEnd ) {
                m_lineStart = m_column.m_string.end();
            }
        }

        std::string Column::const_iterator::operator*() const {
            assert( m_lineStart <= m_parsedTo );
            return addIndentAndSuffix( m_lineStart, m_lineEnd );
        }

        Column::const_iterator& Column::const_iterator::operator++() {
            m_lineStart = m_lineEnd;
            AnsiSkippingString const& current_line = m_column.m_string;
            if ( m_lineStart != current_line.end() && *m_lineStart == '\n' ) {
                m_lineStart++;
            } else {
                while ( m_lineStart != current_line.end() &&
                        isWhitespace( *m_lineStart ) ) {
                    ++m_lineStart;
                }
            }

            if ( m_lineStart != current_line.end() ) { calcLength(); }
            return *this;
        }

        Column::const_iterator Column::const_iterator::operator++( int ) {
            const_iterator prev( *this );
            operator++();
            return prev;
        }

        std::ostream& operator<<( std::ostream& os, Column const& col ) {
            bool first = true;
            for ( auto line : col ) {
                if ( first ) {
                    first = false;
                } else {
                    os << '\n';
                }
                os << line;
            }
            return os;
        }

        Column Spacer( size_t spaceWidth ) {
            Column ret{ "" };
            ret.width( spaceWidth );
            return ret;
        }

        Columns::iterator::iterator( Columns const& columns, EndTag ):
            m_columns( columns.m_columns ), m_activeIterators( 0 ) {

            m_iterators.reserve( m_columns.size() );
            for ( auto const& col : m_columns ) {
                m_iterators.push_back( col.end() );
            }
        }

        Columns::iterator::iterator( Columns const& columns ):
            m_columns( columns.m_columns ),
            m_activeIterators( m_columns.size() ) {

            m_iterators.reserve( m_columns.size() );
            for ( auto const& col : m_columns ) {
                m_iterators.push_back( col.begin() );
            }
        }

        std::string Columns::iterator::operator*() const {
            std::string row, padding;

            for ( size_t i = 0; i < m_columns.size(); ++i ) {
                const auto width = m_columns[i].width();
                if ( m_iterators[i] != m_columns[i].end() ) {
                    std::string col = *m_iterators[i];
                    row += padding;
                    row += col;

                    padding.clear();
                    if ( col.size() < width ) {
                        padding.append( width - col.size(), ' ' );
                    }
                } else {
                    padding.append( width, ' ' );
                }
            }
            return row;
        }

        Columns::iterator& Columns::iterator::operator++() {
            for ( size_t i = 0; i < m_columns.size(); ++i ) {
                if ( m_iterators[i] != m_columns[i].end() ) {
                    ++m_iterators[i];
                }
            }
            return *this;
        }

        Columns::iterator Columns::iterator::operator++( int ) {
            iterator prev( *this );
            operator++();
            return prev;
        }

        std::ostream& operator<<( std::ostream& os, Columns const& cols ) {
            bool first = true;
            for ( auto line : cols ) {
                if ( first ) {
                    first = false;
                } else {
                    os << '\n';
                }
                os << line;
            }
            return os;
        }

        Columns operator+( Column const& lhs, Column const& rhs ) {
            Columns cols;
            cols += lhs;
            cols += rhs;
            return cols;
        }
        Columns operator+( Column&& lhs, Column&& rhs ) {
            Columns cols;
            cols += CATCH_MOVE( lhs );
            cols += CATCH_MOVE( rhs );
            return cols;
        }

        Columns& operator+=( Columns& lhs, Column const& rhs ) {
            lhs.m_columns.push_back( rhs );
            return lhs;
        }
        Columns& operator+=( Columns& lhs, Column&& rhs ) {
            lhs.m_columns.push_back( CATCH_MOVE( rhs ) );
            return lhs;
        }
        Columns operator+( Columns const& lhs, Column const& rhs ) {
            auto combined( lhs );
            combined += rhs;
            return combined;
        }
        Columns operator+( Columns&& lhs, Column&& rhs ) {
            lhs += CATCH_MOVE( rhs );
            return CATCH_MOVE( lhs );
        }

    } // namespace TextFlow
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TEXTFLOW_CPP_INCLUDED
// END Amalgamated content from catch_textflow.cpp (@wolfram77)

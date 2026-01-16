//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MESSAGE_HPP_INCLUDED
#define CATCH_MESSAGE_HPP_INCLUDED

#include "internal/catch_config_prefix_messages.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_result_type.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_reusable_string_stream.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_stream_end_stop.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_message_info.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_tostring.hpp"  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_capture.hpp"  // Adjust to relative path (@wolfram77)

#include <string>
#include <vector>

namespace Catch {

    struct SourceLineInfo;
    class IResultCapture;

    struct MessageStream {

        template<typename T>
        MessageStream& operator << ( T const& value ) {
            m_stream << value;
            return *this;
        }

        ReusableStringStream m_stream;
    };

    struct MessageBuilder : MessageStream {
        MessageBuilder( StringRef macroName,
                        SourceLineInfo const& lineInfo,
                        ResultWas::OfType type ):
            m_info(macroName, lineInfo, type) {}

        template<typename T>
        MessageBuilder&& operator << ( T const& value ) && {
            m_stream << value;
            return CATCH_MOVE(*this);
        }

        MessageInfo m_info;
    };

    class ScopedMessage {
    public:
        explicit ScopedMessage( MessageBuilder&& builder );
        ScopedMessage( ScopedMessage& duplicate ) = delete;
        ScopedMessage( ScopedMessage&& old ) noexcept;
        ~ScopedMessage();

        unsigned int m_messageId;
        bool m_moved = false;
    };

    class Capturer {
        std::vector<MessageInfo> m_messages;
        size_t m_captured = 0;
        bool m_isScoped = false;
    public:
        Capturer( StringRef macroName, SourceLineInfo const& lineInfo, ResultWas::OfType resultType, StringRef names, bool isScoped );

        Capturer(Capturer const&) = delete;
        Capturer& operator=(Capturer const&) = delete;

        ~Capturer();

        void captureValue( size_t index, std::string const& value );

        template<typename T>
        void captureValues( size_t index, T const& value ) {
            captureValue( index, Catch::Detail::stringify( value ) );
        }

        template<typename T, typename... Ts>
        void captureValues( size_t index, T const& value, Ts const&... values ) {
            captureValue( index, Catch::Detail::stringify(value) );
            captureValues( index+1, values... );
        }
    };

} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( macroName, messageType, resultDisposition, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName##_catch_sr, CATCH_INTERNAL_LINEINFO, Catch::StringRef(), resultDisposition ); \
        catchAssertionHandler.handleMessage( messageType, ( Catch::MessageStream() << __VA_ARGS__ + ::Catch::StreamEndStop() ).m_stream.str() ); \
        catchAssertionHandler.complete(); \
    } while( false )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_CAPTURE( varName, macroName, scopedCapture, ... ) \
    Catch::Capturer varName( macroName##_catch_sr,                       \
                             CATCH_INTERNAL_LINEINFO,                    \
                             Catch::ResultWas::Info,                     \
                             #__VA_ARGS__##_catch_sr,                    \
                             scopedCapture );                            \
    varName.captureValues( 0, __VA_ARGS__ )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_INFO( macroName, log ) \
    const Catch::ScopedMessage INTERNAL_CATCH_UNIQUE_NAME( scopedMessage )( Catch::MessageBuilder( macroName##_catch_sr, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ) << log )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_UNSCOPED_INFO( macroName, log ) \
    Catch::IResultCapture::emplaceUnscopedMessage( Catch::MessageBuilder( macroName##_catch_sr, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ) << log )


#if defined(CATCH_CONFIG_PREFIX_MESSAGES) && !defined(CATCH_CONFIG_DISABLE)

  #define CATCH_INFO( msg ) INTERNAL_CATCH_INFO( "CATCH_INFO", msg )
  #define CATCH_UNSCOPED_INFO( msg ) INTERNAL_CATCH_UNSCOPED_INFO( "CATCH_UNSCOPED_INFO", msg )
  #define CATCH_WARN( msg ) INTERNAL_CATCH_MSG( "CATCH_WARN", Catch::ResultWas::Warning, Catch::ResultDisposition::ContinueOnFailure, msg )
  #define CATCH_CAPTURE( ... ) INTERNAL_CATCH_CAPTURE( INTERNAL_CATCH_UNIQUE_NAME(capturer), "CATCH_CAPTURE", true, __VA_ARGS__ )
  #define CATCH_UNSCOPED_CAPTURE( ... ) INTERNAL_CATCH_CAPTURE( INTERNAL_CATCH_UNIQUE_NAME(capturer), "CATCH_UNSCOPED_CAPTURE", false, __VA_ARGS__ )

#elif defined(CATCH_CONFIG_PREFIX_MESSAGES) && defined(CATCH_CONFIG_DISABLE)

  #define CATCH_INFO( msg )             (void)(0)
  #define CATCH_UNSCOPED_INFO( msg )    (void)(0)
  #define CATCH_WARN( msg )             (void)(0)
  #define CATCH_CAPTURE( ... )          (void)(0)
  #define CATCH_UNSCOPED_CAPTURE( ... ) (void)(0)

#elif !defined(CATCH_CONFIG_PREFIX_MESSAGES) && !defined(CATCH_CONFIG_DISABLE)

  #define INFO( msg ) INTERNAL_CATCH_INFO( "INFO", msg )
  #define UNSCOPED_INFO( msg ) INTERNAL_CATCH_UNSCOPED_INFO( "UNSCOPED_INFO", msg )
  #define WARN( msg ) INTERNAL_CATCH_MSG( "WARN", Catch::ResultWas::Warning, Catch::ResultDisposition::ContinueOnFailure, msg )
  #define CAPTURE( ... ) INTERNAL_CATCH_CAPTURE( INTERNAL_CATCH_UNIQUE_NAME(capturer), "CAPTURE", true, __VA_ARGS__ )
  #define UNSCOPED_CAPTURE( ... ) INTERNAL_CATCH_CAPTURE( INTERNAL_CATCH_UNIQUE_NAME(capturer), "UNSCOPED_CAPTURE", false, __VA_ARGS__ )

#elif !defined(CATCH_CONFIG_PREFIX_MESSAGES) && defined(CATCH_CONFIG_DISABLE)

  #define INFO( msg )             (void)(0)
  #define UNSCOPED_INFO( msg )    (void)(0)
  #define WARN( msg )             (void)(0)
  #define CAPTURE( ... )          (void)(0)
  #define UNSCOPED_CAPTURE( ... ) (void)(0)

#endif // end of user facing macro declarations




#endif // CATCH_MESSAGE_HPP_INCLUDED



// BEGIN Amalgamated content from catch_message.cpp (@wolfram77)
#ifndef CATCH_MESSAGE_CPP_INCLUDED
#define CATCH_MESSAGE_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_message.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_capture.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <cassert>
#include <stack>

namespace Catch {

    ////////////////////////////////////////////////////////////////////////////


    ScopedMessage::ScopedMessage( MessageBuilder&& builder ):
        m_messageId( builder.m_info.sequence ) {
        MessageInfo info( CATCH_MOVE( builder.m_info ) );
        info.message = builder.m_stream.str();
        IResultCapture::pushScopedMessage( CATCH_MOVE( info ) );
    }

    ScopedMessage::ScopedMessage( ScopedMessage&& old ) noexcept:
        m_messageId( old.m_messageId ) {
        old.m_moved = true;
    }

    ScopedMessage::~ScopedMessage() {
        if ( !m_moved ) { IResultCapture::popScopedMessage( m_messageId ); }
    }


    Capturer::Capturer( StringRef macroName,
                        SourceLineInfo const& lineInfo,
                        ResultWas::OfType resultType,
                        StringRef names,
                        bool isScoped):
        m_isScoped(isScoped) {
        auto trimmed = [&] (size_t start, size_t end) {
            while (names[start] == ',' || isspace(static_cast<unsigned char>(names[start]))) {
                ++start;
            }
            while (names[end] == ',' || isspace(static_cast<unsigned char>(names[end]))) {
                --end;
            }
            return names.substr(start, end - start + 1);
        };
        auto skipq = [&] (size_t start, char quote) {
            for (auto i = start + 1; i < names.size() ; ++i) {
                if (names[i] == quote)
                    return i;
                if (names[i] == '\\')
                    ++i;
            }
            CATCH_INTERNAL_ERROR("CAPTURE parsing encountered unmatched quote");
        };

        size_t start = 0;
        std::stack<char> openings;
        for (size_t pos = 0; pos < names.size(); ++pos) {
            char c = names[pos];
            switch (c) {
            case '[':
            case '{':
            case '(':
            // It is basically impossible to disambiguate between
            // comparison and start of template args in this context
//            case '<':
                openings.push(c);
                break;
            case ']':
            case '}':
            case ')':
//           case '>':
                openings.pop();
                break;
            case '"':
            case '\'':
                pos = skipq(pos, c);
                break;
            case ',':
                if (start != pos && openings.empty()) {
                    m_messages.emplace_back(macroName, lineInfo, resultType);
                    m_messages.back().message += trimmed(start, pos);
                    m_messages.back().message += " := "_sr;
                    start = pos;
                }
                break;
            default:; // noop
            }
        }
        assert(openings.empty() && "Mismatched openings");
        m_messages.emplace_back(macroName, lineInfo, resultType);
        m_messages.back().message += trimmed(start, names.size() - 1);
        m_messages.back().message += " := "_sr;
    }
    Capturer::~Capturer() {
        assert( m_captured == m_messages.size() );
        if ( m_isScoped ) {
            for ( auto const& message : m_messages ) {
                IResultCapture::popScopedMessage( message.sequence );
            }
        }
    }

    void Capturer::captureValue( size_t index, std::string const& value ) {
        assert( index < m_messages.size() );
        m_messages[index].message += value;
        if ( m_isScoped ) {
            IResultCapture::pushScopedMessage( CATCH_MOVE( m_messages[index] ) );
        } else {
            IResultCapture::addUnscopedMessage( CATCH_MOVE( m_messages[index] ) );
        }
        m_captured++;
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_MESSAGE_CPP_INCLUDED
// END Amalgamated content from catch_message.cpp (@wolfram77)

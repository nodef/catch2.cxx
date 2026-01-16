//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MESSAGE_INFO_HPP_INCLUDED
#define CATCH_MESSAGE_INFO_HPP_INCLUDED

#include "catch_deprecation_macro.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_result_type.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_source_line_info.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_stringref.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch {

    struct MessageInfo {
        MessageInfo(    StringRef _macroName,
                        SourceLineInfo const& _lineInfo,
                        ResultWas::OfType _type );

        StringRef macroName;
        std::string message;
        SourceLineInfo lineInfo;
        ResultWas::OfType type;
        // The "ID" of the message, used to know when to remove it from reporter context.
        unsigned int sequence;

        CATCH_DEPRECATED( "Explicitly use the 'sequence' member instead" )
        bool operator == (MessageInfo const& other) const {
            return sequence == other.sequence;
        }
        CATCH_DEPRECATED( "Explicitly use the 'sequence' member instead" )
        bool operator < (MessageInfo const& other) const {
            return sequence < other.sequence;
        }
    };

} // end namespace Catch

#endif // CATCH_MESSAGE_INFO_HPP_INCLUDED



// BEGIN Amalgamated content from catch_message_info.cpp (@wolfram77)
#ifndef CATCH_MESSAGE_INFO_CPP_INCLUDED
#define CATCH_MESSAGE_INFO_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_message_info.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_thread_local.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    namespace {
        // Messages are owned by their individual threads, so the counter should
        // be thread-local as well. Alternative consideration: atomic counter,
        // so threads don't share IDs and things are easier to debug.
        static CATCH_INTERNAL_THREAD_LOCAL unsigned int messageIDCounter = 0;
    }

    MessageInfo::MessageInfo( StringRef _macroName,
                              SourceLineInfo const& _lineInfo,
                              ResultWas::OfType _type )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        type( _type ),
        sequence( ++messageIDCounter )
    {}

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_MESSAGE_INFO_CPP_INCLUDED
// END Amalgamated content from catch_message_info.cpp (@wolfram77)


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

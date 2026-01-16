//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_SECTION_INFO_HPP_INCLUDED
#define CATCH_SECTION_INFO_HPP_INCLUDED

#include "internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_source_line_info.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_stringref.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_totals.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch {

    struct SectionInfo {
        // The last argument is ignored, so that people can write
        // SECTION("ShortName", "Proper description that is long") and
        // still use the `-c` flag comfortably.
        SectionInfo( SourceLineInfo const& _lineInfo, std::string _name,
                    const char* const = nullptr ):
            name(CATCH_MOVE(_name)),
            lineInfo(_lineInfo)
            {}

        std::string name;
        SourceLineInfo lineInfo;
    };

    struct SectionEndInfo {
        SectionInfo sectionInfo;
        Counts prevAssertions;
        double durationInSeconds;
    };

} // end namespace Catch

#endif // CATCH_SECTION_INFO_HPP_INCLUDED



// BEGIN Include main implementation (@wolfram77)
#ifdef CATCH2_IMPLEMENTATION
#include "internal/catch_main.cpp"
#include "catch_registry_hub.cpp"
#endif // CATCH2_IMPLEMENTATION
// END Include main implementation (@wolfram77)

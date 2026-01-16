
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_ASSERTION_INFO_HPP_INCLUDED
#define CATCH_ASSERTION_INFO_HPP_INCLUDED

#include "internal/catch_result_type.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_source_line_info.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_stringref.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    struct AssertionInfo {
        // AssertionInfo() = delete;

        StringRef macroName;
        SourceLineInfo lineInfo;
        StringRef capturedExpression;
        ResultDisposition::Flags resultDisposition;
    };

} // end namespace Catch

#endif // CATCH_ASSERTION_INFO_HPP_INCLUDED

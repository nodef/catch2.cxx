//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TAG_ALIAS_AUTOREGISTRAR_HPP_INCLUDED
#define CATCH_TAG_ALIAS_AUTOREGISTRAR_HPP_INCLUDED

#include "internal/catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_unique_name.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_source_line_info.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    struct RegistrarForTagAliases {
        RegistrarForTagAliases( char const* alias, char const* tag, SourceLineInfo const& lineInfo );
    };

} // end namespace Catch

#define CATCH_REGISTER_TAG_ALIAS( alias, spec ) \
    CATCH_INTERNAL_START_WARNINGS_SUPPRESSION \
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
    namespace{ const Catch::RegistrarForTagAliases INTERNAL_CATCH_UNIQUE_NAME( AutoRegisterTagAlias )( alias, spec, CATCH_INTERNAL_LINEINFO ); } \
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

#endif // CATCH_TAG_ALIAS_AUTOREGISTRAR_HPP_INCLUDED



// BEGIN Amalgamated content from catch_tag_alias_autoregistrar.cpp (@wolfram77)
#ifndef CATCH_TAG_ALIAS_AUTOREGISTRAR_CPP_INCLUDED
#define CATCH_TAG_ALIAS_AUTOREGISTRAR_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_tag_alias_autoregistrar.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "internal/catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    RegistrarForTagAliases::RegistrarForTagAliases(char const* alias, char const* tag, SourceLineInfo const& lineInfo) {
        CATCH_TRY {
            getMutableRegistryHub().registerTagAlias(alias, tag, lineInfo);
        } CATCH_CATCH_ALL {
            // Do not throw when constructing global objects, instead register the exception to be processed later
            getMutableRegistryHub().registerStartupException();
        }
    }

}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TAG_ALIAS_AUTOREGISTRAR_CPP_INCLUDED
// END Amalgamated content from catch_tag_alias_autoregistrar.cpp (@wolfram77)

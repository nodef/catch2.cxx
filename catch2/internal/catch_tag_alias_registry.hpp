//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED
#define CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED

#include "../interfaces/catch_interfaces_tag_alias_registry.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_tag_alias.hpp"  // Adjust to relative path (@wolfram77)

#include <map>
#include <string>

namespace Catch {
    struct SourceLineInfo;

    class TagAliasRegistry : public ITagAliasRegistry {
    public:
        ~TagAliasRegistry() override;
        TagAlias const* find( std::string const& alias ) const override;
        std::string expandAliases( std::string const& unexpandedTestSpec ) const override;
        void add( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo );

    private:
        std::map<std::string, TagAlias> m_registry;
    };

} // end namespace Catch

#endif // CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED



// BEGIN Amalgamated content from catch_tag_alias_registry.cpp (@wolfram77)
#ifndef CATCH_TAG_ALIAS_REGISTRY_CPP_INCLUDED
#define CATCH_TAG_ALIAS_REGISTRY_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_tag_alias_registry.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "../interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_string_manip.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    TagAliasRegistry::~TagAliasRegistry() = default;

    TagAlias const* TagAliasRegistry::find( std::string const& alias ) const {
        auto it = m_registry.find( alias );
        if( it != m_registry.end() )
            return &(it->second);
        else
            return nullptr;
    }

    std::string TagAliasRegistry::expandAliases( std::string const& unexpandedTestSpec ) const {
        std::string expandedTestSpec = unexpandedTestSpec;
        for( auto const& registryKvp : m_registry ) {
            std::size_t pos = expandedTestSpec.find( registryKvp.first );
            if( pos != std::string::npos ) {
                expandedTestSpec =  expandedTestSpec.substr( 0, pos ) +
                                    registryKvp.second.tag +
                                    expandedTestSpec.substr( pos + registryKvp.first.size() );
            }
        }
        return expandedTestSpec;
    }

    void TagAliasRegistry::add( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) {
        CATCH_ENFORCE( startsWith(alias, "[@") && endsWith(alias, ']'),
                      "error: tag alias, '" << alias << "' is not of the form [@alias name].\n" << lineInfo );

        CATCH_ENFORCE( m_registry.insert(std::make_pair(alias, TagAlias(tag, lineInfo))).second,
                      "error: tag alias, '" << alias << "' already registered.\n"
                      << "\tFirst seen at: " << find(alias)->lineInfo << "\n"
                      << "\tRedefined at: " << lineInfo );
    }

    ITagAliasRegistry::~ITagAliasRegistry() = default;

    ITagAliasRegistry const& ITagAliasRegistry::get() {
        return getRegistryHub().getTagAliasRegistry();
    }

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TAG_ALIAS_REGISTRY_CPP_INCLUDED
// END Amalgamated content from catch_tag_alias_registry.cpp (@wolfram77)

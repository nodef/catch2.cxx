//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEST_SPEC_HPP_INCLUDED
#define CATCH_TEST_SPEC_HPP_INCLUDED

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "internal/catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_wildcard_pattern.hpp"  // Adjust to relative path (@wolfram77)

#include <iosfwd>
#include <string>
#include <vector>

namespace Catch {

    class IConfig;
    struct TestCaseInfo;
    class TestCaseHandle;

    class TestSpec {

        class Pattern {
        public:
            explicit Pattern( std::string const& name );
            virtual ~Pattern();
            virtual bool matches( TestCaseInfo const& testCase ) const = 0;
            std::string const& name() const;
        private:
            virtual void serializeTo( std::ostream& out ) const = 0;
            // Writes string that would be reparsed into the pattern
            friend std::ostream& operator<<(std::ostream& out,
                                            Pattern const& pattern) {
                pattern.serializeTo( out );
                return out;
            }

            std::string const m_name;
        };

        class NamePattern : public Pattern {
        public:
            explicit NamePattern( std::string const& name, std::string const& filterString );
            bool matches( TestCaseInfo const& testCase ) const override;
        private:
            void serializeTo( std::ostream& out ) const override;

            WildcardPattern m_wildcardPattern;
        };

        class TagPattern : public Pattern {
        public:
            explicit TagPattern( std::string const& tag, std::string const& filterString );
            bool matches( TestCaseInfo const& testCase ) const override;
        private:
            void serializeTo( std::ostream& out ) const override;

            std::string m_tag;
        };

        struct Filter {
            std::vector<Detail::unique_ptr<Pattern>> m_required;
            std::vector<Detail::unique_ptr<Pattern>> m_forbidden;

            //! Serializes this filter into a string that would be parsed into
            //! an equivalent filter
            void serializeTo( std::ostream& out ) const;
            friend std::ostream& operator<<(std::ostream& out, Filter const& f) {
                f.serializeTo( out );
                return out;
            }

            bool matches( TestCaseInfo const& testCase ) const;
        };

        static std::string extractFilterName( Filter const& filter );

    public:
        struct FilterMatch {
            std::string name;
            std::vector<TestCaseHandle const*> tests;
        };
        using Matches = std::vector<FilterMatch>;
        using vectorStrings = std::vector<std::string>;

        bool hasFilters() const;
        bool matches( TestCaseInfo const& testCase ) const;
        Matches matchesByFilter( std::vector<TestCaseHandle> const& testCases, IConfig const& config ) const;
        const vectorStrings & getInvalidSpecs() const;

    private:
        std::vector<Filter> m_filters;
        std::vector<std::string> m_invalidSpecs;

        friend class TestSpecParser;
        //! Serializes this test spec into a string that would be parsed into
        //! equivalent test spec
        void serializeTo( std::ostream& out ) const;
        friend std::ostream& operator<<(std::ostream& out,
                                        TestSpec const& spec) {
            spec.serializeTo( out );
            return out;
        }
    };
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // CATCH_TEST_SPEC_HPP_INCLUDED



// BEGIN Amalgamated content from catch_test_spec.cpp (@wolfram77)
#ifndef CATCH_TEST_SPEC_CPP_INCLUDED
#define CATCH_TEST_SPEC_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_test_spec.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_testcase.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_test_case_registry_impl.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_reusable_string_stream.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_string_manip.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_test_case_info.hpp"  // Adjust to relative path (@wolfram77)

#include <algorithm>
#include <string>
#include <vector>
#include <ostream>

namespace Catch {

    TestSpec::Pattern::Pattern( std::string const& name )
    : m_name( name )
    {}

    TestSpec::Pattern::~Pattern() = default;

    std::string const& TestSpec::Pattern::name() const {
        return m_name;
    }


    TestSpec::NamePattern::NamePattern( std::string const& name, std::string const& filterString )
    : Pattern( filterString )
    , m_wildcardPattern( toLower( name ), CaseSensitive::No )
    {}

    bool TestSpec::NamePattern::matches( TestCaseInfo const& testCase ) const {
        return m_wildcardPattern.matches( testCase.name );
    }

    void TestSpec::NamePattern::serializeTo( std::ostream& out ) const {
        out << '"' << name() << '"';
    }


    TestSpec::TagPattern::TagPattern( std::string const& tag, std::string const& filterString )
    : Pattern( filterString )
    , m_tag( tag )
    {}

    bool TestSpec::TagPattern::matches( TestCaseInfo const& testCase ) const {
        return std::find( begin( testCase.tags ),
                          end( testCase.tags ),
                          Tag( m_tag ) ) != end( testCase.tags );
    }

    void TestSpec::TagPattern::serializeTo( std::ostream& out ) const {
        out << name();
    }

    bool TestSpec::Filter::matches( TestCaseInfo const& testCase ) const {
        bool should_use = !testCase.isHidden();
        for (auto const& pattern : m_required) {
            should_use = true;
            if (!pattern->matches(testCase)) {
                return false;
            }
        }
        for (auto const& pattern : m_forbidden) {
            if (pattern->matches(testCase)) {
                return false;
            }
        }
        return should_use;
    }

    void TestSpec::Filter::serializeTo( std::ostream& out ) const {
        bool first = true;
        for ( auto const& pattern : m_required ) {
            if ( !first ) {
                out << ' ';
            }
            out << *pattern;
            first = false;
        }
        for ( auto const& pattern : m_forbidden ) {
            if ( !first ) {
                out << ' ';
            }
            out << *pattern;
            first = false;
        }
    }


    std::string TestSpec::extractFilterName( Filter const& filter ) {
        Catch::ReusableStringStream sstr;
        sstr << filter;
        return sstr.str();
    }

    bool TestSpec::hasFilters() const {
        return !m_filters.empty();
    }

    bool TestSpec::matches( TestCaseInfo const& testCase ) const {
        return std::any_of( m_filters.begin(), m_filters.end(), [&]( Filter const& f ){ return f.matches( testCase ); } );
    }

    TestSpec::Matches TestSpec::matchesByFilter( std::vector<TestCaseHandle> const& testCases, IConfig const& config ) const {
        Matches matches;
        matches.reserve( m_filters.size() );
        for ( auto const& filter : m_filters ) {
            std::vector<TestCaseHandle const*> currentMatches;
            for ( auto const& test : testCases )
                if ( isThrowSafe( test, config ) &&
                     filter.matches( test.getTestCaseInfo() ) )
                    currentMatches.emplace_back( &test );
            matches.push_back(
                FilterMatch{ extractFilterName( filter ), currentMatches } );
        }
        return matches;
    }

    const TestSpec::vectorStrings& TestSpec::getInvalidSpecs() const {
        return m_invalidSpecs;
    }

    void TestSpec::serializeTo( std::ostream& out ) const {
        bool first = true;
        for ( auto const& filter : m_filters ) {
            if ( !first ) {
                out << ',';
            }
            out << filter;
            first = false;
        }
    }

}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TEST_SPEC_CPP_INCLUDED
// END Amalgamated content from catch_test_spec.cpp (@wolfram77)

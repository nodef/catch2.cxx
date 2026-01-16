//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_SECTION_HPP_INCLUDED
#define CATCH_SECTION_HPP_INCLUDED

#include "catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_config_static_analysis_support.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_noncopyable.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_section_info.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_timer.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_totals.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_unique_name.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    class Section : Detail::NonCopyable {
    public:
        Section( SectionInfo&& info );
        Section( SourceLineInfo const& _lineInfo,
                 StringRef _name,
                 const char* const = nullptr );
        ~Section();

        // This indicates whether the section should be executed or not
        explicit operator bool() const;

    private:
        SectionInfo m_info;

        Counts m_assertions;
        bool m_sectionIncluded;
        Timer m_timer;
    };

} // end namespace Catch

#if !defined(CATCH_CONFIG_EXPERIMENTAL_STATIC_ANALYSIS_SUPPORT)
#    define INTERNAL_CATCH_SECTION( ... )                                 \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                         \
        CATCH_INTERNAL_SUPPRESS_UNUSED_VARIABLE_WARNINGS                  \
        if ( Catch::Section const& INTERNAL_CATCH_UNIQUE_NAME(            \
                 catch_internal_Section ) =                               \
                 Catch::Section( CATCH_INTERNAL_LINEINFO, __VA_ARGS__ ) ) \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

#    define INTERNAL_CATCH_DYNAMIC_SECTION( ... )                     \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                     \
        CATCH_INTERNAL_SUPPRESS_UNUSED_VARIABLE_WARNINGS              \
        if ( Catch::Section const& INTERNAL_CATCH_UNIQUE_NAME(        \
                 catch_internal_Section ) =                           \
                 Catch::SectionInfo(                                  \
                     CATCH_INTERNAL_LINEINFO,                         \
                     ( Catch::ReusableStringStream() << __VA_ARGS__ ) \
                         .str() ) )                                   \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

#else

// These section definitions imply that at most one section at one level
// will be entered (because only one section's __LINE__ can be equal to
// the dummy `catchInternalSectionHint` variable from `TEST_CASE`).

namespace Catch {
    namespace Detail {
        // Intentionally without linkage, as it should only be used as a dummy
        // symbol for static analysis.
        // The arguments are used as a dummy for checking warnings in the passed
        // expressions.
        int GetNewSectionHint( StringRef, const char* const = nullptr );
    } // namespace Detail
} // namespace Catch


#    define INTERNAL_CATCH_SECTION( ... )                                   \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                           \
        CATCH_INTERNAL_SUPPRESS_UNUSED_VARIABLE_WARNINGS                    \
        CATCH_INTERNAL_SUPPRESS_SHADOW_WARNINGS                             \
        if ( [[maybe_unused]] const int catchInternalPreviousSectionHint =  \
                 catchInternalSectionHint,                                  \
             catchInternalSectionHint =                                     \
                 Catch::Detail::GetNewSectionHint(__VA_ARGS__);             \
             catchInternalPreviousSectionHint == __LINE__ )                 \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

#    define INTERNAL_CATCH_DYNAMIC_SECTION( ... )                           \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                           \
        CATCH_INTERNAL_SUPPRESS_UNUSED_VARIABLE_WARNINGS                    \
        CATCH_INTERNAL_SUPPRESS_SHADOW_WARNINGS                             \
        if ( [[maybe_unused]] const int catchInternalPreviousSectionHint =  \
                 catchInternalSectionHint,                                  \
             catchInternalSectionHint = Catch::Detail::GetNewSectionHint(   \
                ( Catch::ReusableStringStream() << __VA_ARGS__ ).str());    \
             catchInternalPreviousSectionHint == __LINE__ )                 \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

#endif


#endif // CATCH_SECTION_HPP_INCLUDED



// BEGIN Amalgamated content from catch_section.cpp (@wolfram77)
#ifndef CATCH_SECTION_CPP_INCLUDED
#define CATCH_SECTION_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_section.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "../interfaces/catch_interfaces_capture.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_uncaught_exceptions.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {

    Section::Section( SectionInfo&& info ):
        m_info( CATCH_MOVE( info ) ),
        m_sectionIncluded(
            getResultCapture().sectionStarted( m_info.name, m_info.lineInfo, m_assertions ) ) {
        // Non-"included" sections will not use the timing information
        // anyway, so don't bother with the potential syscall.
        if (m_sectionIncluded) {
            m_timer.start();
        }
    }

    Section::Section( SourceLineInfo const& _lineInfo,
                      StringRef _name,
                      const char* const ):
        m_info( { "invalid", static_cast<std::size_t>( -1 ) }, std::string{} ),
        m_sectionIncluded(
            getResultCapture().sectionStarted( _name, _lineInfo, m_assertions ) ) {
        // We delay initialization the SectionInfo member until we know
        // this section needs it, so we avoid allocating std::string for name.
        // We also delay timer start to avoid the potential syscall unless we
        // will actually use the result.
        if ( m_sectionIncluded ) {
            m_info.name = static_cast<std::string>( _name );
            m_info.lineInfo = _lineInfo;
            m_timer.start();
        }
    }

    Section::~Section() {
        if( m_sectionIncluded ) {
            SectionEndInfo endInfo{ CATCH_MOVE(m_info), m_assertions, m_timer.getElapsedSeconds() };
            if ( uncaught_exceptions() ) {
                getResultCapture().sectionEndedEarly( CATCH_MOVE(endInfo) );
            } else {
                getResultCapture().sectionEnded( CATCH_MOVE( endInfo ) );
            }
        }
    }

    // This indicates whether the section should be executed or not
    Section::operator bool() const {
        return m_sectionIncluded;
    }


} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_SECTION_CPP_INCLUDED
// END Amalgamated content from catch_section.cpp (@wolfram77)

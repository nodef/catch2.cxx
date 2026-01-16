//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_INTERFACES_GENERATORTRACKER_HPP_INCLUDED
#define CATCH_INTERFACES_GENERATORTRACKER_HPP_INCLUDED

#include "../internal/catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_stringref.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch {

    namespace Generators {
        class GeneratorUntypedBase {
            // Caches result from `toStringImpl`, assume that when it is an
            // empty string, the cache is invalidated.
            mutable std::string m_stringReprCache;

            // Counts based on `next` returning true
            std::size_t m_currentElementIndex = 0;

            /**
             * Attempts to move the generator to the next element
             *
             * Returns true iff the move succeeded (and a valid element
             * can be retrieved).
             */
            virtual bool next() = 0;

            //! Customization point for `currentElementAsString`
            virtual std::string stringifyImpl() const = 0;

            /**
             * Customization point for skipping to the n-th element
             *
             * Defaults to successively calling `countedNext`. If there
             * are not enough elements to reach the nth one, will throw
             * an error.
             */
            virtual void skipToNthElementImpl( std::size_t n );

        public:
            GeneratorUntypedBase() = default;
            // Generation of copy ops is deprecated (and Clang will complain)
            // if there is a user destructor defined
            GeneratorUntypedBase(GeneratorUntypedBase const&) = default;
            GeneratorUntypedBase& operator=(GeneratorUntypedBase const&) = default;

            virtual ~GeneratorUntypedBase(); // = default;

            /**
             * Attempts to move the generator to the next element
             *
             * Serves as a non-virtual interface to `next`, so that the
             * top level interface can provide sanity checking and shared
             * features.
             *
             * As with `next`, returns true iff the move succeeded and
             * the generator has new valid element to provide.
             */
            bool countedNext();

            std::size_t currentElementIndex() const { return m_currentElementIndex; }

            /**
             * Moves the generator forward **to** the n-th element
             *
             * Cannot move backwards. Can stay in place.
             */
            void skipToNthElement( std::size_t n );

            /**
             * Returns generator's current element as user-friendly string.
             *
             * By default returns string equivalent to calling
             * `Catch::Detail::stringify` on the current element, but generators
             * can customize their implementation as needed.
             *
             * Not thread-safe due to internal caching.
             *
             * The returned ref is valid only until the generator instance
             * is destructed, or it moves onto the next element, whichever
             * comes first.
             */
            StringRef currentElementAsString() const;
        };
        using GeneratorBasePtr = Catch::Detail::unique_ptr<GeneratorUntypedBase>;

    } // namespace Generators

    class IGeneratorTracker {
    public:
        virtual ~IGeneratorTracker(); // = default;
        virtual auto hasGenerator() const -> bool = 0;
        virtual auto getGenerator() const -> Generators::GeneratorBasePtr const& = 0;
        virtual void setGenerator( Generators::GeneratorBasePtr&& generator ) = 0;
    };

} // namespace Catch

#endif // CATCH_INTERFACES_GENERATORTRACKER_HPP_INCLUDED



// BEGIN Amalgamated content from catch_interfaces_generatortracker.cpp (@wolfram77)
#ifndef CATCH_INTERFACES_GENERATORTRACKER_CPP_INCLUDED
#define CATCH_INTERFACES_GENERATORTRACKER_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_interfaces_generatortracker.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "../generators/catch_generators.hpp"  // Adjust to relative path (@wolfram77)

#include <string>

namespace Catch {
    namespace Generators {

        bool GeneratorUntypedBase::countedNext() {
            auto ret = next();
            if ( ret ) {
                m_stringReprCache.clear();
                ++m_currentElementIndex;
            }
            return ret;
        }

        void GeneratorUntypedBase::skipToNthElementImpl( std::size_t n ) {
            for ( size_t i = m_currentElementIndex; i < n; ++i ) {
                bool isValid = next();
                if ( !isValid ) {
                    Detail::throw_generator_exception(
                        "Coud not jump to Nth element: not enough elements" );
                }
            }
        }

        void GeneratorUntypedBase::skipToNthElement( std::size_t n ) {
            if ( n < m_currentElementIndex ) {
                Detail::throw_generator_exception(
                    "Tried to jump generator backwards" );
            }
            if ( n == m_currentElementIndex ) { return; }

            skipToNthElementImpl(n);
            // Fixup tracking after moving the generator forward
            //  * Ensure that the correct element index is set after skipping
            //  * Invalidate cache
            m_currentElementIndex = n;
            m_stringReprCache.clear();
        }

        StringRef GeneratorUntypedBase::currentElementAsString() const {
            if ( m_stringReprCache.empty() ) {
                m_stringReprCache = stringifyImpl();
            }
            return m_stringReprCache;
        }

    } // namespace Generators
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_INTERFACES_GENERATORTRACKER_CPP_INCLUDED
// END Amalgamated content from catch_interfaces_generatortracker.cpp (@wolfram77)

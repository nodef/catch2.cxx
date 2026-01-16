//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MATCHERS_CONTAINER_PROPERTIES_HPP_INCLUDED
#define CATCH_MATCHERS_CONTAINER_PROPERTIES_HPP_INCLUDED

#include "catch_matchers_templated.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_container_nonmembers.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
    namespace Matchers {

        class IsEmptyMatcher final : public MatcherGenericBase {
        public:
            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
#if defined(CATCH_CONFIG_POLYFILL_NONMEMBER_CONTAINER_ACCESS)
                using Catch::Detail::empty;
#else
                using std::empty;
#endif
                return empty(rng);
            }

            std::string describe() const override;
        };

        class HasSizeMatcher final : public MatcherGenericBase {
            std::size_t m_target_size;
        public:
            explicit HasSizeMatcher(std::size_t target_size):
                m_target_size(target_size)
            {}

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
#if defined(CATCH_CONFIG_POLYFILL_NONMEMBER_CONTAINER_ACCESS)
                using Catch::Detail::size;
#else
                using std::size;
#endif
                return size(rng) == m_target_size;
            }

            std::string describe() const override;
        };

        template <typename Matcher>
        class SizeMatchesMatcher final : public MatcherGenericBase {
            Matcher m_matcher;
        public:
            explicit SizeMatchesMatcher(Matcher m):
                m_matcher(CATCH_MOVE(m))
            {}

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
#if defined(CATCH_CONFIG_POLYFILL_NONMEMBER_CONTAINER_ACCESS)
                using Catch::Detail::size;
#else
                using std::size;
#endif
                return m_matcher.match(size(rng));
            }

            std::string describe() const override {
                return "size matches " + m_matcher.describe();
            }
        };


        //! Creates a matcher that accepts empty ranges/containers
        IsEmptyMatcher IsEmpty();
        //! Creates a matcher that accepts ranges/containers with specific size
        HasSizeMatcher SizeIs(std::size_t sz);
        template <typename Matcher>
        std::enable_if_t<Detail::is_matcher_v<Matcher>,
        SizeMatchesMatcher<Matcher>> SizeIs(Matcher&& m) {
            return SizeMatchesMatcher<Matcher>{CATCH_FORWARD(m)};
        }

    } // end namespace Matchers
} // end namespace Catch

#endif // CATCH_MATCHERS_CONTAINER_PROPERTIES_HPP_INCLUDED



// BEGIN Amalgamated content from catch_matchers_container_properties.cpp (@wolfram77)
#ifndef CATCH_MATCHERS_CONTAINER_PROPERTIES_CPP_INCLUDED
#define CATCH_MATCHERS_CONTAINER_PROPERTIES_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_matchers_container_properties.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "../internal/catch_reusable_string_stream.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
namespace Matchers {

    std::string IsEmptyMatcher::describe() const {
        return "is empty";
    }

    std::string HasSizeMatcher::describe() const {
        ReusableStringStream sstr;
        sstr << "has size == " << m_target_size;
        return sstr.str();
    }

    IsEmptyMatcher IsEmpty() {
        return {};
    }

    HasSizeMatcher SizeIs(std::size_t sz) {
        return HasSizeMatcher{ sz };
    }

} // end namespace Matchers
} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_MATCHERS_CONTAINER_PROPERTIES_CPP_INCLUDED
// END Amalgamated content from catch_matchers_container_properties.cpp (@wolfram77)

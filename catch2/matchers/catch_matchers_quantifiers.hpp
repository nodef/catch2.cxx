//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MATCHERS_QUANTIFIERS_HPP_INCLUDED
#define CATCH_MATCHERS_QUANTIFIERS_HPP_INCLUDED

#include "catch_matchers_templated.hpp"  // Adjust to relative path (@wolfram77)
#include "../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

namespace Catch {
    namespace Matchers {
        // Matcher for checking that all elements in range matches a given matcher.
        template <typename Matcher>
        class AllMatchMatcher final : public MatcherGenericBase {
            Matcher m_matcher;
        public:
            AllMatchMatcher(Matcher matcher):
                m_matcher(CATCH_MOVE(matcher))
            {}

            std::string describe() const override {
                return "all match " + m_matcher.describe();
            }

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
                for (auto&& elem : rng) {
                    if (!m_matcher.match(elem)) {
                        return false;
                    }
                }
                return true;
            }
        };

        // Matcher for checking that no element in range matches a given matcher.
        template <typename Matcher>
        class NoneMatchMatcher final : public MatcherGenericBase {
            Matcher m_matcher;
        public:
            NoneMatchMatcher(Matcher matcher):
                m_matcher(CATCH_MOVE(matcher))
            {}

            std::string describe() const override {
                return "none match " + m_matcher.describe();
            }

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
                for (auto&& elem : rng) {
                    if (m_matcher.match(elem)) {
                        return false;
                    }
                }
                return true;
            }
        };

        // Matcher for checking that at least one element in range matches a given matcher.
        template <typename Matcher>
        class AnyMatchMatcher final : public MatcherGenericBase {
            Matcher m_matcher;
        public:
            AnyMatchMatcher(Matcher matcher):
                m_matcher(CATCH_MOVE(matcher))
            {}

            std::string describe() const override {
                return "any match " + m_matcher.describe();
            }

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
                for (auto&& elem : rng) {
                    if (m_matcher.match(elem)) {
                        return true;
                    }
                }
                return false;
            }
        };

        // Matcher for checking that all elements in range are true.
        class AllTrueMatcher final : public MatcherGenericBase {
        public:
            std::string describe() const override;

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
                for (auto&& elem : rng) {
                    if (!elem) {
                        return false;
                    }
                }
                return true;
            }
        };

        // Matcher for checking that no element in range is true.
        class NoneTrueMatcher final : public MatcherGenericBase {
        public:
            std::string describe() const override;

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
                for (auto&& elem : rng) {
                    if (elem) {
                        return false;
                    }
                }
                return true;
            }
        };

        // Matcher for checking that any element in range is true.
        class AnyTrueMatcher final : public MatcherGenericBase {
        public:
            std::string describe() const override;

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
                for (auto&& elem : rng) {
                    if (elem) {
                        return true;
                    }
                }
                return false;
            }
        };

        // Creates a matcher that checks whether all elements in a range match a matcher
        template <typename Matcher>
        AllMatchMatcher<Matcher> AllMatch(Matcher&& matcher) {
            return { CATCH_FORWARD(matcher) };
        }

        // Creates a matcher that checks whether no element in a range matches a matcher.
        template <typename Matcher>
        NoneMatchMatcher<Matcher> NoneMatch(Matcher&& matcher) {
            return { CATCH_FORWARD(matcher) };
        }

        // Creates a matcher that checks whether any element in a range matches a matcher.
        template <typename Matcher>
        AnyMatchMatcher<Matcher> AnyMatch(Matcher&& matcher) {
            return { CATCH_FORWARD(matcher) };
        }

        // Creates a matcher that checks whether all elements in a range are true
        AllTrueMatcher AllTrue();

        // Creates a matcher that checks whether no element in a range is true
        NoneTrueMatcher NoneTrue();

        // Creates a matcher that checks whether any element in a range is true
        AnyTrueMatcher AnyTrue();
    }
}

#endif // CATCH_MATCHERS_QUANTIFIERS_HPP_INCLUDED



// BEGIN Amalgamated content from catch_matchers_quantifiers.cpp (@wolfram77)
#ifndef CATCH_MATCHERS_QUANTIFIERS_CPP_INCLUDED
#define CATCH_MATCHERS_QUANTIFIERS_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_matchers_quantifiers.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

namespace Catch {
    namespace Matchers {
        std::string AllTrueMatcher::describe() const { return "contains only true"; }

        AllTrueMatcher AllTrue() { return AllTrueMatcher{}; }

        std::string NoneTrueMatcher::describe() const { return "contains no true"; }

        NoneTrueMatcher NoneTrue() { return NoneTrueMatcher{}; }

        std::string AnyTrueMatcher::describe() const { return "contains at least one true"; }

        AnyTrueMatcher AnyTrue() { return AnyTrueMatcher{}; }
    } // namespace Matchers
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_MATCHERS_QUANTIFIERS_CPP_INCLUDED
// END Amalgamated content from catch_matchers_quantifiers.cpp (@wolfram77)

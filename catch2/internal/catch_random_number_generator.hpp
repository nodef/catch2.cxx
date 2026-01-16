//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_RANDOM_NUMBER_GENERATOR_HPP_INCLUDED
#define CATCH_RANDOM_NUMBER_GENERATOR_HPP_INCLUDED

#include <cstdint>

namespace Catch {

    // This is a simple implementation of C++11 Uniform Random Number
    // Generator. It does not provide all operators, because Catch2
    // does not use it, but it should behave as expected inside stdlib's
    // distributions.
    // The implementation is based on the PCG family (http://pcg-random.org)
    class SimplePcg32 {
        using state_type = std::uint64_t;
    public:
        using result_type = std::uint32_t;
        static constexpr result_type (min)() {
            return 0;
        }
        static constexpr result_type (max)() {
            return static_cast<result_type>(-1);
        }

        // Provide some default initial state for the default constructor
        SimplePcg32():SimplePcg32(0xed743cc4U) {}

        explicit SimplePcg32(result_type seed_);

        void seed(result_type seed_);
        void discard(uint64_t skip);

        result_type operator()();

    private:
        friend bool operator==(SimplePcg32 const& lhs, SimplePcg32 const& rhs);
        friend bool operator!=(SimplePcg32 const& lhs, SimplePcg32 const& rhs);

        // In theory we also need operator<< and operator>>
        // In practice we do not use them, so we will skip them for now


        std::uint64_t m_state;
        // This part of the state determines which "stream" of the numbers
        // is chosen -- we take it as a constant for Catch2, so we only
        // need to deal with seeding the main state.
        // Picked by reading 8 bytes from `/dev/random` :-)
        static const std::uint64_t s_inc = (0x13ed0cc53f939476ULL << 1ULL) | 1ULL;
    };

} // end namespace Catch

#endif // CATCH_RANDOM_NUMBER_GENERATOR_HPP_INCLUDED



// BEGIN Amalgamated content from catch_random_number_generator.cpp (@wolfram77)
#ifndef CATCH_RANDOM_NUMBER_GENERATOR_CPP_INCLUDED
#define CATCH_RANDOM_NUMBER_GENERATOR_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// #include "catch_random_number_generator.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)

#if defined( __clang__ )
#    define CATCH2_CLANG_NO_SANITIZE_INTEGER \
        __attribute__( ( no_sanitize( "unsigned-integer-overflow" ) ) )
#else
#    define CATCH2_CLANG_NO_SANITIZE_INTEGER
#endif
namespace Catch {

namespace {

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4146) // we negate uint32 during the rotate
#endif
        // Safe rotr implementation thanks to John Regehr
        CATCH2_CLANG_NO_SANITIZE_INTEGER
        uint32_t rotate_right(uint32_t val, uint32_t count) {
            const uint32_t mask = 31;
            count &= mask;
            return (val >> count) | (val << (-count & mask));
        }

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

}


    SimplePcg32::SimplePcg32(result_type seed_) {
        seed(seed_);
    }


    void SimplePcg32::seed(result_type seed_) {
        m_state = 0;
        (*this)();
        m_state += seed_;
        (*this)();
    }

    void SimplePcg32::discard(uint64_t skip) {
        // We could implement this to run in O(log n) steps, but this
        // should suffice for our use case.
        for (uint64_t s = 0; s < skip; ++s) {
            static_cast<void>((*this)());
        }
    }

    CATCH2_CLANG_NO_SANITIZE_INTEGER
    SimplePcg32::result_type SimplePcg32::operator()() {
        // prepare the output value
        const uint32_t xorshifted = static_cast<uint32_t>(((m_state >> 18u) ^ m_state) >> 27u);
        const auto output = rotate_right(xorshifted, static_cast<uint32_t>(m_state >> 59u));

        // advance state
        m_state = m_state * 6364136223846793005ULL + s_inc;

        return output;
    }

    bool operator==(SimplePcg32 const& lhs, SimplePcg32 const& rhs) {
        return lhs.m_state == rhs.m_state;
    }

    bool operator!=(SimplePcg32 const& lhs, SimplePcg32 const& rhs) {
        return lhs.m_state != rhs.m_state;
    }
}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_RANDOM_NUMBER_GENERATOR_CPP_INCLUDED
// END Amalgamated content from catch_random_number_generator.cpp (@wolfram77)

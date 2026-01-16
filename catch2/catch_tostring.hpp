//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TOSTRING_HPP_INCLUDED
#define CATCH_TOSTRING_HPP_INCLUDED

#include <ctime>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <string>

#include "internal/catch_compiler_capabilities.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_config_wchar.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_reusable_string_stream.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_void_type.hpp"  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_enum_values_registry.hpp"  // Adjust to relative path (@wolfram77)

#ifdef CATCH_CONFIG_CPP17_STRING_VIEW
#include <string_view>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4180) // We attempt to stream a function (address) by const&, which MSVC complains about but is harmless
#endif

// We need a dummy global operator<< so we can bring it into Catch namespace later
struct Catch_global_namespace_dummy{};
std::ostream& operator<<(std::ostream&, Catch_global_namespace_dummy);

namespace Catch {
    // Bring in global namespace operator<< for ADL lookup in
    // `IsStreamInsertable` below.
    using ::operator<<;

    namespace Detail {

        std::size_t catch_strnlen(const char *str, std::size_t n);

        std::string formatTimeT( std::time_t time );

        constexpr StringRef unprintableString = "{?}"_sr;

        //! Encases `string in quotes, and optionally escapes invisibles
        std::string convertIntoString( StringRef string, bool escapeInvisibles );

        //! Encases `string` in quotes, and escapes invisibles if user requested
        //! it via CLI
        std::string convertIntoString( StringRef string );

        std::string rawMemoryToString( const void *object, std::size_t size );

        template<typename T>
        std::string rawMemoryToString( const T& object ) {
          return rawMemoryToString( &object, sizeof(object) );
        }

        template<typename T,typename = void>
        static constexpr bool IsStreamInsertable_v = false;

        template <typename T>
        static constexpr bool IsStreamInsertable_v<
            T,
            decltype( void( std::declval<std::ostream&>() << std::declval<T>() ) )> =
            true;

        template<typename E>
        std::string convertUnknownEnumToString( E e );

        template<typename T>
        std::enable_if_t<
            !std::is_enum<T>::value && !std::is_base_of<std::exception, T>::value,
        std::string> convertUnstreamable( T const& ) {
            return std::string(Detail::unprintableString);
        }
        template<typename T>
        std::enable_if_t<
            !std::is_enum<T>::value && std::is_base_of<std::exception, T>::value,
         std::string> convertUnstreamable(T const& ex) {
            return ex.what();
        }


        template<typename T>
        std::enable_if_t<
            std::is_enum<T>::value,
        std::string> convertUnstreamable( T const& value ) {
            return convertUnknownEnumToString( value );
        }

#if defined(_MANAGED)
        //! Convert a CLR string to a utf8 std::string
        template<typename T>
        std::string clrReferenceToString( T^ ref ) {
            if (ref == nullptr)
                return std::string("null");
            auto bytes = System::Text::Encoding::UTF8->GetBytes(ref->ToString());
            cli::pin_ptr<System::Byte> p = &bytes[0];
            return std::string(reinterpret_cast<char const *>(p), bytes->Length);
        }
#endif

    } // namespace Detail


    template <typename T, typename = void>
    struct StringMaker {
        template <typename Fake = T>
        static
        std::enable_if_t<::Catch::Detail::IsStreamInsertable_v<Fake>, std::string>
            convert(const Fake& value) {
                ReusableStringStream rss;
                // NB: call using the function-like syntax to avoid ambiguity with
                // user-defined templated operator<< under clang.
                rss.operator<<(value);
                return rss.str();
        }

        template <typename Fake = T>
        static
        std::enable_if_t<!::Catch::Detail::IsStreamInsertable_v<Fake>, std::string>
            convert( const Fake& value ) {
#if !defined(CATCH_CONFIG_FALLBACK_STRINGIFIER)
            return Detail::convertUnstreamable(value);
#else
            return CATCH_CONFIG_FALLBACK_STRINGIFIER(value);
#endif
        }
    };

    namespace Detail {

        std::string makeExceptionHappenedString();

        // This function dispatches all stringification requests inside of Catch.
        // Should be preferably called fully qualified, like ::Catch::Detail::stringify
        template <typename T>
        std::string stringify( const T& e ) {
            CATCH_TRY {
                return ::Catch::StringMaker<
                    std::remove_cv_t<std::remove_reference_t<T>>>::convert( e );
            }
            CATCH_CATCH_ALL { return makeExceptionHappenedString(); }
        }

        template<typename E>
        std::string convertUnknownEnumToString( E e ) {
            return ::Catch::Detail::stringify(static_cast<std::underlying_type_t<E>>(e));
        }

#if defined(_MANAGED)
        template <typename T>
        std::string stringify( T^ e ) {
            return ::Catch::StringMaker<T^>::convert(e);
        }
#endif

    } // namespace Detail

    // Some predefined specializations

    template<>
    struct StringMaker<std::string> {
        static std::string convert(const std::string& str);
    };

#ifdef CATCH_CONFIG_CPP17_STRING_VIEW
    template<>
    struct StringMaker<std::string_view> {
        static std::string convert(std::string_view str);
    };
#endif

    template<>
    struct StringMaker<char const *> {
        static std::string convert(char const * str);
    };
    template<>
    struct StringMaker<char *> {
        static std::string convert(char * str);
    };

#if defined(CATCH_CONFIG_WCHAR)
    template<>
    struct StringMaker<std::wstring> {
        static std::string convert(const std::wstring& wstr);
    };

# ifdef CATCH_CONFIG_CPP17_STRING_VIEW
    template<>
    struct StringMaker<std::wstring_view> {
        static std::string convert(std::wstring_view str);
    };
# endif

    template<>
    struct StringMaker<wchar_t const *> {
        static std::string convert(wchar_t const * str);
    };
    template<>
    struct StringMaker<wchar_t *> {
        static std::string convert(wchar_t * str);
    };
#endif // CATCH_CONFIG_WCHAR

    template<size_t SZ>
    struct StringMaker<char[SZ]> {
        static std::string convert(char const* str) {
            return Detail::convertIntoString(
                StringRef( str, Detail::catch_strnlen( str, SZ ) ) );
        }
    };
    template<size_t SZ>
    struct StringMaker<signed char[SZ]> {
        static std::string convert(signed char const* str) {
            auto reinterpreted = reinterpret_cast<char const*>(str);
            return Detail::convertIntoString(
                StringRef(reinterpreted, Detail::catch_strnlen(reinterpreted, SZ)));
        }
    };
    template<size_t SZ>
    struct StringMaker<unsigned char[SZ]> {
        static std::string convert(unsigned char const* str) {
            auto reinterpreted = reinterpret_cast<char const*>(str);
            return Detail::convertIntoString(
                StringRef(reinterpreted, Detail::catch_strnlen(reinterpreted, SZ)));
        }
    };

#if defined(CATCH_CONFIG_CPP17_BYTE)
    template<>
    struct StringMaker<std::byte> {
        static std::string convert(std::byte value);
    };
#endif // defined(CATCH_CONFIG_CPP17_BYTE)
    template<>
    struct StringMaker<int> {
        static std::string convert(int value);
    };
    template<>
    struct StringMaker<long> {
        static std::string convert(long value);
    };
    template<>
    struct StringMaker<long long> {
        static std::string convert(long long value);
    };
    template<>
    struct StringMaker<unsigned int> {
        static std::string convert(unsigned int value);
    };
    template<>
    struct StringMaker<unsigned long> {
        static std::string convert(unsigned long value);
    };
    template<>
    struct StringMaker<unsigned long long> {
        static std::string convert(unsigned long long value);
    };

    template<>
    struct StringMaker<bool> {
        static std::string convert(bool b) {
            using namespace std::string_literals;
            return b ? "true"s : "false"s;
        }
    };

    template<>
    struct StringMaker<char> {
        static std::string convert(char c);
    };
    template<>
    struct StringMaker<signed char> {
        static std::string convert(signed char value);
    };
    template<>
    struct StringMaker<unsigned char> {
        static std::string convert(unsigned char value);
    };

    template<>
    struct StringMaker<std::nullptr_t> {
        static std::string convert(std::nullptr_t) {
            using namespace std::string_literals;
            return "nullptr"s;
        }
    };

    template<>
    struct StringMaker<float> {
        static std::string convert(float value);
        CATCH_EXPORT static int precision;
    };

    template<>
    struct StringMaker<double> {
        static std::string convert(double value);
        CATCH_EXPORT static int precision;
    };

    template <typename T>
    struct StringMaker<T*> {
        template <typename U>
        static std::string convert(U* p) {
            if (p) {
                return ::Catch::Detail::rawMemoryToString(p);
            } else {
                return "nullptr";
            }
        }
    };

    template <typename R, typename C>
    struct StringMaker<R C::*> {
        static std::string convert(R C::* p) {
            if (p) {
                return ::Catch::Detail::rawMemoryToString(p);
            } else {
                return "nullptr";
            }
        }
    };

#if defined(_MANAGED)
    template <typename T>
    struct StringMaker<T^> {
        static std::string convert( T^ ref ) {
            return ::Catch::Detail::clrReferenceToString(ref);
        }
    };
#endif

    namespace Detail {
        template<typename InputIterator, typename Sentinel = InputIterator>
        std::string rangeToString(InputIterator first, Sentinel last) {
            ReusableStringStream rss;
            rss << "{ ";
            if (first != last) {
                rss << ::Catch::Detail::stringify(*first);
                for (++first; first != last; ++first)
                    rss << ", " << ::Catch::Detail::stringify(*first);
            }
            rss << " }";
            return rss.str();
        }
    }

} // namespace Catch

//////////////////////////////////////////////////////
// Separate std-lib types stringification, so it can be selectively enabled
// This means that we do not bring in their headers

#if defined(CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS)
#  define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER
#  define CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER
#  define CATCH_CONFIG_ENABLE_VARIANT_STRINGMAKER
#  define CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER
#endif

// Separate std::pair specialization
#if defined(CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER)
#include <utility>
namespace Catch {
    template<typename T1, typename T2>
    struct StringMaker<std::pair<T1, T2> > {
        static std::string convert(const std::pair<T1, T2>& pair) {
            ReusableStringStream rss;
            rss << "{ "
                << ::Catch::Detail::stringify(pair.first)
                << ", "
                << ::Catch::Detail::stringify(pair.second)
                << " }";
            return rss.str();
        }
    };
}
#endif // CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#if defined(CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER) && defined(CATCH_CONFIG_CPP17_OPTIONAL)
#include <optional>
namespace Catch {
    template<typename T>
    struct StringMaker<std::optional<T> > {
        static std::string convert(const std::optional<T>& optional) {
            if (optional.has_value()) {
                return ::Catch::Detail::stringify(*optional);
            } else {
                return "{ }";
            }
        }
    };
    template <>
    struct StringMaker<std::nullopt_t> {
        static std::string convert(const std::nullopt_t&) {
            return "{ }";
        }
    };
}
#endif // CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER

// Separate std::tuple specialization
#if defined(CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER)
#    include <tuple>
#    include <utility>
namespace Catch {
    namespace Detail {
        template <typename Tuple, std::size_t... Is>
        void PrintTuple( const Tuple& tuple,
                         std::ostream& os,
                         std::index_sequence<Is...> ) {
            // 1 + Account for when the tuple is empty
            char a[1 + sizeof...( Is )] = {
                ( ( os << ( Is ? ", " : " " )
                       << ::Catch::Detail::stringify( std::get<Is>( tuple ) ) ),
                  '\0' )... };
            (void)a;
        }

    } // namespace Detail

    template <typename... Types>
    struct StringMaker<std::tuple<Types...>> {
        static std::string convert( const std::tuple<Types...>& tuple ) {
            ReusableStringStream rss;
            rss << '{';
            Detail::PrintTuple(
                tuple,
                rss.get(),
                std::make_index_sequence<sizeof...( Types )>{} );
            rss << " }";
            return rss.str();
        }
    };
} // namespace Catch
#endif // CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER

#if defined(CATCH_CONFIG_ENABLE_VARIANT_STRINGMAKER) && defined(CATCH_CONFIG_CPP17_VARIANT)
#include <variant>
namespace Catch {
    template<>
    struct StringMaker<std::monostate> {
        static std::string convert(const std::monostate&) {
            return "{ }";
        }
    };

    template<typename... Elements>
    struct StringMaker<std::variant<Elements...>> {
        static std::string convert(const std::variant<Elements...>& variant) {
            if (variant.valueless_by_exception()) {
                return "{valueless variant}";
            } else {
                return std::visit(
                    [](const auto& value) {
                        return ::Catch::Detail::stringify(value);
                    },
                    variant
                );
            }
        }
    };
}
#endif // CATCH_CONFIG_ENABLE_VARIANT_STRINGMAKER

namespace Catch {
    // Import begin/ end from std here
    using std::begin;
    using std::end;

    namespace Detail {
        template <typename T, typename = void>
        struct is_range_impl : std::false_type {};

        template <typename T>
        struct is_range_impl<T, void_t<decltype(begin(std::declval<T>()))>> : std::true_type {};
    } // namespace Detail

    template <typename T>
    struct is_range : Detail::is_range_impl<T> {};

#if defined(_MANAGED) // Managed types are never ranges
    template <typename T>
    struct is_range<T^> {
        static const bool value = false;
    };
#endif

    template<typename Range>
    std::string rangeToString( Range const& range ) {
        return ::Catch::Detail::rangeToString( begin( range ), end( range ) );
    }

    // Handle vector<bool> specially
    template<typename Allocator>
    std::string rangeToString( std::vector<bool, Allocator> const& v ) {
        ReusableStringStream rss;
        rss << "{ ";
        bool first = true;
        for( bool b : v ) {
            if( first )
                first = false;
            else
                rss << ", ";
            rss << ::Catch::Detail::stringify( b );
        }
        rss << " }";
        return rss.str();
    }

    template<typename R>
    struct StringMaker<R, std::enable_if_t<is_range<R>::value && !::Catch::Detail::IsStreamInsertable_v<R>>> {
        static std::string convert( R const& range ) {
            return rangeToString( range );
        }
    };

    template <typename T, size_t SZ>
    struct StringMaker<T[SZ]> {
        static std::string convert(T const(&arr)[SZ]) {
            return rangeToString(arr);
        }
    };


} // namespace Catch

// Separate std::chrono::duration specialization
#include <ctime>
#include <ratio>
#include <chrono>


namespace Catch {

template <class Ratio>
struct ratio_string {
    static std::string symbol() {
        Catch::ReusableStringStream rss;
        rss << '[' << Ratio::num << '/'
            << Ratio::den << ']';
        return rss.str();
    }
};

template <>
struct ratio_string<std::atto> {
    static char symbol() { return 'a'; }
};
template <>
struct ratio_string<std::femto> {
    static char symbol() { return 'f'; }
};
template <>
struct ratio_string<std::pico> {
    static char symbol() { return 'p'; }
};
template <>
struct ratio_string<std::nano> {
    static char symbol() { return 'n'; }
};
template <>
struct ratio_string<std::micro> {
    static char symbol() { return 'u'; }
};
template <>
struct ratio_string<std::milli> {
    static char symbol() { return 'm'; }
};

    ////////////
    // std::chrono::duration specializations
    template<typename Value, typename Ratio>
    struct StringMaker<std::chrono::duration<Value, Ratio>> {
        static std::string convert(std::chrono::duration<Value, Ratio> const& duration) {
            ReusableStringStream rss;
            rss << duration.count() << ' ' << ratio_string<Ratio>::symbol() << 's';
            return rss.str();
        }
    };
    template<typename Value>
    struct StringMaker<std::chrono::duration<Value, std::ratio<1>>> {
        static std::string convert(std::chrono::duration<Value, std::ratio<1>> const& duration) {
            ReusableStringStream rss;
            rss << duration.count() << " s";
            return rss.str();
        }
    };
    template<typename Value>
    struct StringMaker<std::chrono::duration<Value, std::ratio<60>>> {
        static std::string convert(std::chrono::duration<Value, std::ratio<60>> const& duration) {
            ReusableStringStream rss;
            rss << duration.count() << " m";
            return rss.str();
        }
    };
    template<typename Value>
    struct StringMaker<std::chrono::duration<Value, std::ratio<3600>>> {
        static std::string convert(std::chrono::duration<Value, std::ratio<3600>> const& duration) {
            ReusableStringStream rss;
            rss << duration.count() << " h";
            return rss.str();
        }
    };

    ////////////
    // std::chrono::time_point specialization
    // Generic time_point cannot be specialized, only std::chrono::time_point<system_clock>
    template<typename Clock, typename Duration>
    struct StringMaker<std::chrono::time_point<Clock, Duration>> {
        static std::string convert(std::chrono::time_point<Clock, Duration> const& time_point) {
            return ::Catch::Detail::stringify(time_point.time_since_epoch()) + " since epoch";
        }
    };
    // std::chrono::time_point<system_clock> specialization
    template<typename Duration>
    struct StringMaker<std::chrono::time_point<std::chrono::system_clock, Duration>> {
        static std::string convert(std::chrono::time_point<std::chrono::system_clock, Duration> const& time_point) {
            const auto systemish = std::chrono::time_point_cast<
                std::chrono::system_clock::duration>( time_point );
            const auto as_time_t = std::chrono::system_clock::to_time_t( systemish );
            return ::Catch::Detail::formatTimeT( as_time_t );
        }
    };
}

#include "interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)

#define INTERNAL_CATCH_REGISTER_ENUM( enumName, ... ) \
namespace Catch { \
    template<> struct StringMaker<enumName> { \
        static std::string convert( enumName value ) { \
            static const auto& enumInfo = ::Catch::getMutableRegistryHub().getMutableEnumValuesRegistry().registerEnum( #enumName, #__VA_ARGS__, { __VA_ARGS__ } ); \
            return static_cast<std::string>(enumInfo.lookup( static_cast<int>( value ) )); \
        } \
    }; \
}

#define CATCH_REGISTER_ENUM( enumName, ... ) INTERNAL_CATCH_REGISTER_ENUM( enumName, __VA_ARGS__ )

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // CATCH_TOSTRING_HPP_INCLUDED



// BEGIN Amalgamated content from catch_tostring.cpp (@wolfram77)
#ifndef CATCH_TOSTRING_CPP_INCLUDED
#define CATCH_TOSTRING_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_tostring.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_config.hpp"  // Adjust to relative path (@wolfram77)
#include "interfaces/catch_interfaces_registry_hub.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_context.hpp"  // Adjust to relative path (@wolfram77)
#include "internal/catch_polyfills.hpp"  // Adjust to relative path (@wolfram77)

#include <iomanip>

namespace Catch {

namespace Detail {

    namespace {
        const int hexThreshold = 255;

        struct Endianness {
            enum Arch : uint8_t {
                Big,
                Little
            };

            static Arch which() {
                int one = 1;
                // If the lowest byte we read is non-zero, we can assume
                // that little endian format is used.
                auto value = *reinterpret_cast<char*>(&one);
                return value ? Little : Big;
            }
        };

        template<typename T>
        std::string fpToString(T value, int precision) {
            if (Catch::isnan(value)) {
                return "nan";
            }

            ReusableStringStream rss;
            rss << std::setprecision(precision)
                << std::fixed
                << value;
            std::string d = rss.str();
            std::size_t i = d.find_last_not_of('0');
            if (i != std::string::npos && i != d.size() - 1) {
                if (d[i] == '.')
                    i++;
                d = d.substr(0, i + 1);
            }
            return d;
        }
    } // end unnamed namespace

    std::size_t catch_strnlen( const char* str, std::size_t n ) {
        auto ret = std::char_traits<char>::find( str, n, '\0' );
        if ( ret != nullptr ) { return static_cast<std::size_t>( ret - str ); }
        return n;
    }

    std::string formatTimeT(std::time_t time) {
#ifdef _MSC_VER
        std::tm timeInfo = {};
        const auto err = gmtime_s( &timeInfo, &time );
        if ( err ) {
            return "gmtime from provided timepoint has failed. This "
                   "happens e.g. with pre-1970 dates using Microsoft libc";
        }
#else
        std::tm* timeInfo = std::gmtime( &time );
#endif

        auto const timeStampSize = sizeof( "2017-01-16T17:06:45Z" );
        char timeStamp[timeStampSize];
        const char* const fmt = "%Y-%m-%dT%H:%M:%SZ";

#ifdef _MSC_VER
        std::strftime( timeStamp, timeStampSize, fmt, &timeInfo );
#else
        std::strftime( timeStamp, timeStampSize, fmt, timeInfo );
#endif
        return std::string( timeStamp, timeStampSize - 1 );
    }

    std::string convertIntoString(StringRef string, bool escapeInvisibles) {
        std::string ret;
        // This is enough for the "don't escape invisibles" case, and a good
        // lower bound on the "escape invisibles" case.
        ret.reserve( string.size() + 2 );

        if ( !escapeInvisibles ) {
            ret += '"';
            ret += string;
            ret += '"';
            return ret;
        }

        size_t last_start = 0;
        auto write_to = [&]( size_t idx ) {
            if ( last_start < idx ) {
                ret += string.substr( last_start, idx - last_start );
            }
            last_start = idx + 1;
        };

        ret += '"';
        for ( size_t i = 0; i < string.size(); ++i ) {
            const char c = string[i];
            if ( c == '\r' || c == '\n' || c == '\t' || c == '\f' ) {
                write_to( i );
                if ( c == '\r' ) { ret.append( "\\r" ); }
                if ( c == '\n' ) { ret.append( "\\n" ); }
                if ( c == '\t' ) { ret.append( "\\t" ); }
                if ( c == '\f' ) { ret.append( "\\f" ); }
            }
        }
        write_to( string.size() );
        ret += '"';

        return ret;
    }

    std::string convertIntoString(StringRef string) {
        return convertIntoString(string, getCurrentContext().getConfig()->showInvisibles());
    }

    std::string rawMemoryToString( const void *object, std::size_t size ) {
        // Reverse order for little endian architectures
        int i = 0, end = static_cast<int>( size ), inc = 1;
        if( Endianness::which() == Endianness::Little ) {
            i = end-1;
            end = inc = -1;
        }

        unsigned char const *bytes = static_cast<unsigned char const *>(object);
        ReusableStringStream rss;
        rss << "0x" << std::setfill('0') << std::hex;
        for( ; i != end; i += inc )
             rss << std::setw(2) << static_cast<unsigned>(bytes[i]);
       return rss.str();
    }

    std::string makeExceptionHappenedString() {
        return "{ stringification failed with an exception: \"" +
               translateActiveException() + "\" }";

    }

} // end Detail namespace



//// ======================================================= ////
//
//   Out-of-line defs for full specialization of StringMaker
//
//// ======================================================= ////

std::string StringMaker<std::string>::convert(const std::string& str) {
    return Detail::convertIntoString( str );
}

#ifdef CATCH_CONFIG_CPP17_STRING_VIEW
std::string StringMaker<std::string_view>::convert(std::string_view str) {
    return Detail::convertIntoString( StringRef( str.data(), str.size() ) );
}
#endif

std::string StringMaker<char const*>::convert(char const* str) {
    if (str) {
        return Detail::convertIntoString( str );
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<char*>::convert(char* str) { // NOLINT(readability-non-const-parameter)
    if (str) {
        return Detail::convertIntoString( str );
    } else {
        return{ "{null string}" };
    }
}

#ifdef CATCH_CONFIG_WCHAR
std::string StringMaker<std::wstring>::convert(const std::wstring& wstr) {
    std::string s;
    s.reserve(wstr.size());
    for (auto c : wstr) {
        s += (c <= 0xff) ? static_cast<char>(c) : '?';
    }
    return ::Catch::Detail::stringify(s);
}

# ifdef CATCH_CONFIG_CPP17_STRING_VIEW
std::string StringMaker<std::wstring_view>::convert(std::wstring_view str) {
    return StringMaker<std::wstring>::convert(std::wstring(str));
}
# endif

std::string StringMaker<wchar_t const*>::convert(wchar_t const * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<wchar_t *>::convert(wchar_t * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}
#endif

#if defined(CATCH_CONFIG_CPP17_BYTE)
#include <cstddef>
std::string StringMaker<std::byte>::convert(std::byte value) {
    return ::Catch::Detail::stringify(std::to_integer<unsigned long long>(value));
}
#endif // defined(CATCH_CONFIG_CPP17_BYTE)

std::string StringMaker<int>::convert(int value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long>::convert(long value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long long>::convert(long long value) {
    ReusableStringStream rss;
    rss << value;
    if (value > Detail::hexThreshold) {
        rss << " (0x" << std::hex << value << ')';
    }
    return rss.str();
}

std::string StringMaker<unsigned int>::convert(unsigned int value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long>::convert(unsigned long value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long long>::convert(unsigned long long value) {
    ReusableStringStream rss;
    rss << value;
    if (value > Detail::hexThreshold) {
        rss << " (0x" << std::hex << value << ')';
    }
    return rss.str();
}

std::string StringMaker<signed char>::convert(signed char value) {
    if (value == '\r') {
        return "'\\r'";
    } else if (value == '\f') {
        return "'\\f'";
    } else if (value == '\n') {
        return "'\\n'";
    } else if (value == '\t') {
        return "'\\t'";
    } else if ('\0' <= value && value < ' ') {
        return ::Catch::Detail::stringify(static_cast<unsigned int>(value));
    } else {
        char chstr[] = "' '";
        chstr[1] = value;
        return chstr;
    }
}
std::string StringMaker<char>::convert(char c) {
    return ::Catch::Detail::stringify(static_cast<signed char>(c));
}
std::string StringMaker<unsigned char>::convert(unsigned char value) {
    return ::Catch::Detail::stringify(static_cast<char>(value));
}

int StringMaker<float>::precision = std::numeric_limits<float>::max_digits10;

std::string StringMaker<float>::convert(float value) {
    return Detail::fpToString(value, precision) + 'f';
}

int StringMaker<double>::precision = std::numeric_limits<double>::max_digits10;

std::string StringMaker<double>::convert(double value) {
    return Detail::fpToString(value, precision);
}

} // end namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_TOSTRING_CPP_INCLUDED
// END Amalgamated content from catch_tostring.cpp (@wolfram77)



// BEGIN Include main implementation (@wolfram77)
#ifdef CATCH2_IMPLEMENTATION
#include "internal/catch_main.cpp"
#include "catch_registry_hub.cpp"
#endif // CATCH2_IMPLEMENTATION
// END Include main implementation (@wolfram77)

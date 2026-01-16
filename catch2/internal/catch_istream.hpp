//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_ISTREAM_HPP_INCLUDED
#define CATCH_ISTREAM_HPP_INCLUDED

#include "catch_noncopyable.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)

#include <iosfwd>
#include <string>

namespace Catch {

    class IStream {
    public:
        virtual ~IStream(); // = default
        virtual std::ostream& stream() = 0;
        /**
         * Best guess on whether the instance is writing to a console (e.g. via stdout/stderr)
         *
         * This is useful for e.g. Win32 colour support, because the Win32
         * API manipulates console directly, unlike POSIX escape codes,
         * that can be written anywhere.
         *
         * Due to variety of ways to change where the stdout/stderr is
         * _actually_ being written, users should always assume that
         * the answer might be wrong.
         */
        virtual bool isConsole() const { return false; }
    };

    /**
     * Creates a stream wrapper that writes to specific file.
     *
     * Also recognizes 4 special filenames
     * * `-` for stdout
     * * `%stdout` for stdout
     * * `%stderr` for stderr
     * * `%debug` for platform specific debugging output
     *
     * \throws if passed an unrecognized %-prefixed stream
     */
    auto makeStream( std::string const& filename ) -> Detail::unique_ptr<IStream>;

}

#endif // CATCH_STREAM_HPP_INCLUDED



// BEGIN Amalgamated content from catch_istream.cpp (@wolfram77)
#ifndef CATCH_ISTREAM_CPP_INCLUDED
#define CATCH_ISTREAM_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// #include "catch_istream.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "catch_enforce.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_debug_console.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_unique_ptr.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_stdstreams.hpp"  // Adjust to relative path (@wolfram77)

#include <cstdio>
#include <fstream>

namespace Catch {

    Catch::IStream::~IStream() = default;

namespace Detail {
    namespace {
        template<typename WriterF, std::size_t bufferSize=256>
        class StreamBufImpl final : public std::streambuf {
            char data[bufferSize];
            WriterF m_writer;

        public:
            StreamBufImpl() {
                setp( data, data + sizeof(data) );
            }

            ~StreamBufImpl() noexcept override {
                StreamBufImpl::sync();
            }

        private:
            int overflow( int c ) override {
                sync();

                if( c != EOF ) {
                    if( pbase() == epptr() )
                        m_writer( std::string( 1, static_cast<char>( c ) ) );
                    else
                        sputc( static_cast<char>( c ) );
                }
                return 0;
            }

            int sync() override {
                if( pbase() != pptr() ) {
                    m_writer( std::string( pbase(), static_cast<std::string::size_type>( pptr() - pbase() ) ) );
                    setp( pbase(), epptr() );
                }
                return 0;
            }
        };

        ///////////////////////////////////////////////////////////////////////////

        struct OutputDebugWriter {

            void operator()( std::string const& str ) {
                if ( !str.empty() ) {
                    writeToDebugConsole( str );
                }
            }
        };

        ///////////////////////////////////////////////////////////////////////////

        class FileStream final : public IStream {
            std::ofstream m_ofs;
        public:
            FileStream( std::string const& filename ) {
                m_ofs.open( filename.c_str() );
                CATCH_ENFORCE( !m_ofs.fail(), "Unable to open file: '" << filename << '\'' );
                m_ofs << std::unitbuf;
            }
        public: // IStream
            std::ostream& stream() override {
                return m_ofs;
            }
        };

        ///////////////////////////////////////////////////////////////////////////

        class CoutStream final : public IStream {
            std::ostream m_os;
        public:
            // Store the streambuf from cout up-front because
            // cout may get redirected when running tests
            CoutStream() : m_os( Catch::cout().rdbuf() ) {}

        public: // IStream
            std::ostream& stream() override { return m_os; }
            bool isConsole() const override { return true; }
        };

        class CerrStream : public IStream {
            std::ostream m_os;

        public:
            // Store the streambuf from cerr up-front because
            // cout may get redirected when running tests
            CerrStream(): m_os( Catch::cerr().rdbuf() ) {}

        public: // IStream
            std::ostream& stream() override { return m_os; }
            bool isConsole() const override { return true; }
        };

        ///////////////////////////////////////////////////////////////////////////

        class DebugOutStream final : public IStream {
            Detail::unique_ptr<StreamBufImpl<OutputDebugWriter>> m_streamBuf;
            std::ostream m_os;
        public:
            DebugOutStream()
            :   m_streamBuf( Detail::make_unique<StreamBufImpl<OutputDebugWriter>>() ),
                m_os( m_streamBuf.get() )
            {}

        public: // IStream
            std::ostream& stream() override { return m_os; }
        };

    } // unnamed namespace
} // namespace Detail

    ///////////////////////////////////////////////////////////////////////////

    auto makeStream( std::string const& filename ) -> Detail::unique_ptr<IStream> {
        if ( filename.empty() || filename == "-" ) {
            return Detail::make_unique<Detail::CoutStream>();
        }
        if( filename[0] == '%' ) {
            if ( filename == "%debug" ) {
                return Detail::make_unique<Detail::DebugOutStream>();
            } else if ( filename == "%stderr" ) {
                return Detail::make_unique<Detail::CerrStream>();
            } else if ( filename == "%stdout" ) {
                return Detail::make_unique<Detail::CoutStream>();
            } else {
                CATCH_ERROR( "Unrecognised stream: '" << filename << '\'' );
            }
        }
        return Detail::make_unique<Detail::FileStream>( filename );
    }

}
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_ISTREAM_CPP_INCLUDED
// END Amalgamated content from catch_istream.cpp (@wolfram77)

/*
 * Copyright (c) 2022 Florian Becker <fb@vxapps.com> (VX APPS).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* c header */
#include <cmath>
#include <cstdint> // std::int32_t
#include <cstring> // std::memcpy

/* stl header */
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
  #include <span>
#endif
#include <string>
#include <system_error>
#include <vector>

/* sqlite header */
#include <sqlite3.h>

/* icu header */
#include <unicode/stringpiece.h>
#include <unicode/translit.h>
#include <unicode/unistr.h>
#include <unicode/utrans.h>
#include <unicode/utypes.h>

/* modern.cpp.core */
#include <StringUtils.h>

/* local header */
#include "SqliteError.h"
#include "SqliteUtils.h"

namespace std { // NOSONAR

#ifdef _WIN32
  using ::strncpy_s;
#endif
}

namespace vx::sqlite_utils {

  /**
   * @brief Degrees of a half round move.
   */
  constexpr std::int32_t halfCircleDegree = 180;

  /**
   * @brief Earth km.
   */
  constexpr double earthBlubKm = 6378.137;

#ifdef WIN32
  /** @brief Buffer size for errno. */
  constexpr std::int32_t errnoBufferSize = 128;
#endif

  void sqlite3_deleter::operator()( sqlite3 *_handle ) const noexcept {

    [[maybe_unused]] const std::int32_t resultCode = sqlite3_close( _handle );
#ifdef DEBUG
    if ( resultCode != SQLITE_OK ) {

      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << "ERROR: '" << sqlite3_errmsg( _handle ) << "'" << std::endl;
      std::cout << std::endl;
    }
#endif
  }

  void sqlite3_stmt_deleter::operator()( sqlite3_stmt *_statement ) const noexcept {

    [[maybe_unused]] const std::int32_t resultCode = sqlite3_finalize( _statement );
#ifdef DEBUG
    if ( resultCode != SQLITE_OK ) {

      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << std::endl;
    }
#endif
  }

  void sqlite3_generic_deleter::operator()( void *_what ) const noexcept { // NOSONAR more meaningful than void

    sqlite3_free( _what );
  }

  void sqlite3_str_deleter::operator()( char *_what ) const noexcept {

    sqlite3_generic_deleter {}( static_cast<void *>( _what ) );
  }

  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename ) noexcept {

    std::error_code error {};
    return sqlite3_make_unique( _filename, error );
  }

  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename,
                                                                 std::error_code &_error ) noexcept {

    sqlite3 *handle = nullptr;
    const std::int32_t resultCode = sqlite3_open( _filename.c_str(), &handle );
    std::unique_ptr<sqlite3, sqlite3_deleter> database { handle };
    if ( resultCode != SQLITE_OK ) {

      _error.clear();
      SqliteErrorCategory::instance().setMessage( sqlite3_errmsg( handle ) );
      _error = { resultCode, SqliteErrorCategory::instance() };

#ifdef DEBUG
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << "ERROR: '" << sqlite3_errmsg( handle ) << "'" << std::endl;
      std::cout << std::endl;
#endif
      database.reset();
    }

    return database;
  }

  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_handle,
                                                                                const std::string &_sql ) noexcept {

    std::error_code error {};
    return sqlite3_stmt_make_unique( _handle, _sql, error );
  }

  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_handle,
                                                                                const std::string &_sql,
                                                                                std::error_code &_error ) noexcept {

    sqlite3_stmt *statementHandle = nullptr;
    const std::int32_t resultCode = sqlite3_prepare_v2( _handle, _sql.c_str(), -1, &statementHandle, nullptr );
    std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> statement { statementHandle };
    if ( resultCode != SQLITE_OK ) {

      _error.clear();
      SqliteErrorCategory::instance().setMessage( sqlite3_errmsg( _handle ) );
      _error = { resultCode, SqliteErrorCategory::instance() };

#ifdef DEBUG
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << "ERROR: '" << sqlite3_errmsg( _handle ) << "'" << std::endl;
      std::cout << std::endl;
#endif
      statement.reset();
    }
    return statement;
  }

  std::error_code importDump( sqlite3 *_handle,
                              const std::string &_schema,
                              const std::string &_filename ) {

    if ( std::error_code errorCode {}; !std::filesystem::exists( _filename, errorCode ) || errorCode ) {

      SqliteErrorCategory::instance().setMessage( "File not found." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }

    std::vector<char> dump {};
    std::ifstream input( _filename, std::ios::in | std::ios::binary );
    if ( !input.is_open() ) {

      SqliteErrorCategory::instance().setMessage( "Cannot open file for import." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }
    if ( !input.eof() && !input.fail() ) {

      input.seekg( 0, std::ios_base::end );
      const std::streampos size = input.tellg();
      dump.resize( static_cast<std::size_t>( size ) );

      input.seekg( 0, std::ios_base::beg );
      input.read( dump.data(), size );
    }
    try {

      input.close();
    }
    catch ( const std::ofstream::failure &_exception ) {

      std::cout << _exception.what() << std::endl;
      SqliteErrorCategory::instance().setMessage( "Unable to close the import file." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }

    std::vector<std::uint8_t> converted( std::cbegin( dump ), std::cend( dump ) );
    if ( converted.empty() ) {

      SqliteErrorCategory::instance().setMessage( "Cannot read data from file." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }

    auto *databuffer( static_cast<std::uint8_t *>( sqlite3_malloc64( sizeof( std::uint8_t ) * converted.size() ) ) );
    std::memcpy( databuffer, converted.data(), converted.size() );

    if ( const std::int32_t resultCode = sqlite3_deserialize( _handle, _schema.c_str(), databuffer, static_cast<sqlite3_int64>( converted.size() ), static_cast<sqlite3_int64>( converted.size() ), SQLITE_DESERIALIZE_RESIZEABLE | SQLITE_DESERIALIZE_FREEONCLOSE ); resultCode != SQLITE_OK ) {

      SqliteErrorCategory::instance().setMessage( sqlite3_errmsg( _handle ) );
      return { resultCode, SqliteErrorCategory::instance() };
    }

    return {};
  }

  std::error_code exportDump( sqlite3 *_handle,
                              const std::string &_schema,
                              const std::string &_filename ) {

    /* Dump database */
    sqlite3_int64 serializationSize = 0;
    const std::unique_ptr<std::uint8_t, sqlite3_generic_deleter> dump( sqlite3_serialize( _handle, _schema.c_str(), &serializationSize, 0 ) );
    if ( !dump || serializationSize == 0 ) {

      SqliteErrorCategory::instance().setMessage( "Export empty or invalid." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }

    std::vector<char> converted( dump.get(), dump.get() + serializationSize );
    if ( converted.empty() ) {

      SqliteErrorCategory::instance().setMessage( "Export not convertable." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }

    std::ofstream output( _filename, std::ios::out | std::ios::binary );
    if ( !output.is_open() ) {

      SqliteErrorCategory::instance().setMessage( "Cannot open file for export." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }
    try {

      output.write( converted.data(), static_cast<std::streamsize>( converted.size() ) );
      output.close();
    }
    catch ( const std::ofstream::failure &_exception ) {

      std::cout << _exception.what() << std::endl;
      SqliteErrorCategory::instance().setMessage( "Unable to write or close the export file." );
      return { SQLITE_IOERR, SqliteErrorCategory::instance() };
    }

    return {};
  }

  namespace {

    /**
   * @brief Generate own PI.
   * @return Calculated pi value.
   */
    double pi() { return std::atan( 1 ) * 4; }
  }

  void distance( sqlite3_context *_context,
                 std::int32_t _argc,
                 sqlite3_value **_argv ) noexcept {

    /* Parameter count mismatch */
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const std::span args( _argv, static_cast<std::size_t>( _argc ) );
    if ( args.size() != 4 ) {
#else
    if ( _argc != 4 ) {
#endif

#ifdef DEBUG
      std::cout << "DISTANCE: Parameter mismatch." << std::endl;
#endif
      sqlite3_result_null( _context );
      return;
    }

    /* Required parameter empty */
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    for ( const auto &arg : args ) {

      if ( sqlite3_value_type( arg ) == SQLITE_NULL ) {

  #ifdef DEBUG
        std::cout << "DISTANCE: Parameter mismatch." << std::endl;
  #endif
        sqlite3_result_null( _context );
        return;
      }
    }
#else
    for ( std::int32_t x = 0; x < _argc; x++ ) {

      if ( sqlite3_value_type( _argv[ x ] ) == SQLITE_NULL ) {

        sqlite3_result_null( _context );
        return;
      }
    }
#endif

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const double latitude1 = sqlite3_value_double( args[ 0 ] );
    const double longitude1 = sqlite3_value_double( args[ 1 ] );
    const double latitude2 = sqlite3_value_double( args[ 2 ] );
    const double longitude2 = sqlite3_value_double( args[ 3 ] );
#else
    const double latitude1 = sqlite3_value_double( _argv[ 0 ] );
    const double longitude1 = sqlite3_value_double( _argv[ 1 ] );
    const double latitude2 = sqlite3_value_double( _argv[ 2 ] );
    const double longitude2 = sqlite3_value_double( _argv[ 3 ] );
#endif
    sqlite3_result_double( _context, std::acos( sin( latitude1 / halfCircleDegree * pi() ) * std::sin( latitude2 / halfCircleDegree * pi() ) + std::cos( latitude1 / halfCircleDegree * pi() ) * std::cos( latitude2 / halfCircleDegree * pi() ) * std::cos( ( longitude2 / halfCircleDegree * pi() ) - ( longitude1 / halfCircleDegree * pi() ) ) ) * earthBlubKm );
  }

  void transliteration( sqlite3_context *_context,
                        std::int32_t _argc,
                        sqlite3_value **_argv ) {

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const std::span args( _argv, static_cast<std::size_t>( _argc ) );
    if ( args.size() != 1 && ( sqlite3_value_type( args[ 0 ] ) == SQLITE_NULL ) ) {
#else
    if ( _argc != 1 && ( sqlite3_value_type( _argv[ 0 ] ) == SQLITE_NULL ) ) {
#endif

#ifdef DEBUG
      std::cout << "TRANSLITERATION: Parameter mismatch." << std::endl;
#endif
      sqlite3_result_null( _context );
      return;
    }

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const std::optional input = string_utils::fromUnsignedChar( sqlite3_value_text( args[ 0 ] ) );
#else
    const std::optional input = string_utils::fromUnsignedChar( sqlite3_value_text( _argv[ 0 ] ) );
#endif

    const std::string delimiter = ";";
    const auto join = [ &delimiter ]( const std::string &_str,
                                      const std::string &_part ) {
      return _str + ( _str.empty() ? std::string() : delimiter ) + _part;
    };
    const std::vector<std::string> transliteratorRules = { "Any-Latin",
                                                           "[:Nonspacing Mark:] Remove",
                                                           "[:Punctuation:] Remove",
                                                           "[:Symbol:] Remove",
                                                           "Latin-ASCII" };
    const std::string result = std::accumulate( std::cbegin( transliteratorRules ), std::cend( transliteratorRules ), std::string(), join );

    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<icu::Transliterator> transliterator { icu::Transliterator::createInstance( icu::UnicodeString::fromUTF8( icu::StringPiece( result ) ), UTRANS_FORWARD, status ) };
    if ( U_FAILURE( status ) ) {

#ifdef DEBUG
      std::cout << "TRANSLITERATION: Cannot create transliterator." << std::endl;
#endif
      sqlite3_result_null( _context );
      return;
    }

    icu::UnicodeString data = icu::UnicodeString::fromUTF8( icu::StringPiece( input.value_or( "" ) ) );
    transliterator->transliterate( data );

    std::string str {};
    data.toUTF8String( str );
    string_utils::simplified( str );

    auto *databuffer( static_cast<char *>( sqlite3_malloc64( sizeof( char ) * str.size() ) ) );
#ifdef _WIN32
    errno_t error = std::strncpy_s( databuffer, str.size() + 1, str.data(), str.size() );
    if ( error ) {

      std::vector<char> errnoBuffer {};
      try {

        errnoBuffer.resize( errnoBufferSize );
      }
      catch ( const std::bad_alloc &_exception ) {

        logFatal() << "bad_alloc:" << _exception.what();
      }
      catch ( const std::exception &_exception ) {

        logFatal() << _exception.what();
      }
      std::ignore = strerror_s( errnoBuffer.data(), errnoBuffer.size(), error );
      logError() << std::string( std::cbegin( errnoBuffer ), std::cend( errnoBuffer ) );
    }
#else
    std::strncpy( databuffer, str.data(), str.size() );
#endif

    sqlite3_result_text( _context, databuffer, static_cast<int>( str.size() ), sqlite3_free );
  }

  std::int32_t outputCallback( [[maybe_unused]] void *_data, // NOSONAR more meaningful than void
                               std::int32_t _argc,
                               char **_argv,
                               char **_columns ) noexcept {

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const std::span args( _argv, static_cast<std::size_t>( _argc ) );
    const std::span columns( _columns, static_cast<std::size_t>( _argc ) );
    for ( std::size_t i = 0; i < args.size(); ++i ) {

      std::cout << columns[ i ] << " = " << ( args[ i ] ? args[ i ] : "NULL" ) << std::endl;
    }
#else
    for ( std::int32_t i = 0; i < _argc; ++i ) {

      std::cout << _columns[ i ] << " = " << ( _argv[ i ] ? _argv[ i ] : "NULL" ) << std::endl;
    }
#endif
    std::cout << std::endl;
    return 0;
  }
}

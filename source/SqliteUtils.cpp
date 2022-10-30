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
#include <cstddef> // std::byte
#include <cstring> // std::memcpy

/* stl header */
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
  #include <span>
#endif
#include <vector>

/* sqlite header */
#include <sqlite3.h>

/* icu header */
#include <unicode/translit.h>
#include <unicode/unistr.h>

/* modern.cpp.core */
#include <StringUtils.h>

/* local header */
#include "SqliteUtils.h"

namespace vx::sqlite_utils {

  constexpr int halfCircleDegree = 180;

  constexpr double earthBlubKm = 6378.137;

  void sqlite3_deleter::operator()( sqlite3 *_handle ) const {

    [[maybe_unused]] const int resultCode = sqlite3_close( _handle );
#ifdef DEBUG
    if ( resultCode != SQLITE_OK ) {

      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << "ERROR: '" << sqlite3_errmsg( _handle ) << "'" << std::endl;
      std::cout << std::endl;
    }
#endif
  }

  void sqlite3_stmt_deleter::operator()( sqlite3_stmt *_statement ) const {

    [[maybe_unused]] const int resultCode = sqlite3_finalize( _statement );
#ifdef DEBUG
    if ( resultCode != SQLITE_OK ) {

      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << std::endl;
    }
#endif
  }

  void sqlite3_generic_deleter::operator()( void *_what ) const { // NOSONAR more meaningful than void

    sqlite3_free( _what );
  }

  void sqlite3_str_deleter::operator()( char *_what ) const {

    sqlite3_generic_deleter {}( static_cast<void *>( _what ) );
  }

  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename ) {

    sqlite3 *databaseHandle = nullptr;
    const int resultCode = sqlite3_open( _filename.c_str(), &databaseHandle );
    std::unique_ptr<sqlite3, sqlite3_deleter> database { databaseHandle };
    if ( resultCode != SQLITE_OK ) {

#ifdef DEBUG
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << "ERROR: '" << sqlite3_errmsg( databaseHandle ) << "'" << std::endl;
      std::cout << std::endl;
#endif
      database.reset();
    }
    return database;
  }

  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_handle, //std::unique_ptr<sqlite3, sqlite3_deleter> _database,
                                                                                const std::string &_sql ) {

    sqlite3_stmt *statementHandle = nullptr;
    const int resultCode = sqlite3_prepare_v2( _handle, _sql.c_str(), -1, &statementHandle, nullptr );
    std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> statement { statementHandle };
    if ( resultCode != SQLITE_OK ) {

#ifdef DEBUG
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << std::endl;
#endif
      statement.reset();
    }
    return statement;
  }

  std::tuple<int, std::string> importDump( sqlite3 *_handle,
                                           const std::string &_schema,
                                           const std::string &_filename ) noexcept {

    if ( std::error_code errorCode {}; !std::filesystem::exists( _filename, errorCode ) || errorCode ) {

      return { SQLITE_IOERR, "File not found." };
    }

    std::vector<char> dump {};
    std::ifstream input( _filename, std::ios::in | std::ios::binary );
    if ( !input.is_open() ) {

      return { SQLITE_IOERR, "Cannot open file for import." };
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
      return { SQLITE_IOERR, "Unable to close the import file." };
    }

    std::vector<unsigned char> converted( std::begin( dump ), std::end( dump ) );
    if ( converted.empty() ) {

      return { SQLITE_IOERR, "Cannot read data from file." };
    }

    auto *databuffer( static_cast<unsigned char *>( sqlite3_malloc64( sizeof( unsigned char ) * converted.size() ) ) );
    std::memcpy( databuffer, converted.data(), converted.size() );

    if ( const int resultCode = sqlite3_deserialize( _handle, _schema.c_str(), databuffer, static_cast<sqlite3_int64>( converted.size() ), static_cast<sqlite3_int64>( converted.size() ), SQLITE_DESERIALIZE_RESIZEABLE | SQLITE_DESERIALIZE_FREEONCLOSE ); resultCode != SQLITE_OK ) {

      return { resultCode, sqlite3_errmsg( _handle ) };
    }

    return {};
  }

  std::tuple<int, std::string> exportDump( sqlite3 *_handle,
                                           const std::string &_schema,
                                           const std::string &_filename ) noexcept {

    /* Dump database */
    sqlite3_int64 serializationSize = 0;
    const std::unique_ptr<unsigned char, sqlite3_generic_deleter> dump( sqlite3_serialize( _handle, _schema.c_str(), &serializationSize, 0 ) );
    if ( !dump || serializationSize == 0 ) {

      return { SQLITE_IOERR, "Export empty or invalid." };
    }

    std::vector<char> converted( dump.get(), dump.get() + serializationSize );
    if ( converted.empty() ) {

      return { SQLITE_IOERR, "Export not convertable." };
    }

    std::ofstream output( _filename, std::ios::out | std::ios::binary );
    if ( !output.is_open() ) {

      return { SQLITE_IOERR, "Cannot open file for export." };
    }
    try {

      output.write( converted.data(), static_cast<std::streamsize>( converted.size() ) );
      output.close();
    }
    catch ( const std::ofstream::failure &_exception ) {

      std::cout << _exception.what() << std::endl;
      return { SQLITE_IOERR, "Unable to write or close the export file." };
    }

    return {};
  }

  void distance( sqlite3_context *_context,
                 int _argc,
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
    for ( int x = 0; x < _argc; x++ ) {

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
    sqlite3_result_double( _context, std::acos( sin( latitude1 / halfCircleDegree * M_PI ) * std::sin( latitude2 / halfCircleDegree * M_PI ) + std::cos( latitude1 / halfCircleDegree * M_PI ) * std::cos( latitude2 / halfCircleDegree * M_PI ) * std::cos( ( longitude2 / halfCircleDegree * M_PI ) - ( longitude1 / halfCircleDegree * M_PI ) ) ) * earthBlubKm );
  }

  void transliteration( sqlite3_context *_context,
                        int _argc,
                        sqlite3_value **_argv ) noexcept {

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
    std::strncpy( databuffer, str.data(), str.size() );

    sqlite3_result_text( _context, databuffer, static_cast<int>( str.size() ), sqlite3_free );
  }

  int outputCallback( [[maybe_unused]] void *_data, // NOSONAR more meaningful than void
                      int _argc,
                      char **_argv,
                      char **_columns ) noexcept {

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const std::span args( _argv, static_cast<std::size_t>( _argc ) );
    const std::span columns( _columns, static_cast<std::size_t>( _argc ) );
    for ( std::size_t i = 0; i < args.size(); ++i ) {

      std::cout << columns[ i ] << " = " << ( args[ i ] ? args[ i ] : "NULL" ) << std::endl;
    }
#else
    for ( int i = 0; i < _argc; ++i ) {

      std::cout << _columns[ i ] << " = " << ( _argv[ i ] ? _argv[ i ] : "NULL" ) << std::endl;
    }
#endif
    std::cout << std::endl;
    return 0;
  }
}

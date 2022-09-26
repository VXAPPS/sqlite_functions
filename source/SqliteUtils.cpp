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

/* stl header */
#include <filesystem>
#include <fstream>
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
  #include <span>
#endif
#include <vector>

/* sqlite header */
#include <sqlite3.h>

/* local header */
#include "SqliteUtils.h"

/*namespace std {

  enum class byte : unsigned char {} ;
} */

namespace vx::sqlite_utils {

  constexpr int halfCircleDegree = 180;

  constexpr double earthBlubKm = 6378.137;

  void sqlite3_deleter::operator()( sqlite3 *_handle ) const {

    [[maybe_unused]] const int resultCode = sqlite3_close( _handle );
#ifdef DEBUG
    if ( resultCode != SQLITE_OK ) {

      std::cout << __PRETTY_FUNCTION__ << std::endl;
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

      std::cout << __PRETTY_FUNCTION__ << std::endl;
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << std::endl;
    }
#endif
  }

  void sqlite3_generic_deleter::operator()( void *_what ) const {

    sqlite3_free( _what );
  }

  void sqlite3_str_deleter::operator()(char* _what) const {

    sqlite3_generic_deleter{}(static_cast<void*>(_what));
  }

  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename ) {

    sqlite3 *databaseHandle = nullptr;
    const int resultCode = sqlite3_open( _filename.c_str(), &databaseHandle );
    std::unique_ptr<sqlite3, sqlite3_deleter> database { databaseHandle };
    if ( resultCode != SQLITE_OK ) {

#ifdef DEBUG
      std::cout << __PRETTY_FUNCTION__ << std::endl;
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
      std::cout << __PRETTY_FUNCTION__ << std::endl;
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << std::endl;
#endif
      statement.reset();
    }
    return statement;
  }

  int import_dump( sqlite3 *_handle,
                   const std::string &_schema,
                   const std::string &_filename ) {

    if ( !std::filesystem::exists( _filename ) ) {

      return SQLITE_OK;
    }
    /* Dump database */
    /*    sqlite3_int64 serializationSize = 0;
        std::unique_ptr<unsigned char, sqlite3_dump_deleter> dump( sqlite3_serialize( _handle, _schema.c_str(), &serializationSize, 0 ) );
        if ( serializationSize == 0 ) {

          return SQLITE_ERROR;
        }

        std::vector<char> converted( dump.get(), dump.get() + serializationSize );
        if ( converted.empty() ) {

          return SQLITE_ERROR;
        }

        std::ofstream output;
        output.open( _filename, std::ofstream::out | std::ofstream::binary );
        output.write( converted.data(), static_cast<std::streamsize>( converted.size() ) );
        output.close(); */

    // TODO(FB) Fehlerbehandlung und Exceptions - wenn ich es nicht schreiben kann? Wenn nicht genügend Platz vorhanden ist usw. usw.
    std::vector<char> dump {};
    std::ifstream input( _filename, std::ios::in | std::ios::binary );
    if ( !input.eof() && !input.fail() ) {

      input.seekg( 0, std::ios_base::end );
      const std::streampos size = input.tellg();
      std::cout << "input2 " << size << std::endl;
      dump.resize( static_cast<std::size_t>( size ) );

      input.seekg( 0, std::ios_base::beg );
      input.read( dump.data(), size );
    }
    input.close();

    std::cout << "input " << dump.size() << std::endl;
    std::cout << "input " << dump.data() << std::endl;
    std::vector<unsigned char> converted( std::begin( dump ), std::end( dump ) );
    if ( converted.empty() ) {

      return SQLITE_ERROR;
    }
    std::cout << "input neu " << converted.size() << std::endl;
//    std::vector<unsigned char> converted {};
//    std::copy(dump.begin(), dump.end(), std::back_inserter(converted));
//    std::streamoff fs = input.tellg();


//    input >> data.data();
//    input.close();

    // copies all data into buffer
//    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), fs);
//    std::cout << "buffer " << buffer.size() << std::endl;

    const int resultCode = sqlite3_deserialize( _handle, _schema.c_str(), converted.data(), static_cast<sqlite3_int64>( converted.size() ), static_cast<sqlite3_int64>( converted.size() ), 0 ); //, SQLITE_DESERIALIZE_RESIZEABLE | SQLITE_DESERIALIZE_FREEONCLOSE );
    if ( resultCode != SQLITE_OK ) {

#ifdef DEBUG
      std::cout << __PRETTY_FUNCTION__ << std::endl;
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << "ERROR: '" << sqlite3_errmsg( _handle ) << "'" << std::endl;
      std::cout << std::endl;
#endif
    }

    return 0;
  }

  int export_dump( sqlite3 *_handle,
                   const std::string &_schema,
                   const std::string &_filename ) {

    /* Dump database */
    sqlite3_int64 serializationSize = 0;
    const std::unique_ptr<unsigned char, sqlite3_generic_deleter> dump( sqlite3_serialize( _handle, _schema.c_str(), &serializationSize, 0 ) );
    if ( !dump || serializationSize == 0 ) {

      return SQLITE_ERROR;
    }

    std::vector<char> converted( dump.get(), dump.get() + serializationSize );
    std::cout << "serialsize " << serializationSize << std::endl;
    if ( converted.empty() ) {

      return SQLITE_ERROR;
    }

    // TODO(FB) Fehlerbehandlung und Exceptions - wenn ich es nicht schreiben kann? Wenn nicht genügend Platz vorhanden ist usw. usw.
    std::cout << "output " << converted.size() << std::endl;
    std::cout << "output " << converted.data() << std::endl;
    std::ofstream output( _filename, std::ios::out | std::ios::binary );
    output.write( converted.data(), static_cast<std::streamsize>( converted.size() ) );
    output.close();

    return SQLITE_OK;
  }

  void distance( sqlite3_context *_context,
                 int _argc,
                 sqlite3_value **_argv ) {

    /* Parameter count mismatch */
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const std::span args( _argv, static_cast<std::size_t>( _argc ) );
    if ( args.size() != 4 ) {
#else
    if ( _argc != 4 ) {
#endif

      sqlite3_result_null( _context );
      return;
    }

    /* Required parameter empty */
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    for ( const auto &arg : args ) {

      if ( sqlite3_value_type( arg ) == SQLITE_NULL ) {

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

  void ascii( sqlite3_context *_context,
              int _argc,
              sqlite3_value **_argv ) {

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    const std::span args( _argv, static_cast<std::size_t>( _argc ) );
    if ( args.size() != 1 && ( sqlite3_value_type( args[ 0 ] ) == SQLITE_NULL ) ) {
#else
    if ( _argc != 1 && ( sqlite3_value_type( _argv[ 0 ] ) == SQLITE_NULL ) ) {
#endif

      sqlite3_result_null( _context );
      return;
    }

//    NSString *text = [NSString stringWithUTF8String:( char * )sqlite3_value_text( argv[0] )];
//    text = [text lowercaseString];
//    text = [text stringByReplacingOccurrencesOfString:@"ß" withString:@"ss"];
    /* auch noch fehlerhaft geschriebene umlaute suchbar machen!!! */
//    text = [text stringByReplacingOccurrencesOfString:@"ae" withString:@"a"];
//    text = [text stringByReplacingOccurrencesOfString:@"oe" withString:@"o"];
//    text = [text stringByReplacingOccurrencesOfString:@"ue" withString:@"u"];
//    NSData *data = [text dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
//    text = [[[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding] autorelease];
//    sqlite3_result_text( context, [text cStringUsingEncoding:NSASCIIStringEncoding], [text length], NULL );
  }

  int output_callback( [[maybe_unused]] void *_data,
                       int _argc,
                       char **_argv,
                       char **_columns ) {

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

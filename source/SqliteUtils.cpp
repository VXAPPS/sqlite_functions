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

/* stl header */
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
#include <span>
#endif

/* sqlite header */
#include <sqlite3.h>

/* local header */
#include "SqliteUtils.h"

namespace vx::sqlite_utils {

  constexpr int halfCircleDegree = 180;

  constexpr double earthBlubKm = 6378.137;

  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename ) {

    sqlite3 *databaseHandle = nullptr;
    int resultCode = sqlite3_open( _filename.c_str(), &databaseHandle );
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

  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_database, //std::unique_ptr<sqlite3, sqlite3_deleter> _database,
                                                                                const std::string &_sql ) {

    sqlite3_stmt *statementHandle = nullptr;
    int resultCode = sqlite3_prepare_v2( _database, _sql.c_str(), -1, &statementHandle, nullptr );
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

  int output_callback( [[maybe_unused]] void *_data,
                       int _argc,
                       char **_argv,
                       char **_columns ) {

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    std::span args( _argv, static_cast<std::size_t>( _argc ) );
    std::span columns( _columns, static_cast<std::size_t>( _argc ) );
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

  void distance( sqlite3_context *_context,
                 int _argc,
                 sqlite3_value **_argv ) {

    /* Parameter count mismatch */
#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    std::span args( _argv, static_cast<std::size_t>( _argc ) );
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
    double latitude1 = sqlite3_value_double( args[ 0 ] );
    double longitude1 = sqlite3_value_double( args[ 1 ] );
    double latitude2 = sqlite3_value_double( args[ 2 ] );
    double longitude2 = sqlite3_value_double( args[ 3 ] );
#else
    double latitude1 = sqlite3_value_double( _argv[ 0 ] );
    double longitude1 = sqlite3_value_double( _argv[ 1 ] );
    double latitude2 = sqlite3_value_double( _argv[ 2 ] );
    double longitude2 = sqlite3_value_double( _argv[ 3 ] );
#endif
    sqlite3_result_double( _context, std::acos( sin( latitude1 / halfCircleDegree * M_PI ) * std::sin( latitude2 / halfCircleDegree * M_PI ) + std::cos( latitude1 / halfCircleDegree * M_PI ) * std::cos( latitude2 / halfCircleDegree * M_PI ) * std::cos( ( longitude2 / halfCircleDegree * M_PI ) - ( longitude1 / halfCircleDegree * M_PI ) ) ) * earthBlubKm );
  }

  void ascii( sqlite3_context *_context,
              int _argc,
              sqlite3_value **_argv ) {

#if __cplusplus > 201703L && ( defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1926 || defined __clang__ && __clang_major__ >= 10 )
    std::span args( _argv, static_cast<std::size_t>( _argc ) );
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
}

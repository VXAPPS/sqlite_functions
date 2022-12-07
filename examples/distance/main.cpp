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

/* stl header */
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>

/* sqlite header */
#include <sqlite3.h>

/* sqlite_functions */
#include <SqliteUtils.h>

/* modern.cpp.core */
#include <StringUtils.h>

/*
 * SQL DATA
 *      city | latitude | longitude
 * Munich    |  48.1375 |    11.575
 * New York  |  40.6943 |  -73.9249
 * Tokyo     |  35.6839 |  139.7744
 */

static std::int32_t printResultAndExit( std::int32_t _code,
                                        const std::string &_message,
                                        const std::string &_sql ) {

  std::cout << "RESULT CODE: (" << _code << ")" << std::endl;
  std::cout << "ERROR: '" << _message << "'" << std::endl;
  std::cout << "SQL: '" << _sql << "'" << std::endl;
  std::cout << std::endl;
  return EXIT_FAILURE;
}

std::int32_t main() {

  /* Open database */
  std::error_code error_code {};
  const auto database { vx::sqlite_utils::sqlite3_make_unique( ":memory:", error_code ) };
  if ( !database ) {

    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  std::int32_t resultCode = sqlite3_create_function_v2( database.get(), "distance", 4, SQLITE_UTF8, nullptr, &vx::sqlite_utils::distance, nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Create table */
  std::string sql = "CREATE TABLE cities (city STRING, latitude REAL, longitude REAL)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

  /* Insert data */
  sql = "INSERT INTO cities VALUES('New York', 40.6943, -73.9249)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

  sql = "INSERT INTO cities VALUES('Tokyo', 35.6839, 139.7744)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

  sql = "INSERT INTO cities VALUES('Munich', 48.1375, 11.575)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

  /* Calculate a distance from select */
  /* std::string city = "Berlin"; */ // NOSONAR Alternative code
  constexpr double latitude = 52.5167;
  constexpr double longitude = 13.3833;

#ifdef DEBUG
  const std::string sql2Expect = "SELECT DISTANCE(latitude, longitude, 52.5167, 13.3833) AS distance FROM cities WHERE city IS 'Munich'";
#endif

  sql = "SELECT DISTANCE(latitude, longitude, ?1, ?2) AS distance FROM cities WHERE city IS 'Munich'";
  const auto statement = vx::sqlite_utils::sqlite3_stmt_make_unique( database.get(), sql, error_code );
  resultCode = sqlite3_bind_double( statement.get(), 1, latitude );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }
  resultCode = sqlite3_bind_double( statement.get(), 2, longitude );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

#ifdef DEBUG
  if ( const std::unique_ptr<char, vx::sqlite_utils::sqlite3_str_deleter> expandedSql { sqlite3_expanded_sql( statement.get() ) }; sql2Expect != expandedSql.get() ) {

    std::cout << "RESULT AFTER BIND MISMATCH" << std::endl;
    std::cout << "EXPECT: '" << sql2Expect << "'" << std::endl;
    std::cout << "GOT: '" << expandedSql << "'" << std::endl;
    return EXIT_FAILURE;
  }
#endif

  while ( ( resultCode = sqlite3_step( statement.get() ) ) == SQLITE_ROW ) {

    const double distance = sqlite3_column_double( statement.get(), 0 );
    /* Everything inside sqlite3 is handled as text! */
    /* std::optional distance = string_utils::fromUnsignedChar( sqlite3_column_text( statement.get(), 0 ) ); */ // NOSONAR Alternative code
    /* Begin to print out the complete double precision! - this is just needed once! */
    std::cout << std::setprecision( std::numeric_limits<double>::digits10 ) << std::endl;
    std::cout << sqlite3_column_name( statement.get(), 0 ) << " = " << distance << std::endl;
    std::cout << std::endl;
  }
  if ( resultCode != SQLITE_DONE ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

#ifdef DEBUG
  resultCode = sqlite3_exec( database.get(), sql2Expect.c_str(), vx::sqlite_utils::outputCallback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }
#endif

  sql = "SELECT DISTANCE(latitude, longitude, 52.5167, 13.3833) AS distance FROM cities WHERE city IS 'New York'";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::outputCallback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

  sql = "SELECT DISTANCE(latitude, longitude, 52.5167, 13.3833) AS distance FROM cities WHERE city IS 'Tokyo'";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::outputCallback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    return printResultAndExit( resultCode, sqlite3_errmsg( database.get() ), sql );
  }

  return EXIT_SUCCESS;
}

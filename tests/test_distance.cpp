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

/* cppunit header */
#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Weverything"
#endif
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Weffc++"
#endif
#include <gtest/gtest.h>
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

/* sqlite header */
#include <sqlite3.h>

/* sqlite_functions */
#include <SqliteUtils.h>

using ::testing::InitGoogleTest;
using ::testing::Test;

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wglobal-constructors"
#endif
namespace vx {

  /*
   * SQL DATA
   *      city | latitude | longitude
   * Munich    |  48.1375 |    11.575
   * New York  |  40.6943 |  -73.9249
   * Tokyo     |  35.6839 |  139.7744
   */

  TEST( Distance, Calc ) {

    /* Open database */
    const std::unique_ptr<sqlite3, sqlite_utils::sqlite3_deleter> database { sqlite_utils::sqlite3_make_unique( ":memory:" ) };
    if ( !database ) {

      GTEST_FAIL() << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    int resultCode = sqlite3_create_function_v2( database.get(), "distance", 4, SQLITE_UTF8, nullptr, &sqlite_utils::distance, nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    /* Create table */
    std::string sql = "CREATE TABLE cities (city STRING, latitude REAL, longitude REAL)";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    /* Insert data */
    sql = "INSERT INTO cities VALUES('New York', 40.6943, -73.9249)";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO cities VALUES('Tokyo', 35.6839, 139.7744)";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO cities VALUES('Munich', 48.1375, 11.575)";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    /* Calculate a distance from select */
    /* std::string city = "Berlin"; */ // NOSONAR Alternative code.
    constexpr double latitude = 52.5167;
    constexpr double longitude = 13.3833;

    sql = "SELECT DISTANCE(latitude, longitude, ?1, ?2) AS distance FROM cities WHERE city IS 'Munich'";
    const auto statement = sqlite_utils::sqlite3_stmt_make_unique( database.get(), sql );
    resultCode = sqlite3_bind_double( statement.get(), 1, latitude );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }
    resultCode = sqlite3_bind_double( statement.get(), 2, longitude );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    while ( ( resultCode = sqlite3_step( statement.get() ) ) == SQLITE_ROW ) {

      const double distance = sqlite3_column_double( statement.get(), 0 );
      EXPECT_EQ( distance, 504.100899610037 );
    }
    if ( resultCode != SQLITE_DONE ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "SELECT DISTANCE(latitude, longitude, ?1, ?2) AS distance FROM cities WHERE city IS 'New York'";
    const auto statementNY = sqlite_utils::sqlite3_stmt_make_unique( database.get(), sql );
    resultCode = sqlite3_bind_double( statementNY.get(), 1, latitude );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }
    resultCode = sqlite3_bind_double( statementNY.get(), 2, longitude );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    while ( ( resultCode = sqlite3_step( statementNY.get() ) ) == SQLITE_ROW ) {

      const double distance = sqlite3_column_double( statementNY.get(), 0 );
      EXPECT_DOUBLE_EQ( distance, 6387.48357973918 );
    }
    if ( resultCode != SQLITE_DONE ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "SELECT DISTANCE(latitude, longitude, ?1, ?2) AS distance FROM cities WHERE city IS 'Tokyo'";
    const auto statementT = sqlite_utils::sqlite3_stmt_make_unique( database.get(), sql );
    resultCode = sqlite3_bind_double( statementT.get(), 1, latitude );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }
    resultCode = sqlite3_bind_double( statementT.get(), 2, longitude );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    while ( ( resultCode = sqlite3_step( statementT.get() ) ) == SQLITE_ROW ) {

      const double distance = sqlite3_column_double( statementT.get(), 0 );
      EXPECT_DOUBLE_EQ( distance, 8931.60465248976 );
    }
    if ( resultCode != SQLITE_DONE ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }
  }
}
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

int main( int argc, char **argv ) {

  InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
/*
 * Copyright (c) 2023 Florian Becker <fb@vxapps.com> (VX APPS).
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
#include <filesystem>

/* gtest header */
#include <gtest/gtest.h>

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

  constexpr std::string_view exportFilename = "dump.sql";

  TEST( Dump, Export ) {

    /* Open database */
    std::error_code error {};
    const auto database { sqlite_utils::sqlite3_make_unique( ":memory:", error ) };
    if ( !database ) {

      GTEST_FAIL() << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    /* Create table */
    std::string sql = "CREATE TABLE cities (city STRING, latitude REAL, longitude REAL)";
    std::int32_t resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
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

    error = sqlite_utils::exportDump( database.get(), "main", std::string( exportFilename ) );
    EXPECT_FALSE( error );
    EXPECT_TRUE( std::filesystem::exists( exportFilename ) );
  }

  TEST( Dump, Import ) {

    EXPECT_TRUE( std::filesystem::exists( exportFilename ) );

    /* Open database */
    std::error_code error {};
    const auto database { sqlite_utils::sqlite3_make_unique( ":memory:", error ) };
    if ( !database ) {

      GTEST_FAIL() << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    error = sqlite_utils::importDump( database.get(), "main", std::string( exportFilename ) );
    EXPECT_FALSE( error );

    /* Count entries */
    const std::string sql = "SELECT COUNT(city) FROM cities";
    std::int32_t resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    const auto statement = vx::sqlite_utils::sqlite3_stmt_make_unique( database.get(), sql, error );
    while ( ( resultCode = sqlite3_step( statement.get() ) ) == SQLITE_ROW ) {

      const std::int32_t count = sqlite3_column_int( statement.get(), 0 );
      EXPECT_EQ( count, 3 );
    }
    if ( resultCode != SQLITE_DONE ) {

      GTEST_FAIL() << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    EXPECT_TRUE( std::filesystem::remove( exportFilename ) );
    EXPECT_FALSE( std::filesystem::exists( exportFilename ) );
  }
}
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

std::int32_t main( std::int32_t argc,
                   char **argv ) {

  InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

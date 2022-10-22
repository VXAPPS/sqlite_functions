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

/* modern.cpp.core */
#include <StringUtils.h>

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
   * name
   * Игорь Фёдорович Стравинский # Igor' Fëdorovič Stravinskij
   * 宮崎 駿 # Hayao Miyazaki
   * 艾未未 # Ai Weiwei
   * 오미주 # Sandra Oh
   * パイナップル # Painappuro
   * Albert Einstein
   */

  TEST( Ascii, OrderBy ) {

    /* Open database */
    const auto database { sqlite_utils::sqlite3_make_unique( ":memory:" ) };
    if ( !database ) {

      GTEST_FAIL() << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    int resultCode = sqlite3_create_function_v2( database.get(), "ascii", 1, SQLITE_UTF8, nullptr, &sqlite_utils::ascii, nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    /* Create table */
    std::string sql = "CREATE TABLE mixed (name STRING, note STRING)";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    /* Insert data */
    sql = "INSERT INTO mixed VALUES('Игорь Фёдорович Стравинский','Igor'' Fëdorovič Stravinskij')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('宮崎 駿','Hayao Miyazaki')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('艾未未','Ai Weiwei')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('오미주','Sandra Oh')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('パイナップル','Ananas')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('Albert Einstein','Albert Einstein')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    /* SELECT */
    sql = "SELECT ASCII(name) AS ascii FROM mixed ORDER BY ascii";
    std::vector<std::string> asciiListOrdered {};
    const auto statement = sqlite_utils::sqlite3_stmt_make_unique( database.get(), sql );
    while ( ( resultCode = sqlite3_step( statement.get() ) ) == SQLITE_ROW ) {

      std::optional ascii = string_utils::fromUnsignedChar( sqlite3_column_text( statement.get(), 0 ) );
      asciiListOrdered.emplace_back( ascii.value_or( "" ) );
    }
    if ( resultCode != SQLITE_DONE ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    std::vector<std::string> expected = { "ai wei wei", "albert einstein", "gong qi jun", "igor' fedorovic stravinskij", "omiju", "painappuru" };
    EXPECT_EQ( asciiListOrdered, expected );
  }

  TEST( Ascii, Search ) {

    /* Open database */
    const auto database { sqlite_utils::sqlite3_make_unique( ":memory:" ) };
    if ( !database ) {

      GTEST_FAIL() << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    int resultCode = sqlite3_create_function_v2( database.get(), "ascii", 1, SQLITE_UTF8, nullptr, &sqlite_utils::ascii, nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "'";
    }

    /* Create table */
    std::string sql = "CREATE TABLE mixed (name STRING, note STRING)";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    /* Insert data */
    sql = "INSERT INTO mixed VALUES('Игорь Фёдорович Стравинский','Igor'' Fëdorovič Stravinskij')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('宮崎 駿','Hayao Miyazaki')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('艾未未','Ai Weiwei')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('오미주','Sandra Oh')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('パイナップル','Ananas')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    sql = "INSERT INTO mixed VALUES('Albert Einstein','Albert Einstein')";
    resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
    if ( resultCode != SQLITE_OK ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    /* SELECT */
    sql = "SELECT ASCII(name) AS ascii FROM mixed WHERE ascii LIKE '%ei%' ORDER BY ascii";
    std::vector<std::string> asciiListOrdered {};
    const auto statement = sqlite_utils::sqlite3_stmt_make_unique( database.get(), sql );
    while ( ( resultCode = sqlite3_step( statement.get() ) ) == SQLITE_ROW ) {

      std::optional ascii = string_utils::fromUnsignedChar( sqlite3_column_text( statement.get(), 0 ) );
      asciiListOrdered.emplace_back( ascii.value_or( "" ) );
    }
    if ( resultCode != SQLITE_DONE ) {

      GTEST_FAIL() << "RESULT CODE: (" << resultCode << ") ERROR: '" << sqlite3_errmsg( database.get() ) << "' SQL: '" << sql << "'";
    }

    std::vector<std::string> expected = { "ai wei wei", "albert einstein" };
    EXPECT_EQ( asciiListOrdered, expected );
  }
}
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

int main( int argc, char **argv ) {

  InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

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
#include <fstream>
#include <iostream>
#include <locale>
#include <memory>
#include <vector>

/* sqlite header */
#include <sqlite3.h>

/* sqlite_functions */
#include <SqliteUtils.h>

/*
 * SQL DATA
 *
 * First DB
 *      city | latitude | longitude
 * Munich    |  48.1375 |    11.575
 * New York  |  40.6943 |  -73.9249
 * Tokyo     |  35.6839 |  139.7744
 *
 * Second DB
 *      city | latitude | longitude
 * Berlin    |  52.5167 |   13.3833
 * Paris     |  48.8566 |    2.3522
 * Hong Kong |  22.3069 |  114.1831
 *
 * Third DB
 *      city | latitude | longitude
 * Alexandria|     31.2 |   29.9167
 * Boston    |  42.3188 |  -71.0846
 * Melbourne | -37.8136 |  144.9631
 */

static std::wstring s2ws( const std::string &str ) {

  using convert_typeX = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_typeX, wchar_t> converterX;

  return converterX.from_bytes( str );
}

static std::string ws2s( const std::wstring &wstr ) {

  using convert_typeX = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_typeX, wchar_t> converterX;

  return converterX.to_bytes( wstr );
}

int main() {

  /* Open database */
  const auto database { vx::sqlite_utils::sqlite3_make_unique( ":memory:" ) };
  if ( !database ) {

    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Create table */
  std::string sql = "CREATE TABLE cities (city STRING, latitude REAL, longitude REAL)";
  int resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Insert data */
  sql = "INSERT INTO cities VALUES('New York', 40.6943, -73.9249)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO cities VALUES('Tokyo', 35.6839, 139.7744)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO cities VALUES('Munich', 48.1375, 11.575)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* SECOND DB */
  resultCode = sqlite3_exec( database.get(), "ATTACH DATABASE ':memory:' AS second", nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Create table */
  sql = "CREATE TABLE IF NOT EXISTS second.cities (city STRING, latitude REAL, longitude REAL)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Insert data */
  sql = "INSERT INTO second.cities VALUES('Berlin', 52.5167, 13.3833)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO second.cities VALUES('Paris', 48.8566, 2.3522)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO second.cities VALUES('Hong Kong', 22.3069, 114.1831)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* THIRD DB */
  resultCode = sqlite3_exec( database.get(), "ATTACH DATABASE ':memory:' AS third", nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Create table */
  sql = "CREATE TABLE third.cities (city STRING, latitude REAL, longitude REAL)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Insert data */
  sql = "INSERT INTO third.cities VALUES('Alexandria', 31.2, 29.9167)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO third.cities VALUES('Boston', 42.3188, -71.0846)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO third.cities VALUES('Melbourne', -37.8136, 144.9631)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO third.cities VALUES('イチゴ', -37.8136, 144.9631)"; // I chi go
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO third.cities VALUES('チイゴ', -37.8136, 144.9631)"; // chi i go
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  //  _setmode(_fileno(stdout), _O_U8TEXT);

  const std::string str = "ABCチイゴ";
  std::cout << str << " " << str.length() << std::endl;
  const std::wstring wstr = L"チイゴ";
  std::wcout << wstr << " " << wstr.length() << std::endl;
  const std::string str2 = ws2s( wstr );
  std::cout << str2 << " " << str2.length() << std::endl;

  const std::wstring wstr2 = s2ws( str );
  std::wcout << wstr2 << " " << wstr2.length() << std::endl;

  sql = "INSERT INTO third.cities VALUES('ゴチイ', -37.8136, 144.9631)"; // go chi i
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO third.cities VALUES('说/説🧡', -37.8136, 144.9631)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Overall SELECT */
  sql = "SELECT * FROM cities UNION ALL SELECT * FROM second.cities UNION ALL SELECT * FROM third.cities ORDER BY city";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::outputCallback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "PRAGMA database_list";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::outputCallback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  resultCode = sqlite3_exec( database.get(), "ATTACH DATABASE ':memory:' AS forth", nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

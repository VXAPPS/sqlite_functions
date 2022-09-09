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
#include <iostream>
#include <memory>

/* sqlite_functions */
#include <SqliteUtils.h>

int main() {

  /* Open database */
  std::unique_ptr<sqlite3, vx::sqlite_utils::sqlite3_deleter> database { vx::sqlite_utils::sqlite3_make_unique( "/bin/memory" ) };
  if ( !database ) {

    std::cout << __LINE__ << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

//  bool databaseOpen = true;

//    sqlite3_exec( m_database, "PRAGMA synchronous = 0", nullptr, nullptr, nullptr );
//    sqlite3_exec( m_database, "PRAGMA journal_mode = MEMORY", nullptr, nullptr, nullptr );
//    sqlite3_exec( m_database, "PRAGMA temp_store = 2", nullptr, nullptr, nullptr );
  int resultCode = sqlite3_create_function( database.get(), "distance", 4, SQLITE_UTF8, nullptr, &vx::sqlite_utils::distance, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __LINE__ << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }
  resultCode = sqlite3_create_function( database.get(), "ascii", 1, SQLITE_UTF8, nullptr, &vx::sqlite_utils::ascii, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __LINE__ << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Create table */
//  char* error_msg{};
//  scope_exit cleaner_upper{[&] { sqlite3_free(error_msg); }};
  std::string sql = "CREATE TABLE cities (city STRING, latitude REAL, longitude REAL)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Insert data */
  sql = "INSERT INTO cities VALUES ('New York', 40.6943, -73.9249)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO cities VALUES ('Tokyo', 35.6839, 139.7744)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO cities VALUES ('Munich', 48.1375, 11.575)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Calculate a distance from select */
  /* std::string city = "Berlin"; */
  /* double latitude = 52.5167; */
  /* double longitude = 13.3833; */
  sql = "SELECT DISTANCE( latitude, longitude, '52.5167', '13.3833' ) AS distance FROM cities WHERE city IS 'Munich'";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::output_callback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "SELECT DISTANCE( latitude, longitude, '52.5167', '13.3833' ) AS distance FROM cities WHERE city IS 'New York'";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::output_callback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "SELECT DISTANCE( latitude, longitude, '52.5167', '13.3833' ) AS distance FROM cities WHERE city IS 'Tokyo'";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::output_callback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

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
#include <fstream>
#include <iostream>
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

int main() {

  std::cout << SQLITE_VERSION << std::endl;

  /* Open database */
  const std::unique_ptr<sqlite3, vx::sqlite_utils::sqlite3_deleter> database { vx::sqlite_utils::sqlite3_make_unique( ":memory:" ) };
  if ( !database ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Create table */
  std::string sql = "CREATE TABLE cities (city STRING, latitude REAL, longitude REAL)";
  int resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
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

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO cities VALUES('Tokyo', 35.6839, 139.7744)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO cities VALUES('Munich', 48.1375, 11.575)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

//  vx::sqlite_utils::exportDump( database.get(), "main", "main_test.sqlite3" );

  /* SECOND DB */
  resultCode = sqlite3_exec( database.get(), "ATTACH DATABASE ':memory:' AS second", nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  if ( true ) {

    auto [ resultCodeImport, resultMessageImport ] = vx::sqlite_utils::importDump( database.get(), "second", "second_test.sqlite3" );
    std::cout << resultCodeImport << " " << resultMessageImport << std::endl;
  }
  else {

    const std::string _schema = "second";
    const std::string _filename = "second_test.sqlite3";

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

    resultCode = sqlite3_deserialize( database.get(), _schema.c_str(), converted.data(), static_cast<sqlite3_int64>( converted.size() ), static_cast<sqlite3_int64>( converted.size() ), SQLITE_DESERIALIZE_RESIZEABLE | SQLITE_DESERIALIZE_FREEONCLOSE );
    if ( resultCode != SQLITE_OK ) {

      std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
      std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
      std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
      std::cout << "SQL: '" << sql << "'" << std::endl;
      std::cout << std::endl;
      return EXIT_FAILURE;
    }
  }

  /* Create table */
  sql = "CREATE TABLE IF NOT EXISTS second.cities (city STRING, latitude REAL, longitude REAL)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
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

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO second.cities VALUES('Paris', 48.8566, 2.3522)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO second.cities VALUES('Hong Kong', 22.3069, 114.1831)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* THIRD DB */
  resultCode = sqlite3_exec( database.get(), "ATTACH DATABASE ':memory:' AS third", nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
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

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
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

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO third.cities VALUES('Boston', 42.3188, -71.0846)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "INSERT INTO third.cities VALUES('Melbourne', -37.8136, 144.9631)";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* Overall SELECT */
  sql = "SELECT * FROM cities UNION ALL SELECT * FROM second.cities UNION ALL SELECT * FROM third.cities ORDER BY city"; // LIMIT 3, 3";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::outputCallback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  sql = "PRAGMA database_list";
  resultCode = sqlite3_exec( database.get(), sql.c_str(), vx::sqlite_utils::outputCallback, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  vx::sqlite_utils::exportDump( database.get(), "main", "main_test.sqlite3" );
  vx::sqlite_utils::exportDump( database.get(), "second", "second_test.sqlite3" );
  vx::sqlite_utils::exportDump( database.get(), "third", "third_test.sqlite3" );

  resultCode = sqlite3_exec( database.get(), "ATTACH DATABASE ':memory:' AS forth", nullptr, nullptr, nullptr );
  if ( resultCode != SQLITE_OK ) {

    std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
    std::cout << "RESULT CODE: (" << resultCode << ")" << std::endl;
    std::cout << "ERROR: '" << sqlite3_errmsg( database.get() ) << "'" << std::endl;
    std::cout << "SQL: '" << sql << "'" << std::endl;
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

//  std::cout << vx::sqlite_utils::importDump( database.get(), "forth", "second_test.sqlite3" ) << std::endl;

  return EXIT_SUCCESS;
}

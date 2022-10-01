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

#pragma once

/* stl header */
#include <iostream>
#include <memory>
#include <string>

/* forward declation of sqlite3 */
struct sqlite3;
struct sqlite3_context;
struct sqlite3_stmt;
struct sqlite3_value;

/**
 * @brief vx (VX APPS) namespace.
 */
namespace vx::sqlite_utils {

  struct sqlite3_deleter {

    void operator()( sqlite3 *_handle ) const;
  };

  struct sqlite3_stmt_deleter {

    void operator()( sqlite3_stmt *_statement ) const;
  };

  struct sqlite3_generic_deleter {

    void operator()( void *_what ) const;
  };

  struct sqlite3_str_deleter {

    void operator()( char *_what ) const;
  };

  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename );

  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_handle,
                                                                                const std::string &_sql );

  std::tuple<int, std::string> import_dump( sqlite3 *_handle,
                                            const std::string &_schema,
                                            const std::string &_filename );

  std::tuple<int, std::string> export_dump( sqlite3 *_handle,
                                            const std::string &_schema,
                                            const std::string &_filename );

  /**
   * @brief Calculate the distance inside sql command.
   * @param _context   SQLite3 context.
   * @param _argc   Args size.
   * @param _argv   Args array.
   * @todo Extend to 5. param km or mi
   */
  void distance( sqlite3_context *_context,
                 int _argc,
                 sqlite3_value **_argv );

  void ascii( sqlite3_context *_context,
              int _argc,
              sqlite3_value **_argv );

  int output_callback( void *_data,
                       int _argc,
                       char **_argv,
                       char **_columns );
}

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
#include <memory>
#include <string>
#include <system_error>

/* forward declation of sqlite3 */
struct sqlite3;
struct sqlite3_context;
struct sqlite3_stmt;
struct sqlite3_value;

/**
 * @brief vx (VX APPS) namespace.
 */
namespace vx::sqlite_utils {

  /**
   * @brief The Result enum.
   */
  enum class Result {

    Code,   /**< Result code. */
    Message /**< Result message. */
  };

  /**
   * @brief The sqlite3_deleter class.
   */
  struct sqlite3_deleter {

    /**
     * @brief Sqlite3 operator ().
     * @param _handle   To sqlite3_close.
     */
    void operator()( sqlite3 *_handle ) const noexcept;
  };

  /**
   * @brief The sqlite3_stmt_deleter class.
   */
  struct sqlite3_stmt_deleter {

    /**
     * @brief Sqlite3 statement operator ().
     * @param _statement   To sqlite3_finalize.
     */
    void operator()( sqlite3_stmt *_statement ) const noexcept;
  };

  /**
   * @brief The sqlite3_generic_deleter class.
   */
  struct sqlite3_generic_deleter {

    /**
     * @brief Sqlite3 generic operator ().
     * @param _what   To sqlite3_free.
     */
    void operator()( void *_what ) const noexcept;
  };

  /**
   * @brief The sqlite3_str_deleter class.
   */
  struct sqlite3_str_deleter {

    /**
     * @brief Sqlite3 string operator ().
     * @param _what   To sqlite3_free.
     */
    void operator()( char *_what ) const noexcept;
  };

  /**
   * @brief Create sqlite3_open unique pointer.
   * @param _filename   Database filename.
   * @return Unique pointer for sqlite3 handle or nullptr on error.
   */
  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename ) noexcept;

  /**
   * @brief Create sqlite3_open unique pointer.
   * @param _filename   Database filename.
   * @param _error   Error code.
   * @return Unique pointer for sqlite3 handle or nullptr on error.
   */
  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename,
                                                                 std::error_code &_error ) noexcept;

  /**
   * @brief Create sqlite3_stmt unique pointer.
   * @param _handle   Database handle.
   * @param _sql   Sql command.
   * @return Unique pointer for sqlite3_stmt or nullptr on error.
   */
  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_handle,
                                                                                const std::string &_sql ) noexcept;

  /**
   * @brief Create sqlite3_stmt unique pointer.
   * @param _handle   Database handle.
   * @param _sql   Sql command.
   * @param _error   Error code.
   * @return Unique pointer for sqlite3_stmt or nullptr on error.
   */
  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_handle,
                                                                                const std::string &_sql,
                                                                                std::error_code &_error ) noexcept;

  /**
   * @brief Import sql dump.
   * @param _handle   Database handle.
   * @param _schema   Import shema - default is main.
   * @param _filename   Database filename.
   * @return Result code and message of operation.
   */
  std::error_code importDump( sqlite3 *_handle,
                              const std::string &_schema,
                              const std::string &_filename );

  /**
   * @brief Export sql dump.
   * @param _handle   Database handle.
   * @param _schema   Export shema - default is main.
   * @param _filename   Database filename.
   * @return Result code and message of operation.
   */
  std::error_code exportDump( sqlite3 *_handle,
                              const std::string &_schema,
                              const std::string &_filename );

  /**
   * @brief Calculate the location distance as sql command.
   * @param _context   SQLite3 context.
   * @param _argc   Args size.
   * @param _argv   Args array.
   */
  void distance( sqlite3_context *_context,
                 std::int32_t _argc,
                 sqlite3_value **_argv ) noexcept;

  /**
   * @brief Transliteration as sql command.
   * @param _context   SQLite3 context.
   * @param _argc   Args size.
   * @param _argv   Args array.
   */
  void transliteration( sqlite3_context *_context,
                        std::int32_t _argc,
                        sqlite3_value **_argv );

  /**
   * @brief Output callback for sql command.
   * @param _data   Incoming data.
   * @param _argc   Args size.
   * @param _argv   Args array.
   * @param _columns   Column names.
   * @return Result code.
   */
  std::int32_t outputCallback( void *_data,
                               std::int32_t _argc,
                               char **_argv,
                               char **_columns ) noexcept;
}

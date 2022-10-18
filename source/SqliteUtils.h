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

  enum class Result {

    Code,
    Message
  };

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

  //  enum class SqliteError {

  //    SQLITE_OK = 0,          /* Successful result */
  //                            /* beginning-of-error-codes */
  //    SQLITE_ERROR = 1,       /* Generic error */
  //    SQLITE_INTERNAL = 2,    /* Internal logic error in SQLite */
  //    SQLITE_PERM = 3,        /* Access permission denied */
  //    SQLITE_ABORT = 4,       /* Callback routine requested an abort */
  //    SQLITE_BUSY = 5,        /* The database file is locked */
  //    SQLITE_LOCKED = 6,      /* A table in the database is locked */
  //    SQLITE_NOMEM = 7,       /* A malloc() failed */
  //    SQLITE_READONLY = 8,    /* Attempt to write a readonly database */
  //    SQLITE_INTERRUPT = 9,   /* Operation terminated by sqlite3_interrupt()*/
  //    SQLITE_IOERR = 10,      /* Some kind of disk I/O error occurred */
  //    SQLITE_CORRUPT = 11,    /* The database disk image is malformed */
  //    SQLITE_NOTFOUND = 12,   /* Unknown opcode in sqlite3_file_control() */
  //    SQLITE_FULL = 13,       /* Insertion failed because database is full */
  //    SQLITE_CANTOPEN = 14,   /* Unable to open the database file */
  //    SQLITE_PROTOCOL = 15,   /* Database lock protocol error */
  //    SQLITE_EMPTY = 16,      /* Internal use only */
  //    SQLITE_SCHEMA = 17,     /* The database schema changed */
  //    SQLITE_TOOBIG = 18,     /* String or BLOB exceeds size limit */
  //    SQLITE_CONSTRAINT = 19, /* Abort due to constraint violation */
  //    SQLITE_MISMATCH = 20,   /* Data type mismatch */
  //    SQLITE_MISUSE = 21,     /* Library used incorrectly */
  //    SQLITE_NOLFS = 22,      /* Uses OS features not supported on host */
  //    SQLITE_AUTH = 23,       /* Authorization denied */
  //    SQLITE_FORMAT = 24,     /* Not used */
  //    SQLITE_RANGE = 25,      /* 2nd parameter to sqlite3_bind out of range */
  //    SQLITE_NOTADB = 26,     /* File opened that is not a database file */
  //    SQLITE_NOTICE = 27,     /* Notifications from sqlite3_log() */
  //    SQLITE_WARNING = 28,    /* Warnings from sqlite3_log() */
  //    SQLITE_ROW = 100,       /* sqlite3_step() has another row ready */
  //    SQLITE_DONE = 101       /* sqlite3_step() has finished executing */
  //  };

  namespace { // anonymous namespace

    struct SqliteErrorCategory : std::error_category {

      [[nodiscard]] const char *name() const noexcept override {

        return "sqlite";
      }

      [[nodiscard]] std::string message( int condition ) const override {

        (void)condition;
        return "SQLite Error Message";
      }
    };
  }

  std::unique_ptr<sqlite3, sqlite3_deleter> sqlite3_make_unique( const std::string &_filename );

  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> sqlite3_stmt_make_unique( sqlite3 *_handle,
                                                                                const std::string &_sql );

  std::tuple<int, std::string> importDump( sqlite3 *_handle,
                                           const std::string &_schema,
                                           const std::string &_filename );

  std::tuple<int, std::string> exportDump( sqlite3 *_handle,
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

  int outputCallback( void *_data,
                      int _argc,
                      char **_argv,
                      char **_columns );
}

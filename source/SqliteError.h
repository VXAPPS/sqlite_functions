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
#include <string>
#include <string_view>
#include <system_error>

/* modern.cpp.core */
#include <Singleton.h>

/**
 * @brief vx (VX APPS) sqlite namespace.
 */
namespace vx::sqlite {

  /**
   * @brief The Error enum.
   */
  enum class Error {

    Ok = 0,             /**< Successful result */
    Error = 1,          /**< Generic error */
    Internal = 2,       /**< Internal logic error in SQLite */
    Permission = 3,     /**< Access permission denied */
    Abort = 4,          /**< Callback routine requested an abort */
    Busy = 5,           /**< The database file is locked */
    Locked = 6,         /**< A table in the database is locked */
    NoMem = 7,          /**< A malloc() failed */
    Readonly = 8,       /**< Attempt to write a readonly database */
    Interrupt = 9,      /**< Operation terminated by sqlite3_interrupt()*/
    IOErr = 10,         /**< Some kind of disk I/O error occurred */
    Corrupt = 11,       /**< The database disk image is malformed */
    NotFound = 12,      /**< Unknown opcode in sqlite3_file_control() */
    Full = 13,          /**< Insertion failed because database is full */
    CantOpen = 14,      /**< Unable to open the database file */
    Protocol = 15,      /**< Database lock protocol error */
    Empty = 16,         /**< Internal use only */
    Schema = 17,        /**< The database schema changed */
    TooBig = 18,        /**< String or BLOB exceeds size limit */
    Consraint = 19,     /**< Abort due to constraint violation */
    Mismatch = 20,      /**< Data type mismatch */
    Misuse = 21,        /**< Library used incorrectly */
    NoLFS = 22,         /**< Uses OS features not supported on host */
    Authorization = 23, /**< Authorization denied */
    Format = 24,        /**< Not used */
    Range = 25,         /**< 2nd parameter to sqlite3_bind out of range */
    NotADatabase = 26,  /**< File opened that is not a database file */
    Notice = 27,        /**< Notifications from sqlite3_log() */
    Warning = 28,       /**< Warnings from sqlite3_log() */
    Row = 100,          /**< sqlite3_step() has another row ready */
    Done = 101          /**< sqlite3_step() has finished executing */
  };
}

namespace std {

  template <>
  struct is_error_code_enum<vx::sqlite::Error> : true_type {};
}

namespace { // NOSONAR

  class SqliteErrorCategory final : public std::error_category,
                                    public vx::Singleton<SqliteErrorCategory> {

  public:
    /**
     * @brief Reimplementation of std::error_category::name.
     * @return Name of this error category.
     */
    [[nodiscard]] const char *name() const noexcept override { return "sqlite"; }

    /**
     * @brief Reimplementation of std::error_category::message.
     * @param _condition   Condition value.
     * @return Error message for condition.
     */
    [[nodiscard]] std::string message( [[maybe_unused]] std::int32_t _condition ) const noexcept override { return m_message; }

    /**
     * @brief Set a textual message.
     * @param _message   Message to set.
     */
    [[maybe_unused]] void setMessage( std::string_view _message ) noexcept { m_message = _message; }

  private:
    /**
     * @brief Member for message.
     */
    std::string m_message {};
  };
}

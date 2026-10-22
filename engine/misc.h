/*
  ShashChess, a UCI chess playing engine derived from Stockfish
  Copyright (C) 2004-2026 ShashChess developers (see AUTHORS file)

  ShashChess is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ShashChess is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <exception>  // IWYU pragma: keep
// IWYU pragma: no_include <__exception/terminate.h>
#include <functional>
#include <iosfwd>
#include <optional>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
//ShashChess specific begin
#include <rukh/support/hash.h>
#include "types.h"
#include <iostream>
#ifndef _WIN32
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
#else
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX  // Disable macros min() and max()
    #endif
    #include <windows.h>
#endif
//ShashChess specific end

#define stringify2(x) #x
#define stringify(x) stringify2(x)

namespace ShashChess {

std::string engine_version_info();
std::string engine_info(bool to_uci = false);
std::string compiler_info();

// Preloads the given address in L1/L2 cache. This is a non-blocking
// function that doesn't stall the CPU waiting for data to be loaded from memory,
// which can be quite slow.
void prefetch(const void* addr);

void start_logger(const std::string& fname);

#if defined(__linux__)

struct PipeDeleter {
    void operator()(FILE* file) const {
        if (file != nullptr)
        {
            pclose(file);
        }
    }
};

#endif

// Reads the file as bytes.
// Returns std::nullopt if the file does not exist.
std::optional<std::string> read_file_to_string(const std::string& path);

void dbg_hit_on(bool cond, int slot = 0);
void dbg_mean_of(int64_t value, int slot = 0);
void dbg_stdev_of(int64_t value, int slot = 0);
void dbg_extremes_of(int64_t value, int slot = 0);
void dbg_correl_of(int64_t value1, int64_t value2, int slot = 0);
void dbg_print();
void dbg_clear();

using TimePoint = std::chrono::milliseconds::rep;  // A value in milliseconds
static_assert(sizeof(TimePoint) == sizeof(int64_t), "TimePoint should be 64 bits");
inline TimePoint now() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

enum SyncCout {
    IO_LOCK,
    IO_UNLOCK
};
std::ostream& operator<<(std::ostream&, SyncCout);

#define sync_cout std::cout << IO_LOCK
#define sync_endl std::endl << IO_UNLOCK

void sync_cout_start();
void sync_cout_end();

// True if and only if the binary is compiled on a little-endian machine
static inline const std::uint16_t Le             = 1;
static inline const bool          IsLittleEndian = *reinterpret_cast<const char*>(&Le) == 1;


//omissis ShashChess: in types.h


// xorshift64star Pseudo-Random Number Generator
// This class is based on original code written and dedicated
// to the public domain by Sebastiano Vigna (2014).
// It has the following characteristics:
//
//  -  Outputs 64-bit numbers
//  -  Passes Dieharder and SmallCrush test batteries
//  -  Does not require warm-up, no zeroland to escape
//  -  Internal state is a single 64-bit integer
//  -  Period is 2^64 - 1
//  -  Speed: 1.60 ns/call (Core i7 @3.40GHz)
//
// For further analysis see
//   <http://vigna.di.unimi.it/ftp/papers/xorshift.pdf>

class PRNG {

    uint64_t s;

    uint64_t rand64() {

        s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
        return s * 2685821657736338717LL;
    }

   public:
    PRNG(uint64_t seed) :
        s(seed) {
        assert(seed);
    }

    template<typename T>
    T rand() {
        return T(rand64());
    }

    // Special generator used to fast init magic numbers.
    // Output values only have 1/8th of their bits set on average.
    template<typename T>
    T sparse_rand() {
        return T(rand64() & rand64() & rand64());
    }
};

struct CommandLine {
   public:
    CommandLine(int, char**);  //from learning

    int    argc;
    char** argv;
    //from learning begin
    std::string        binaryDirectory;   // path of the executable directory
    std::string        workingDirectory;  // path of the working directory
    static std::string get_working_directory();
    static std::string get_binary_directory(std::string argv0, std::string workingDirectory);
    //from learning end
};

namespace Utility {

template<typename T, typename Predicate>
void move_to_front(std::vector<T>& vec, Predicate pred) {
    auto it = std::find_if(vec.begin(), vec.end(), pred);

    if (it != vec.end())
    {
        std::rotate(vec.begin(), it, it + 1);
    }
}
}


#if defined(__GNUC__)
    #define sf_always_inline __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define sf_always_inline __forceinline
#else
    // do nothing for other compilers
    #define sf_always_inline
#endif

//ShashChess omissis: in types.h

//begin from learning
#define EMPTY "<empty>"

class Util {
   private:
#if defined(_WIN32) || defined(_WIN64)
    static constexpr char DirectorySeparator        = '\\';
    static constexpr char ReverseDirectorySeparator = '/';
#else
    static constexpr char DirectorySeparator        = '/';
    static constexpr char ReverseDirectorySeparator = '\\';
#endif
    static CommandLine* cli;

   public:
    static void        init(CommandLine* _cli);
    static std::string unquote(const std::string& s);
    static bool        is_empty_filename(const std::string& f);
    static std::string fix_path(const std::string& p);
    static std::string combine_path(const std::string& p1, const std::string& p2);
    static std::string map_path(const std::string& p);

    static size_t get_file_size(const std::string& f);
    static bool   is_same_file(const std::string& f1, const std::string& f2);

    static std::string format_bytes(uint64_t bytes, int decimals);

    static std::string format_string(const char* const fmt, ...);
};
//end from learning

}  // namespace ShashChess
#endif  // #ifndef MISC_H_INCLUDED

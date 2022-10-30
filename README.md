# sqlite_functions
Sqlite3 function extension in most native and modern C++17 or C++20.

## Build
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE:STRING=Debug|Release ../sqlite_functions
make -j`nproc`
```

## Helper
- **sqlite3_make_unique** - Create unique pointer from sqlite3_open.
- **sqlite3_stmt_make_unique** - Create unique pointer from sqlite3_stmt.

## Functions
- **importDump** - Import sql dump.
- **exportDump** - Export sql dump.

## Function Extensions
- **DISTANCE** - DISTANCE( latitude1, longitude1, latitude2, longitude2 ).
- **TRANSLITERATION** - TRANSLITERATION( any_literation ).

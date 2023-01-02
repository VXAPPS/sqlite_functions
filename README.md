# sqlite_functions
Sqlite3 function extension in most native and modern C++17 or C++20.

## Examples
### Distance
```sql
# First = Munich, Second = Berlin
SELECT DISTANCE(48.1375, 11.575, 52.5167, 13.3833) AS distance # distance = 504.100899610028
```

### Transliteration
```sql
# Katakana
SELECT TRANSLITERATION('パイナップル') AS transliterated # transliterated = painappuru
```

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

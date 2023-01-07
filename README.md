# sqlite_functions
Sqlite3 function extension in most native and modern C++23 or C++20 or at least C+17.

## Build
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE:STRING=Debug|Release ../sqlite_functions
make -j`nproc`
```

## Examples
### Distance
```sql
# First = Munich, Second = Berlin
SELECT DISTANCE(48.1375, 11.575, 52.5167, 13.3833) AS distance;
# distance = 504.100899610028
```

### Transliteration
```sql
# Katakana
SELECT TRANSLITERATION('パイナップル') AS transliterated;
# transliterated = painappuru
````

### Phone Book
```sql
# SQL DATA
# name
# Игорь Фёдорович Стравинский # Igor' Fëdorovič Stravinskij
# 宮崎 駿                      # Hayao Miyazaki
# 艾未未                       # Ai Weiwei
# 오미주                        # Sandra Oh
# パイナップル                  # Painappuro
# Albert Einstein             # Albert Einstein
# Zebra                       # Zebra

# Without TRANSLITERATION
SELECT name FROM phone_book ORDER BY LOWER(name);
# Albert Einstein             # Albert Einstein
# Zebra                       # Zebra
# Игорь Фёдорович Стравинский # Igor' Fëdorovič Stravinskij
# パイナップル                  # Painappuro
# 宮崎 駿                      # Hayao Miyazaki
# 艾未未                       # Ai Weiwei
# 오미주                        # Sandra Oh

# With TRANSLITERATION
SELECT name FROM phone_book ORDER BY LOWER(TRANSLITERATION(name));
# 艾未未                       # Ai Weiwei
# Albert Einstein             # Albert Einstein
# 宮崎 駿                      # Hayao Miyazaki
# Игорь Фёдорович Стравинский # Igor' Fëdorovič Stravinskij
# 오미주                        # Sandra Oh
# パイナップル                  # Painappuro
# Zebra                       # Zebra
```

## Helper
- **sqlite3_make_unique** - Create unique pointer from sqlite3_open.
- **sqlite3_stmt_make_unique** - Create unique pointer from sqlite3_stmt.

## Functions
- **importDump** - Import sql dump.
- **exportDump** - Export sql dump.

## Function Extensions
- **DISTANCE** - DISTANCE(latitude1, longitude1, latitude2, longitude2).
- **TRANSLITERATION** - TRANSLITERATION(any_literation).

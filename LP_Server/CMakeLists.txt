cmake_minimum_required(VERSION 3.10)
project(LP_Server)

set(CMAKE_CXX_STANDARD 17)

# Поиск PostgreSQL (libpq)
find_package(PostgreSQL REQUIRED)

# Если PostgreSQL не нашёлся автоматически, можно указать пути вручную:
# set(PostgreSQL_INCLUDE_DIRS "/usr/local/include")
# set(PostgreSQL_LIBRARIES "/usr/local/lib/libpq.so")

include_directories(
    src
    external
    ${PostgreSQL_INCLUDE_DIRS}   # добавляем пути к заголовкам PostgreSQL
)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(server ${SOURCES})

# Линкуем библиотеку PostgreSQL
target_link_libraries(server ${PostgreSQL_LIBRARIES})
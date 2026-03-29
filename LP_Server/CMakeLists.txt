cmake_minimum_required(VERSION 3.10)
project(LP_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Поиск PostgreSQL (libpq)
find_package(PostgreSQL REQUIRED)

# Если PostgreSQL не нашёлся автоматически, можно указать пути вручную:
# set(PostgreSQL_INCLUDE_DIRS "/usr/local/include")
# set(PostgreSQL_LIBRARIES "/usr/local/lib/libpq.so")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(server ${SOURCES})

target_include_directories(server PRIVATE
    src
    external
    ${PostgreSQL_INCLUDE_DIRS}
)

# Линкуем библиотеку PostgreSQL
target_link_libraries(server PRIVATE ${PostgreSQL_LIBRARIES})

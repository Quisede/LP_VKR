#pragma once

#include <libpq-fe.h>
#include <string>

class PostgresConnection {
public:
    PostgresConnection(const std::string& conninfo);
    ~PostgresConnection();

    // возвращает указатель на структуру PGconn, которая представляет собой соединение с базой данных PostgreSQL
    PGconn* get();

private:
    PGconn* connection = nullptr;
};

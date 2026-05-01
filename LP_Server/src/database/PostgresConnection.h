#pragma once

#include <libpq-fe.h>
#include <mutex>
#include <string>

class PostgresConnection {
public:
    PostgresConnection(const std::string& conninfo);
    ~PostgresConnection();

    // возвращает указатель на структуру PGconn, которая представляет собой соединение с базой данных PostgreSQL
    PGconn* get();
    std::mutex& mutex();

private:
    PGconn* connection = nullptr;
    std::mutex connectionMutex;
};

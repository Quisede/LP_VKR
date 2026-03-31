#pragma once

#include <libpq-fe.h>
#include <string>

class PostgresConnection {
public:
    PostgresConnection(const std::string& conninfo);
    ~PostgresConnection();

    PGconn* get();

private:
    PGconn* connection = nullptr;
};

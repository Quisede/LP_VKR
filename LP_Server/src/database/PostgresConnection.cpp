#include "PostgresConnection.h"
#include <stdexcept>

PostgresConnection::PostgresConnection(const std::string& conninfo) {
    connection = PQconnectdb(conninfo.c_str());
    if(connection == nullptr) {
        throw std::runtime_error("Connection to database failed: PQconnectdb returned null");
    }

    if(PQstatus(connection) != CONNECTION_OK) {
        std::string error = PQerrorMessage(connection);
        PQfinish(connection);
        connection = nullptr;
        throw std::runtime_error("Connection to database failed: " + error);
    }
}

PostgresConnection::~PostgresConnection() {
    if(connection != nullptr) {
        PQfinish(connection);
    }
}

PGconn* PostgresConnection::get() {
    return connection;
}

#include "PostgresConnection.h"
#include <stdexcept>

PostgresConnection::PostgresConnection(const std::string& conninfo) {
    // создание соединения на основе строки подключения
    connection = PQconnectdb(conninfo.c_str());
    if(connection == nullptr) {
        throw std::runtime_error("Connection to database failed: PQconnectdb returned null");
    }

    // проверка статуса соединения
    if(PQstatus(connection) != CONNECTION_OK) {
        // если статус НЕ ОК, то получаем сообщение об ошибке, закрываем соединение и выбрасываем исключение
        std::string error = PQerrorMessage(connection);
        PQfinish(connection);
        connection = nullptr;
        throw std::runtime_error("Connection to database failed: " + error);
    }
}

PostgresConnection::~PostgresConnection() {
    // закрываем соединение, если оно было успешно установлено
    if(connection != nullptr) {
        PQfinish(connection);
    }
}

PGconn* PostgresConnection::get() {
    // возвращаем указатель на структуру PGconn, которая представляет собой соединение с базой данных PostgreSQL
    return connection;
}

std::mutex& PostgresConnection::mutex() {
    return connectionMutex;
}

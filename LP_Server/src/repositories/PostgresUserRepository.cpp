#include "PostgresUserRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

PostgresUserRepository::PostgresUserRepository(PostgresConnection& conn) : connection(conn) {}

std::optional<User> PostgresUserRepository::findByLogin(
    const std::string& login) {

    const char* paramValues[1];
    paramValues[0] = login.c_str();

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT id, login, password_hash, role FROM users WHERE login=$1",
        1,
        NULL,
        paramValues,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("User select failed: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    User user;

    user.id = std::stoi(PQgetvalue(res,0,0));
    user.login = PQgetvalue(res,0,1);
    user.passwordHash = PQgetvalue(res,0,2);

    std::string role = PQgetvalue(res,0,3);

    if(role == "Student") user.role = UserRole::Student;
    if(role == "Teacher") user.role = UserRole::Teacher;
    if(role == "Admin") user.role = UserRole::Admin;

    PQclear(res);

    return user;
}

bool PostgresUserRepository::exists(const std::string& login) {
    std::string query =
        "SELECT 1 FROM users WHERE login='" + login + "' LIMIT 1";

    PGresult* res = PQexec(connection.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return false;
    }

    bool found = PQntuples(res) > 0;
    PQclear(res);
    return found;
}

User PostgresUserRepository::createUser(
    const std::string& login,
    const std::string& passwordHash,
    UserRole role) {

    std::string roleStr;

    if(role == UserRole::Student) roleStr = "Student";
    if(role == UserRole::Teacher) roleStr = "Teacher";
    if(role == UserRole::Admin) roleStr = "Admin";

    std::string query =
        "INSERT INTO users(login,password_hash,role) VALUES('" +
        login + "','" +
        passwordHash + "','" +
        roleStr + "') RETURNING id";

    PGresult* res = PQexec(connection.get(), query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        throw std::runtime_error("User insert failed");
    }

    User user;

    user.id = std::stoi(PQgetvalue(res,0,0));
    user.login = login;
    user.passwordHash = passwordHash;
    user.role = role;

    PQclear(res);

    return user;
}

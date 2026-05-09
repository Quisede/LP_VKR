#include "PostgresUserRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

PostgresUserRepository::PostgresUserRepository(PostgresConnection& conn) : connection(conn) {}

std::optional<User> PostgresUserRepository::findByLogin(
    const std::string& login) {
    std::lock_guard<std::mutex> lock(connection.mutex());

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
    std::lock_guard<std::mutex> lock(connection.mutex());
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
    std::lock_guard<std::mutex> lock(connection.mutex());

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

std::vector<User> PostgresUserRepository::getAllUsers()
{
    std::lock_guard<std::mutex> lock(connection.mutex());

    PGresult* res = PQexec(
        connection.get(),
        "SELECT id, login, password_hash, role FROM users ORDER BY id");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to get all users: " + error);
    }

    std::vector<User> users;
    const int rows = PQntuples(res);
    users.reserve(rows);

    for (int i = 0; i < rows; ++i) {
        User user;
        user.id = std::stoi(PQgetvalue(res, i, 0));
        user.login = PQgetvalue(res, i, 1);
        user.passwordHash = PQgetvalue(res, i, 2);

        const std::string role = PQgetvalue(res, i, 3);
        if (role == "Student") user.role = UserRole::Student;
        if (role == "Teacher") user.role = UserRole::Teacher;
        if (role == "Admin") user.role = UserRole::Admin;

        users.push_back(user);
    }

    PQclear(res);
    return users;
}

User PostgresUserRepository::updateUserRole(int userId, UserRole role)
{
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string roleStr;
    if (role == UserRole::Student) roleStr = "Student";
    if (role == UserRole::Teacher) roleStr = "Teacher";
    if (role == UserRole::Admin) roleStr = "Admin";

    std::string query =
        "UPDATE users SET role='" + roleStr + "' "
        "WHERE id=" + std::to_string(userId) +
        " RETURNING id, login, password_hash, role";

    PGresult* res = PQexec(connection.get(), query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to update user role: " + error);
    }

    User user;
    user.id = std::stoi(PQgetvalue(res, 0, 0));
    user.login = PQgetvalue(res, 0, 1);
    user.passwordHash = PQgetvalue(res, 0, 2);
    user.role = role;

    PQclear(res);
    return user;
}

void PostgresUserRepository::deleteUser(int userId)
{
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string query =
        "DELETE FROM users WHERE id=" + std::to_string(userId);

    PGresult* res = PQexec(connection.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to delete user: " + error);
    }

    PQclear(res);
}

std::vector<CourseStudent> PostgresUserRepository::getStudentsForCourse(int courseId) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string courseIdValue = std::to_string(courseId);
    const char* params[] = {courseIdValue.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT u.id, u.login, "
        "COALESCE(ROUND(AVG(a.percentage)), 0) AS progress "
        "FROM users u "
        "JOIN enrollments e ON e.student_id = u.id "
        "LEFT JOIN tests t ON t.course_id = e.course_id "
        "LEFT JOIN attempts a ON a.user_id = u.id AND a.test_id = t.id "
        "WHERE e.course_id = $1 AND u.role = 'Student' "
        "GROUP BY u.id, u.login "
        "ORDER BY u.id",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to get students for course: " + error);
    }

    std::vector<CourseStudent> students;
    int rows = PQntuples(res);

    for (int i = 0; i < rows; ++i) {
        CourseStudent student;
        student.id = std::stoi(PQgetvalue(res, i, 0));
        student.login = PQgetvalue(res, i, 1);
        student.progress = std::stoi(PQgetvalue(res, i, 2));
        students.push_back(student);
    }

    PQclear(res);
    return students;
}

#include "PostgresUserRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

namespace {

const char* kUserSelectColumns =
    "id, login, password_hash, role, first_name, last_name, group_name, email, phone";

std::string roleToString(UserRole role)
{
    if (role == UserRole::Student) return "Student";
    if (role == UserRole::Teacher) return "Teacher";
    if (role == UserRole::Admin) return "Admin";
    return "Student";
}

UserRole roleFromString(const std::string& role)
{
    if (role == "Teacher") return UserRole::Teacher;
    if (role == "Admin") return UserRole::Admin;
    return UserRole::Student;
}

void fillUserFromResult(User& user, PGresult* res, int row)
{
    user.id = std::stoi(PQgetvalue(res, row, 0));
    user.login = PQgetvalue(res, row, 1);
    user.passwordHash = PQgetvalue(res, row, 2);
    user.role = roleFromString(PQgetvalue(res, row, 3));
    user.firstName = PQgetvalue(res, row, 4);
    user.lastName = PQgetvalue(res, row, 5);
    user.groupName = PQgetvalue(res, row, 6);
    user.email = PQgetvalue(res, row, 7);
    user.phone = PQgetvalue(res, row, 8);
}

}

PostgresUserRepository::PostgresUserRepository(PostgresConnection& conn) : connection(conn) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    PGresult* res = PQexec(
        connection.get(),
        "ALTER TABLE users ADD COLUMN IF NOT EXISTS first_name TEXT NOT NULL DEFAULT '';"
        "ALTER TABLE users ADD COLUMN IF NOT EXISTS last_name TEXT NOT NULL DEFAULT '';"
        "ALTER TABLE users ADD COLUMN IF NOT EXISTS group_name TEXT NOT NULL DEFAULT '';"
        "ALTER TABLE users ADD COLUMN IF NOT EXISTS email TEXT NOT NULL DEFAULT '';"
        "ALTER TABLE users ADD COLUMN IF NOT EXISTS phone TEXT NOT NULL DEFAULT '';");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("User profile migration failed: " + error);
    }

    PQclear(res);
}

std::optional<User> PostgresUserRepository::findByLogin(
    const std::string& login) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    const char* paramValues[1];
    paramValues[0] = login.c_str();

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT id, login, password_hash, role, first_name, last_name, group_name, email, phone "
        "FROM users WHERE login=$1",
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
    fillUserFromResult(user, res, 0);

    PQclear(res);

    return user;
}

std::optional<User> PostgresUserRepository::findById(int userId) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string userIdValue = std::to_string(userId);
    const char* paramValues[1] = {userIdValue.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT id, login, password_hash, role, first_name, last_name, group_name, email, phone "
        "FROM users WHERE id=$1",
        1,
        nullptr,
        paramValues,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("User select by id failed: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    User user;
    fillUserFromResult(user, res, 0);
    PQclear(res);
    return user;
}

bool PostgresUserRepository::exists(const std::string& login) {
    std::lock_guard<std::mutex> lock(connection.mutex());
    const char* paramValues[1] = {login.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT 1 FROM users WHERE login=$1 LIMIT 1",
        1,
        nullptr,
        paramValues,
        nullptr,
        nullptr,
        0);
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

    const std::string roleStr = roleToString(role);
    const std::string firstName = login;
    const char* params[] = {
        login.c_str(),
        passwordHash.c_str(),
        roleStr.c_str(),
        firstName.c_str()
    };

    PGresult* res = PQexecParams(
        connection.get(),
        "INSERT INTO users(login, password_hash, role, first_name) "
        "VALUES($1, $2, $3, $4) "
        "RETURNING id, login, password_hash, role, first_name, last_name, group_name, email, phone",
        4,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        throw std::runtime_error("User insert failed");
    }

    User user;
    fillUserFromResult(user, res, 0);

    PQclear(res);

    return user;
}

std::vector<User> PostgresUserRepository::getAllUsers()
{
    std::lock_guard<std::mutex> lock(connection.mutex());

    PGresult* res = PQexec(
        connection.get(),
        "SELECT id, login, password_hash, role, first_name, last_name, group_name, email, phone "
        "FROM users ORDER BY id");

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
        fillUserFromResult(user, res, i);
        users.push_back(user);
    }

    PQclear(res);
    return users;
}

User PostgresUserRepository::updateUserRole(int userId, UserRole role)
{
    std::lock_guard<std::mutex> lock(connection.mutex());

    const std::string roleStr = roleToString(role);
    const std::string userIdValue = std::to_string(userId);
    const char* params[] = {roleStr.c_str(), userIdValue.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "UPDATE users SET role=$1 "
        "WHERE id=$2 "
        "RETURNING id, login, password_hash, role, first_name, last_name, group_name, email, phone",
        2,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to update user role: " + error);
    }

    User user;
    fillUserFromResult(user, res, 0);

    PQclear(res);
    return user;
}

void PostgresUserRepository::updatePasswordHash(int userId, const std::string& passwordHash)
{
    std::lock_guard<std::mutex> lock(connection.mutex());

    const std::string userIdValue = std::to_string(userId);
    const char* params[] = {passwordHash.c_str(), userIdValue.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "UPDATE users SET password_hash=$1 WHERE id=$2",
        2,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to update password: " + error);
    }

    PQclear(res);
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
        "COALESCE(ROUND(AVG(a.percentage)), 0) AS test_progress, "
        "CASE "
        "  WHEN COUNT(DISTINCT l.id) = 0 THEN 0 "
        "  ELSE ROUND(100.0 * COUNT(DISTINCT lp.lesson_id) / COUNT(DISTINCT l.id)) "
        "END AS lesson_progress "
        "FROM users u "
        "JOIN enrollments e ON e.student_id = u.id "
        "LEFT JOIN tests t ON t.course_id = e.course_id "
        "LEFT JOIN attempts a ON a.user_id = u.id AND a.test_id = t.id "
        "LEFT JOIN lessons l ON l.course_id = e.course_id "
        "LEFT JOIN lesson_progress lp ON lp.user_id = u.id AND lp.lesson_id = l.id "
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
        student.testProgress = std::stoi(PQgetvalue(res, i, 2));
        student.lessonProgress = std::stoi(PQgetvalue(res, i, 3));
        student.progress = (student.lessonProgress + student.testProgress) / 2;
        students.push_back(student);
    }

    PQclear(res);
    return students;
}

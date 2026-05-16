#include "PostgresAttemptRepository.h"
#include <stdexcept>

PostgresAttemptRepository::PostgresAttemptRepository(PostgresConnection& connection) 
: connection(connection) {}

void PostgresAttemptRepository::ensureSchema() {
    if (schemaChecked) {
        return;
    }

    PGresult* res = PQexec(
        connection.get(),
        "ALTER TABLE attempts "
        "ADD COLUMN IF NOT EXISTS created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to ensure attempts schema: " + error);
    }

    PQclear(res);
    schemaChecked = true;
}

void PostgresAttemptRepository::saveAttempt(const Attempt& attempt) {
    std::lock_guard<std::mutex> lock(connection.mutex());
    ensureSchema();
    std::string query =
        "INSERT INTO attempts(user_id,test_id,score,total,percentage,passed) "
        "VALUES(" +
        std::to_string(attempt.userId) + "," +
        std::to_string(attempt.testId) + "," +
        std::to_string(attempt.score) + "," +
        std::to_string(attempt.total) + "," +
        std::to_string(attempt.percentage) + "," +
        (attempt.passed ? "true" : "false") + ")";

    // выполняем SQL-запрос для сохранения попытки в базе данных
    PGresult* res = PQexec(connection.get(), query.c_str());
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        // если запрос не был выполнен успешно, то выбрасываем исключение с сообщением об ошибке из PostgreSQL
        throw std::runtime_error("Failed to save attempt: " + std::string(PQerrorMessage(connection.get())));
    }
    // очищаем результат запроса, так как он больше не нужен
    PQclear(res);
}

std::vector<Attempt> PostgresAttemptRepository::getAttemptsForUser(int userId) {
    std::lock_guard<std::mutex> lock(connection.mutex());
    ensureSchema();
    std::vector<Attempt> attempts;
    // формируем SQL-запрос для получения всех попыток, связанных с определенным пользователем, из базы данных
    std::string query = 
        "SELECT a.id, a.user_id, a.test_id, a.score, a.total, a.percentage, a.passed, "
        "COALESCE(t.title, ''), "
        "COALESCE(TO_CHAR(a.created_at, 'DD.MM.YYYY HH24:MI'), ''), "
        "COALESCE(t.course_id, -1) "
        "FROM attempts a "
        "LEFT JOIN tests t ON t.id = a.test_id "
        "WHERE a.user_id = " + std::to_string(userId) +
        " ORDER BY a.id DESC";
    
    PGresult* res = PQexec(connection.get(), query.c_str());
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        // если запрос не был выполнен успешно, то выбрасываем исключение с сообщением об ошибке из PostgreSQL
        throw std::runtime_error("Failed to get attempts: " + std::string(PQerrorMessage(connection.get())));
    }

    // получаем количество строк в результате запроса, чтобы знать, сколько попыток было найдено для данного пользователя
    // обрабатываем результат запроса, извлекая данные о попытках и добавляя их в вектор attempts
    int rows = PQntuples(res);
    for(int i = 0; i < rows; ++i) {
        Attempt attempt;
        // извлекаем данные из каждой строки результата запроса и заполняем структуру Attempt, которую затем добавляем в вектор attempts
        // используя PQgetvalue для извлечения значений по индексу столбца, с преобразованием типов: std::stoi для int, std::stod для double, и проверка "t" для boolean
        attempt.id = std::stoi(PQgetvalue(res, i, 0));
        attempt.userId = std::stoi(PQgetvalue(res, i, 1));
        attempt.testId = std::stoi(PQgetvalue(res, i, 2));
        attempt.score = std::stoi(PQgetvalue(res, i, 3));
        attempt.total = std::stoi(PQgetvalue(res, i, 4));
        attempt.percentage = std::stod(PQgetvalue(res, i, 5));
        attempt.passed = (std::string(PQgetvalue(res, i, 6)) == "t");
        attempt.testTitle = PQgetvalue(res, i, 7);
        attempt.submittedAt = PQgetvalue(res, i, 8);
        attempt.courseId = std::stoi(PQgetvalue(res, i, 9));
        attempts.push_back(attempt);
    }
    PQclear(res);
    return attempts;
}

CourseAnalytics PostgresAttemptRepository::getCourseAnalytics(int courseId) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    CourseAnalytics analytics;
    const std::string courseIdValue = std::to_string(courseId);
    const char* params[] = {courseIdValue.c_str()};

    PGresult* summaryRes = PQexecParams(
        connection.get(),
        "SELECT "
        "  (SELECT COUNT(DISTINCT e.student_id) FROM enrollments e WHERE e.course_id = $1) AS students_count, "
        "  COUNT(a.id) AS attempts_count, "
        "  COALESCE(AVG(a.percentage), 0) AS average_percentage, "
        "  COALESCE(( "
        "    SELECT AVG(student_lesson_progress.progress) "
        "    FROM ( "
        "      SELECT CASE "
        "        WHEN COUNT(DISTINCT l.id) = 0 THEN 0 "
        "        ELSE 100.0 * COUNT(DISTINCT lp.lesson_id) / COUNT(DISTINCT l.id) "
        "      END AS progress "
        "      FROM enrollments e "
        "      LEFT JOIN lessons l ON l.course_id = e.course_id "
        "      LEFT JOIN lesson_progress lp ON lp.user_id = e.student_id AND lp.lesson_id = l.id "
        "      WHERE e.course_id = $1 "
        "      GROUP BY e.student_id "
        "    ) student_lesson_progress "
        "  ), 0) AS average_lesson_progress "
        "FROM attempts a "
        "JOIN tests t ON t.id = a.test_id "
        "WHERE t.course_id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(summaryRes) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(summaryRes);
        throw std::runtime_error("Failed to get analytics summary: " + error);
    }

    if (PQntuples(summaryRes) > 0) {
        analytics.studentsCount = std::stoi(PQgetvalue(summaryRes, 0, 0));
        analytics.attemptsCount = std::stoi(PQgetvalue(summaryRes, 0, 1));
        analytics.averagePercentage = std::stod(PQgetvalue(summaryRes, 0, 2));
        analytics.averageLessonProgress = std::stod(PQgetvalue(summaryRes, 0, 3));
    }
    PQclear(summaryRes);

    PGresult* rowsRes = PQexecParams(
        connection.get(),
        "SELECT u.login, t.title, a.score, a.total, a.percentage, a.passed "
        "FROM attempts a "
        "JOIN tests t ON t.id = a.test_id "
        "JOIN users u ON u.id = a.user_id "
        "WHERE t.course_id = $1 "
        "ORDER BY a.id DESC",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(rowsRes) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(rowsRes);
        throw std::runtime_error("Failed to get analytics rows: " + error);
    }

    int rows = PQntuples(rowsRes);
    for (int i = 0; i < rows; ++i) {
        analytics.rows.push_back({
            PQgetvalue(rowsRes, i, 0),
            PQgetvalue(rowsRes, i, 1),
            std::stoi(PQgetvalue(rowsRes, i, 2)),
            std::stoi(PQgetvalue(rowsRes, i, 3)),
            std::stod(PQgetvalue(rowsRes, i, 4)),
            std::string(PQgetvalue(rowsRes, i, 5)) == "t"
        });
    }

    PQclear(rowsRes);
    return analytics;
}

std::vector<StudentCourseAttempt> PostgresAttemptRepository::getStudentCourseAttempts(int courseId, int studentId) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::vector<StudentCourseAttempt> attempts;
    const std::string courseIdValue = std::to_string(courseId);
    const std::string studentIdValue = std::to_string(studentId);
    const char* params[] = {courseIdValue.c_str(), studentIdValue.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT t.title, a.score, a.total, a.percentage, a.passed "
        "FROM attempts a "
        "JOIN tests t ON t.id = a.test_id "
        "WHERE t.course_id = $1 AND a.user_id = $2 "
        "ORDER BY a.id DESC",
        2,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to get student attempts: " + error);
    }

    const int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        attempts.push_back({
            PQgetvalue(res, i, 0),
            std::stoi(PQgetvalue(res, i, 1)),
            std::stoi(PQgetvalue(res, i, 2)),
            std::stod(PQgetvalue(res, i, 3)),
            std::string(PQgetvalue(res, i, 4)) == "t"
        });
    }

    PQclear(res);
    return attempts;
}

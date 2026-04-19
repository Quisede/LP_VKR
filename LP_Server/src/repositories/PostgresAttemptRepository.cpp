#include "PostgresAttemptRepository.h"

PostgresAttemptRepository::PostgresAttemptRepository(PostgresConnection& connection) 
: connection(connection) {}

void PostgresAttemptRepository::saveAttempt(const Attempt& attempt) {
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
    std::vector<Attempt> attempts;
    // формируем SQL-запрос для получения всех попыток, связанных с определенным пользователем, из базы данных
    std::string query = 
        "SELECT id, user_id, test_id, score, total, percentage, passed"
         " FROM attempts WHERE user_id = " + std::to_string(userId);
    
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
        attempts.push_back(attempt);
    }
    PQclear(res);
    return attempts;
}
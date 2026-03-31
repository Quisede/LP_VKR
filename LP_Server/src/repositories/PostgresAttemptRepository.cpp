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

    PGresult* res = PQexec(connection.get(), query.c_str());
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        throw std::runtime_error("Failed to save attempt: " + std::string(PQerrorMessage(connection.get())));
    }
    PQclear(res);
}

std::vector<Attempt> PostgresAttemptRepository::getAttemptsForUser(int userId) {
    std::vector<Attempt> attempts;
    std::string query = 
        "SELECT id, user_id, test_id, score, total, percentage, passed"
         " FROM attempts WHERE user_id = " + std::to_string(userId);
    
    PGresult* res = PQexec(connection.get(), query.c_str());
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        throw std::runtime_error("Failed to get attempts: " + std::string(PQerrorMessage(connection.get())));
    }

    int rows = PQntuples(res);
    for(int i = 0; i < rows; ++i) {
        Attempt attempt;
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
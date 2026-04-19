#include "PostgresQuestionRepository.h"
#include <libpq-fe.h>
#include <map>

PostgresQuestionRepository::PostgresQuestionRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Question> PostgresQuestionRepository::getQuestionsForTest(int testId) {
    std::vector<Question> questions;

    std::string query =
        "SELECT q.id, q.test_id, q.text, q.correct_answer_id, "
        "a.id, a.text "
        "FROM questions q "
        "LEFT JOIN answers a ON q.id = a.question_id "
        "WHERE q.test_id = " + std::to_string(testId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());

    int rows = PQntuples(res);

    std::map<int, Question> questionMap;

    for (int i = 0; i < rows; ++i) {
        int qId = std::stoi(PQgetvalue(res, i, 0)); // id вопроса

        // если вопрос еще не создан - создаем
        if(questionMap.find(qId) == questionMap.end()) {
            Question q;
            q.id = qId;
            q.testId = std::stoi(PQgetvalue(res, i, 1));
            q.text = PQgetvalue(res, i, 2);
            q.correctAnswerId = std::stoi(PQgetvalue(res, i, 3));

            questionMap[qId] = q;
        }

        // если есть ответ - добавляем его к вопросу
        if (PQgetisnull(res, i, 4) == 0) {
            AnswerOption opt;
            opt.id = std::stoi(PQgetvalue(res, i, 4));
            opt.text = PQgetvalue(res, i, 5);

            questionMap[qId].options.push_back(opt);
        }
    }

    PQclear(res);
    for (auto& pair : questionMap) {
        questions.push_back(pair.second);
    }

    return questions;
}
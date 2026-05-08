#include "PostgresQuestionRepository.h"
#include <libpq-fe.h>
#include <map>
#include <stdexcept>

PostgresQuestionRepository::PostgresQuestionRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Question> PostgresQuestionRepository::getQuestionsForTest(int testId) {
    std::lock_guard<std::mutex> lock(db.mutex());
    std::vector<Question> questions;

    std::string query =
        "SELECT q.id, q.test_id, q.text, q.correct_answer_id, "
        "a.id, a.text "
        "FROM questions q "
        "LEFT JOIN answers a ON q.id = a.question_id "
        "WHERE q.test_id = " + std::to_string(testId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get questions for test: " + error);
    }

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

std::optional<Question> PostgresQuestionRepository::getQuestionById(int questionId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string query =
        "SELECT q.id, q.test_id, q.text, q.correct_answer_id, "
        "a.id, a.text "
        "FROM questions q "
        "LEFT JOIN answers a ON q.id = a.question_id "
        "WHERE q.id = " + std::to_string(questionId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get question by id: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    Question question;
    question.id = std::stoi(PQgetvalue(res, 0, 0));
    question.testId = std::stoi(PQgetvalue(res, 0, 1));
    question.text = PQgetvalue(res, 0, 2);
    question.correctAnswerId = std::stoi(PQgetvalue(res, 0, 3));

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        if (PQgetisnull(res, i, 4) == 0) {
            AnswerOption option;
            option.id = std::stoi(PQgetvalue(res, i, 4));
            option.text = PQgetvalue(res, i, 5);
            question.options.push_back(option);
        }
    }

    PQclear(res);
    return question;
}

Question PostgresQuestionRepository::createQuestion(
    int testId,
    const std::string& text,
    const std::vector<std::string>& options,
    int correctOptionIndex) {
    std::lock_guard<std::mutex> lock(db.mutex());

    PGresult* beginRes = PQexec(db.get(), "BEGIN");
    if (PQresultStatus(beginRes) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(beginRes);
        throw std::runtime_error("Failed to start transaction for question creation: " + error);
    }
    PQclear(beginRes);

    try {
        std::string testIdValue = std::to_string(testId);
        const char* questionParams[] = {
            testIdValue.c_str(),
            text.c_str()
        };

        PGresult* questionRes = PQexecParams(
            db.get(),
            "INSERT INTO questions (test_id, text, correct_answer_id) "
            "VALUES ($1, $2, NULL) "
            "RETURNING id, test_id, text",
            2,
            nullptr,
            questionParams,
            nullptr,
            nullptr,
            0);

        if (PQresultStatus(questionRes) != PGRES_TUPLES_OK) {
            std::string error = PQerrorMessage(db.get());
            PQclear(questionRes);
            throw std::runtime_error("Failed to create question: " + error);
        }

        Question question;
        question.id = std::stoi(PQgetvalue(questionRes, 0, 0));
        question.testId = std::stoi(PQgetvalue(questionRes, 0, 1));
        question.text = PQgetvalue(questionRes, 0, 2);
        PQclear(questionRes);

        int correctAnswerId = -1;

        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
            std::string questionIdValue = std::to_string(question.id);
            const char* answerParams[] = {
                questionIdValue.c_str(),
                options[i].c_str()
            };

            PGresult* answerRes = PQexecParams(
                db.get(),
                "INSERT INTO answers (question_id, text) "
                "VALUES ($1, $2) "
                "RETURNING id, text",
                2,
                nullptr,
                answerParams,
                nullptr,
                nullptr,
                0);

            if (PQresultStatus(answerRes) != PGRES_TUPLES_OK) {
                std::string error = PQerrorMessage(db.get());
                PQclear(answerRes);
                throw std::runtime_error("Failed to create answer option: " + error);
            }

            AnswerOption option;
            option.id = std::stoi(PQgetvalue(answerRes, 0, 0));
            option.text = PQgetvalue(answerRes, 0, 1);
            question.options.push_back(option);

            if (i == correctOptionIndex) {
                correctAnswerId = option.id;
            }

            PQclear(answerRes);
        }

        std::string questionIdValue = std::to_string(question.id);
        std::string correctIdValue = std::to_string(correctAnswerId);
        const char* updateParams[] = {
            correctIdValue.c_str(),
            questionIdValue.c_str()
        };

        PGresult* updateRes = PQexecParams(
            db.get(),
            "UPDATE questions SET correct_answer_id = $1 WHERE id = $2",
            2,
            nullptr,
            updateParams,
            nullptr,
            nullptr,
            0);

        if (PQresultStatus(updateRes) != PGRES_COMMAND_OK) {
            std::string error = PQerrorMessage(db.get());
            PQclear(updateRes);
            throw std::runtime_error("Failed to set correct answer: " + error);
        }
        PQclear(updateRes);

        PGresult* commitRes = PQexec(db.get(), "COMMIT");
        if (PQresultStatus(commitRes) != PGRES_COMMAND_OK) {
            std::string error = PQerrorMessage(db.get());
            PQclear(commitRes);
            throw std::runtime_error("Failed to commit question creation: " + error);
        }
        PQclear(commitRes);

        question.correctAnswerId = correctAnswerId;
        return question;
    } catch (...) {
        PGresult* rollbackRes = PQexec(db.get(), "ROLLBACK");
        PQclear(rollbackRes);
        throw;
    }
}

Question PostgresQuestionRepository::updateQuestion(
    int questionId,
    const std::string& text,
    const std::vector<std::string>& options,
    int correctOptionIndex) {
    std::lock_guard<std::mutex> lock(db.mutex());

    PGresult* beginRes = PQexec(db.get(), "BEGIN");
    if (PQresultStatus(beginRes) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(beginRes);
        throw std::runtime_error("Failed to start transaction for question update: " + error);
    }
    PQclear(beginRes);

    try {
        std::string questionIdValue = std::to_string(questionId);
        const char* questionParams[] = {
            text.c_str(),
            questionIdValue.c_str()
        };

        PGresult* questionRes = PQexecParams(
            db.get(),
            "UPDATE questions SET text = $1, correct_answer_id = NULL "
            "WHERE id = $2 "
            "RETURNING id, test_id, text",
            2,
            nullptr,
            questionParams,
            nullptr,
            nullptr,
            0);

        if (PQresultStatus(questionRes) != PGRES_TUPLES_OK) {
            std::string error = PQerrorMessage(db.get());
            PQclear(questionRes);
            throw std::runtime_error("Failed to update question: " + error);
        }

        if (PQntuples(questionRes) == 0) {
            PQclear(questionRes);
            throw std::runtime_error("Question not found");
        }

        Question question;
        question.id = std::stoi(PQgetvalue(questionRes, 0, 0));
        question.testId = std::stoi(PQgetvalue(questionRes, 0, 1));
        question.text = PQgetvalue(questionRes, 0, 2);
        PQclear(questionRes);

        const char* deleteParams[] = {questionIdValue.c_str()};
        PGresult* deleteAnswersRes = PQexecParams(
            db.get(),
            "DELETE FROM answers WHERE question_id = $1",
            1,
            nullptr,
            deleteParams,
            nullptr,
            nullptr,
            0);

        if (PQresultStatus(deleteAnswersRes) != PGRES_COMMAND_OK) {
            std::string error = PQerrorMessage(db.get());
            PQclear(deleteAnswersRes);
            throw std::runtime_error("Failed to replace answer options: " + error);
        }
        PQclear(deleteAnswersRes);

        int correctAnswerId = -1;
        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
            const char* answerParams[] = {
                questionIdValue.c_str(),
                options[i].c_str()
            };

            PGresult* answerRes = PQexecParams(
                db.get(),
                "INSERT INTO answers (question_id, text) "
                "VALUES ($1, $2) "
                "RETURNING id, text",
                2,
                nullptr,
                answerParams,
                nullptr,
                nullptr,
                0);

            if (PQresultStatus(answerRes) != PGRES_TUPLES_OK) {
                std::string error = PQerrorMessage(db.get());
                PQclear(answerRes);
                throw std::runtime_error("Failed to recreate answer option: " + error);
            }

            AnswerOption option;
            option.id = std::stoi(PQgetvalue(answerRes, 0, 0));
            option.text = PQgetvalue(answerRes, 0, 1);
            question.options.push_back(option);

            if (i == correctOptionIndex) {
                correctAnswerId = option.id;
            }

            PQclear(answerRes);
        }

        std::string correctIdValue = std::to_string(correctAnswerId);
        const char* updateParams[] = {
            correctIdValue.c_str(),
            questionIdValue.c_str()
        };

        PGresult* correctRes = PQexecParams(
            db.get(),
            "UPDATE questions SET correct_answer_id = $1 WHERE id = $2",
            2,
            nullptr,
            updateParams,
            nullptr,
            nullptr,
            0);

        if (PQresultStatus(correctRes) != PGRES_COMMAND_OK) {
            std::string error = PQerrorMessage(db.get());
            PQclear(correctRes);
            throw std::runtime_error("Failed to update correct answer: " + error);
        }
        PQclear(correctRes);

        PGresult* commitRes = PQexec(db.get(), "COMMIT");
        if (PQresultStatus(commitRes) != PGRES_COMMAND_OK) {
            std::string error = PQerrorMessage(db.get());
            PQclear(commitRes);
            throw std::runtime_error("Failed to commit question update: " + error);
        }
        PQclear(commitRes);

        question.correctAnswerId = correctAnswerId;
        return question;
    } catch (...) {
        PGresult* rollbackRes = PQexec(db.get(), "ROLLBACK");
        PQclear(rollbackRes);
        throw;
    }
}

void PostgresQuestionRepository::deleteQuestion(int questionId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string questionIdValue = std::to_string(questionId);
    const char* params[] = {questionIdValue.c_str()};

    PGresult* res = PQexecParams(
        db.get(),
        "DELETE FROM questions WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to delete question: " + error);
    }

    PQclear(res);
}

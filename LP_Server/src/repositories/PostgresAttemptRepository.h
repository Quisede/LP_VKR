#pragma once

#include "AttemptRepository.h"
#include "../database/PostgresConnection.h"
#include <vector>

class PostgresAttemptRepository : public AttemptRepository {
    public:
        // конструктор, который принимает ссылку на объект PostgresConnection для взаимодействия с базой данных
        PostgresAttemptRepository(PostgresConnection& connection);

        // реализация виртуальных методов из AttemptRepository для сохранения попытки и получения попыток для пользователя
        void saveAttempt(const Attempt& attempt) override;


        // реализация метода для получения всех попыток, связанных с определенным пользователем, из базы данных
        std::vector<Attempt> getAttemptsForUser(int userId) override;
        int countAttemptsForUserTest(int userId, int testId) override;
        CourseAnalytics getCourseAnalytics(int courseId) override;
        std::vector<StudentCourseAttempt> getStudentCourseAttempts(int courseId, int studentId) override;
    private:
        void ensureSchema();
        // ссылка на объект PostgresConnection для выполнения операций с базой данных
        PostgresConnection& connection;
        bool schemaChecked = false;
};

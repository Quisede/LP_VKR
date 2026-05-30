#include "AdminController.h"

#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

namespace {
std::string roleToString(UserRole role)
{
    switch (role) {
        case UserRole::Student: return "Student";
        case UserRole::Teacher: return "Teacher";
        case UserRole::Admin: return "Admin";
    }
    return "Unknown";
}
}

AdminController::AdminController(AdminService& adminService, AuthService& authService, JwtService& jwtService)
    : adminService(adminService),
      authService(authService),
      jwtService(jwtService) {}

void AdminController::registerRoutes(httplib::Server& server)
{
    server.Get("/api/admin/overview", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            const auto overview = adminService.getOverview(auth.userId, auth.role);

            json response;
            response["totalUsers"] = overview.totalUsers;
            response["studentsCount"] = overview.studentsCount;
            response["teachersCount"] = overview.teachersCount;
            response["adminsCount"] = overview.adminsCount;
            response["coursesCount"] = overview.coursesCount;
            response["lessonsCount"] = overview.lessonsCount;
            response["testsCount"] = overview.testsCount;
            response["enrollmentsCount"] = overview.enrollmentsCount;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Get("/api/admin/users", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto users = adminService.getAllUsers(auth.userId, auth.role);

            json response;
            response["users"] = json::array();

            for (const auto& user : users) {
                response["users"].push_back({
                    {"id", user.id},
                    {"login", user.login},
                    {"role", roleToString(user.role)},
                    {"firstName", user.firstName},
                    {"lastName", user.lastName},
                    {"groupName", user.groupName},
                    {"email", user.email},
                    {"phone", user.phone}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Get("/api/admin/groups", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto groups = adminService.getGroups(auth.userId, auth.role);

            json response;
            response["groups"] = json::array();
            for (const auto& group : groups) {
                response["groups"].push_back(group);
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post("/api/admin/groups", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto body = json::parse(req.body);
            const std::string groupName = controller_utils::requiredJsonString(body, "name");
            const std::string created = adminService.createGroup(auth.userId, auth.role, groupName);

            json response;
            response["name"] = created;
            response["success"] = true;
            res.status = 201;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Put("/api/admin/groups", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto body = json::parse(req.body);
            const std::string oldName = controller_utils::requiredJsonString(body, "oldName");
            const std::string newName = controller_utils::requiredJsonString(body, "newName");
            const std::string renamed = adminService.renameGroup(auth.userId, auth.role, oldName, newName);

            json response;
            response["name"] = renamed;
            response["success"] = true;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Delete("/api/admin/groups", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto body = json::parse(req.body);
            const std::string groupName = controller_utils::requiredJsonString(body, "name");
            adminService.deleteGroup(auth.userId, auth.role, groupName);
            res.status = 204;
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Get("/api/admin/audit", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            int limit = 12;
            if (req.has_param("limit")) {
                limit = std::stoi(req.get_param_value("limit"));
            }

            const auto events = adminService.getAuditEvents(auth.userId, auth.role, limit);

            json response;
            response["events"] = json::array();

            for (const auto& event : events) {
                response["events"].push_back({
                    {"id", event.id},
                    {"adminId", event.adminId},
                    {"adminLogin", event.adminLogin},
                    {"action", event.action},
                    {"targetType", event.targetType},
                    {"targetId", event.targetId},
                    {"details", event.details},
                    {"createdAt", event.createdAt}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post("/api/admin/users", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Admin) {
                throw std::invalid_argument("Only admins can create users");
            }

            auto body = json::parse(req.body);
            const std::string login = controller_utils::requiredJsonString(body, "login");
            const std::string password = controller_utils::requiredJsonString(body, "password");
            const UserRole role = controller_utils::parseUserRole(
                controller_utils::requiredJsonString(body, "role"));
            const std::string firstName = body.value("firstName", "");
            const std::string lastName = body.value("lastName", "");
            const std::string groupName = body.value("groupName", "");
            const std::string email = body.value("email", "");
            const std::string phone = body.value("phone", "");

            AuthResult result = authService.registerUser(
                login,
                password,
                role,
                firstName,
                lastName,
                groupName,
                email,
                phone);
            if (!result.success) {
                throw std::invalid_argument(result.errorMessage);
            }

            adminService.recordAuditEvent(
                auth.userId,
                auth.role,
                "user.created",
                "user",
                result.userId,
                "Created user " + login + " with role " + roleToString(role));

            json response;
            response["success"] = true;
            response["userId"] = result.userId;
            response["role"] = roleToString(result.role);
            response["login"] = login;
            response["firstName"] = result.firstName;
            response["lastName"] = result.lastName;
            response["groupName"] = result.groupName;
            response["email"] = result.email;
            response["phone"] = result.phone;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Put(R"(/api/admin/users/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto body = json::parse(req.body);
            const int userId = std::stoi(req.matches[1].str());
            const UserRole role = controller_utils::parseUserRole(
                controller_utils::requiredJsonString(body, "role"));

            User user = adminService.updateUserRole(auth.userId, auth.role, userId, role);

            json response;
            response["id"] = user.id;
            response["login"] = user.login;
            response["role"] = roleToString(user.role);
            response["success"] = true;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Delete(R"(/api/admin/users/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            const int userId = std::stoi(req.matches[1].str());
            adminService.deleteUser(auth.userId, auth.role, userId);

            json response;
            response["success"] = true;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}

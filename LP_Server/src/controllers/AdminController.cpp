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
                    {"role", roleToString(user.role)}
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

            AuthResult result = authService.registerUser(login, password, role);
            if (!result.success) {
                throw std::invalid_argument(result.errorMessage);
            }

            json response;
            response["success"] = true;
            response["userId"] = result.userId;
            response["role"] = roleToString(result.role);
            response["login"] = login;
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

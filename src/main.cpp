
#pragma once
#include <iostream>
#include <crow.h>
#include <regex>
#include "controllers/LibraryController.h"
#include "controllers/UserController.h"
//#include <bcrypt/bcrypt.h>
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>
class ILocalInterface {
    public:
    virtual bool allowRequest(const std:: string& path, std::string& method)const = 0;
    virtual ~ILocalInterface() = default;
};
class LoclaService : public ILocalInterface {
    public:
    virtual bool allowRequest(const std:: string& path, std::string& method)const override {
        /** Condition
        if (path == "/") {
            method = "GET";
            */
        }
    virtual ~LoclaService() = default;
};
using json = nlohmann::json;
bool is_valid_email(const std::string& email) {
    const std::regex pattern(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
    return std::regex_match(email, pattern);
}
std::string sanitize_input(const std::string& input) {
    std::string safe = input;
    // Supprimer les balises htm
    safe = std::regex_replace(safe, std::regex("<.*?>"), "");
    return safe;
}
int main() {
    crow::SimpleApp app;
    /***/
    LibraryController library;
    UserController user;
    user.register_routes(app);
    library.register_routes(app);
    CROW_ROUTE(app,"/")([]() {
       return "Hello, World from Crow!";
    });
    CROW_ROUTE(app,"/<string>")([](const crow::request &req, const std::string &value) {
        return crow::response("Hello  "+ value+ "!");
    });
    CROW_ROUTE(app,"/users").methods(crow::HTTPMethod::Post)([](const crow::request &req) {

    });
    app.port(9080).multithreaded().run();
    return 0;
}

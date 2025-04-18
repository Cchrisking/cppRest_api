#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

// User data structure
struct User {
    int id;
    std::string name;
    std::string email;

    // Convert User to JSON
    json to_json() const {
        json j;
        j["id"] = id;
        j["name"] = name;
        j["email"] = email;
        return j;
    }

    // Create User from JSON
    static User from_json(const json& j) {
        User user;
        user.id = j["id"];
        user.name = j["name"];
        user.email = j["email"];
        return user;
    }
};

// In-memory database
std::vector<User> users = {
    {1, "John Doe", "john.doe@example.com"},
    {2, "Jane Doe", "jane.doe@example.com"}
};

// Helper functions
int get_next_id() {
    if (users.empty()) {
        return 1;
    }
    return std::max_element(users.begin(), users.end(),
        [](const User& a, const User& b) { return a.id < b.id; })->id + 1;
}

User* find_user_by_id(int id) {
    auto it = std::find_if(users.begin(), users.end(),
        [id](const User& user) { return user.id == id; });

    if (it != users.end()) {
        return &(*it);
    }
    return nullptr;
}

int main() {
    // Create Crow application
    crow::SimpleApp app;

    // GET /users - Get all users
    CROW_ROUTE(app, "/users")
        .methods(crow::HTTPMethod::GET)
        ([](const crow::request& req) {
            json j = json::array();
            for (const auto& user : users) {
                j.push_back(user.to_json());
            }
            return crow::response{j.dump(2)};
        });

    // POST /users - Create a new user
    CROW_ROUTE(app, "/users")
        .methods(crow::HTTPMethod::POST)
        ([](const crow::request& req) {
            try {
                json data = json::parse(req.body);

                // Validate required fields
                if (!data.contains("name") || !data.contains("email")) {
                    return crow::response(400, "Name and email are required");
                }

                User newUser;
                newUser.id = get_next_id();
                newUser.name = data["name"];
                newUser.email = data["email"];

                users.push_back(newUser);

                return crow::response(201, newUser.to_json().dump(2));
            } catch (const std::exception& e) {
                return crow::response(400, std::string("Invalid JSON: ") + e.what());
            }
        });

    // GET /users/{id} - Get user by ID
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::GET)
        ([](int id) {
            User* user = find_user_by_id(id);
            if (!user) {
                return crow::response(404, "User not found");
            }

            return crow::response(200, user->to_json().dump(2));
        });

    // PUT /users/{id} - Update user
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::PUT)
        ([](const crow::request& req, int id) {
            User* user = find_user_by_id(id);
            if (!user) {
                return crow::response(404, "User not found");
            }

            try {
                json data = json::parse(req.body);

                // Update fields if provided
                if (data.contains("name")) {
                    user->name = data["name"];
                }
                if (data.contains("email")) {
                    user->email = data["email"];
                }

                return crow::response(200, user->to_json().dump(2));
            } catch (const std::exception& e) {
                return crow::response(400, std::string("Invalid JSON: ") + e.what());
            }
        });

    // DELETE /users/{id} - Delete user
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::DELETE)
        ([](int id) {
            auto it = std::find_if(users.begin(), users.end(),
                [id](const User& user) { return user.id == id; });

            if (it == users.end()) {
                return crow::response(404, "User not found");
            }

            users.erase(it);

            // Return success with no content
            return crow::response(204);
        });

    // Set the port, start the server, and run the event loop
    app.port(9080).multithreaded().run();

    return 0;
}
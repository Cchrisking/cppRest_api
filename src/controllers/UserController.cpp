//
// Created by christou on 5/5/25.
//
#include "UserController.h"
#include "UserController.h"
#include <algorithm>
#include <iostream>
// Constructor - Initialize with some example users
UserController::UserController() : next_id(3) {
    users = {
        {1, "John Doe", "john.doe@example.com"},
        {2, "Jane Doe", "jane.doe@example.com"}
    };
}
// Find a user by ID
User* UserController::find_user_by_id(int id) {
    auto it = std::find_if(users.begin(), users.end(),
        [id](const User& user) { return user.id == id; });
    if (it != users.end()) {
        return &(*it);
    }
    return nullptr;
}
// Get the next available ID
int UserController::get_next_id() {
    if (users.empty()) {
        return 1;
    }
    return std::max_element(users.begin(), users.end(),
        [](const User& a, const User& b) { return a.id < b.id; })->id + 1;
}
// Get all users
crow::response UserController::get_all_users(const crow::request& req) {
    json j = json::array();
    for (const auto& user : users) {
        j.push_back(user.to_json());
    }
    return crow::response{j.dump(2)};
}
// Create a new user
crow::response UserController::create_user(const crow::request& req) {
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
    try {
        /*Parse Json*/
        json json_data=json::parse(req.body);
        std::string new_user = json_data.dump(4);
        std::string name = sanitize_input(json_data["name"]);
        std::string email = sanitize_input(json_data["email"]);
        std::string password = json_data["password"];
        if (!is_valid_email(email)) {
            return crow::response(400, "Invalid email format");
        }
        if (password.length() < 6) {
            return crow::response(400, "Password too short (min 6 chars)");
        }
        //Todo sanitize
        /*Hasher le mot de passe*/
        //std::string hash = BCrypt::generateHash(password);
        /*Stocker dans la base donnée*/
        return crow::response(crow::OK,"Utilisateur: \n"+ new_user +" Ajouté avec suceess");
    }catch(const std::exception &e) {
        return crow::response(400,e.what());
    }
}
// Get user by ID
crow::response UserController::get_user_by_id(int id) {
    User* user = find_user_by_id(id);
    if (!user) {
        return crow::response(404, "User not found");
    }
    return crow::response(200, user->to_json().dump(2));
}
// Update a user
crow::response UserController::update_user(const crow::request& req, int id) {
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
}
// Delete a user
crow::response UserController::delete_user(int id) {
    auto it = std::find_if(users.begin(), users.end(),
        [id](const User& user) { return user.id == id; });
    if (it == users.end()) {
        return crow::response(404, "User not found");
    }
    users.erase(it);
    // Return success with no content
    return crow::response(204);
}
// Register routes with the Crow app
void UserController::register_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/users")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            return this->get_all_users(req);
        });
    CROW_ROUTE(app, "/users")
        .methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            return this->create_user(req);
        });
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::GET)
        ([this](int id) {
            return this->get_user_by_id(id);
        });
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::PUT)
        ([this](const crow::request& req, int id) {
            return this->update_user(req, id);
        });
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::DELETE)
        ([this](int id) {
            return this->delete_user(id);
        });
}
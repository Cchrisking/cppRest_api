//
// Created by christou on 5/5/25.
//
#include "UserController.h"
#include "UserController.h"
#include <algorithm>
#include <iostream>
#include "../utils/utils.h"
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
  crow::response response;
  response.add_header("Access-Control-Allow-Origin", "*");
  response.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response.add_header("Access-Control-Allow-Headers", "Content-type");
    json j = json::array();
    for (const auto& user : users) {
        j.push_back(user.to_json());
    }
    response.body = j.dump(2);
    return response;
	}
// Create a new user
crow::response UserController::create_user(const crow::request& req) {
  crow::response response;
  response.add_header("Access-Control-Allow-Origin", "*");
  response.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
  response.add_header("Access-Control-Allow-Headers", "Content-type");
    try {
        json data = json::parse(req.body);
        // Validate required fields
        if (!data.contains("name") || !data.contains("email")) {
          response.code = 400;
          response.body = "Nom et email obbligatoire";
            return response;
        }
        User newUser;
        newUser.id = get_next_id();
        newUser.name = data["name"];
        newUser.email = data["email"];
        users.push_back(newUser);
        response.code = 201;
        response.body = newUser.to_json().dump(2);
        return response;
    } catch (const std::exception& e) {
      response.code = 400;
      response.body = std::string("Json invalide")+e.what();
        return response;
    }
}
// Get user by ID
crow::response UserController::get_user_by_id(int id) {
  crow::response response;
  response.add_header("Access-Control-Allow-Origin", "*");
  response.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response.add_header("Access-Control-Allow-Headers", "Content-Type");
    User* user = find_user_by_id(id);
    if (!user) {
      response.code = 404;
      response.body = "Utilisateur introuvable";
        return response;
    }
    response.code =200;
    response.body = user->to_json().dump(2);
    return response;
}
// Update a user
crow::response UserController::update_user(const crow::request& req, int id) {
  crow::response response;
  response.add_header("Access-Control-Allow-Origin", "*");
  response.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response.add_header("Access-Control-Allow-Headers", "Content-Type");
    User* user = find_user_by_id(id);
    if (!user) {
      response.code = 404;
      response.body = "Utilisateur introuvable";
        return response;
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
        response.code = 200;
      	response.body = user->to_json().dump(2);
        return response;
    } catch (const std::exception& e) {
      response.code = 400;
      response.body = std::string("Json invalide")+ e.what();
      return response;
    }
}
// Delete a user
crow::response UserController::delete_user(int id) {
  crow::response response;
  response.add_header("Access-Control-Allow-Origin", "*");
  response.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response.add_header("Access-Control-Allow-Headers", "Content-Type");
    auto it = std::find_if(users.begin(), users.end(),
        [id](const User& user) { return user.id == id; });
    if (it == users.end()) {
      response.code = 404;
      response.body = "Utilisateur introuvable";
        return response;
    }
    users.erase(it);
    // Return success with no content
    std::cout << "Received DELETE  request to /users with body: "<< std::endl;
    response.code = 204;
    response.body = "Deleted user not found";
    std::cout<<response.code<<" "<<response.body<<std::endl;
    return response;
}
// Register routes with the Crow app
void UserController::register_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/users")
        .methods(crow::HTTPMethod::OPTIONS)
        ([this](const crow::request& req) {
            std::cout << "Received OPTIONS for /users"<< std::endl;
            crow::response response;
            response.add_header("Access-Control-Allow-Origin", "*");
            response.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            response.add_header("Access-Control-Allow-Headers", "Content-Type");
            response.code = 204;  // No content for OPTIONS
            return response;
        });
    CROW_ROUTE(app, "/users")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            std::cout <<"Received GET request to /users with body: "<< req.body << std::endl;
            return this->get_all_users(req);
        });
    CROW_ROUTE(app, "/users")
        .methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            std::cout <<"Received POST request to /users with body: " << req.body << std::endl;
            return this->create_user(req);
        });
    // OPTIONS handler for the specific ID route
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::OPTIONS)
        ([this](int id) {
            crow::response response;
            std::cout << "Received OPTIONS/users/id"<< std::endl;
            response.add_header("Access-Control-Allow-Origin", "*");
            response.add_header("Access-Control-Allow-Methods", "GET, PUT, DELETE, OPTIONS");
            response.add_header("Access-Control-Allow-Headers", "Content-Type");
            response.code = 204;  // No content for OPTIONS
            return response;
        });
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::GET)
        ([this](int id) {
              std::cout << "Received GET request to /users: "<<id<< std::endl;
            return this->get_user_by_id(id);
        });
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::PUT)
        ([this](const crow::request& req, int id) {
              std::cout << "Received PUT request to /users: "<<id<<" "<< req.body << std::endl;
            return this->update_user(req, id);
        });
    CROW_ROUTE(app, "/users/<int>")
        .methods(crow::HTTPMethod::DELETE)
        ([this](int id) {
              std::cout << "Received GET request to /users "<<id<<""<< std::endl;
            return this->delete_user(id);
        });
}
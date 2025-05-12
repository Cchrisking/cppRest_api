//
// Created by christou on 5/5/25.
//
#pragma once
#ifndef USER_H
#define USER_H
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class User {
public:
    int id;
    std::string name;
    std::string email;
    // Default constructor
    User() ;
    // Constructor with parameters
    User(int id, std::string name, std::string email);
    // Convert User to JSON
    json to_json() const;
    // Create User from JSON
    static User from_json(const json& j) {
        User user;
        user.id = j["id"];
        user.name = j["name"];
        user.email = j["email"];
        return user;
    }
};
#endif //USER_H

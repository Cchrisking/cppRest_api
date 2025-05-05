//
// Created by christou on 5/5/25.
//
#pragma once
#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include <vector>
#include <crow.h>
#include "../models/User.h"

class UserController {
private:
    std::vector<User> users;
    int next_id;

    // Helper method to find a user by ID
    User* find_user_by_id(int id);

    // Helper method to get the next available ID
    int get_next_id();

public:
    // Constructor
    UserController();
    // API Endpoints
    crow::response get_all_users(const crow::request& req);
    crow::response create_user(const crow::request& req);
    crow::response get_user_by_id(int id);
    crow::response update_user(const crow::request& req, int id);
    crow::response delete_user(int id);

    // Enregistrer les routes avec Crow app
    void register_routes(crow::SimpleApp& app);
};



#endif //USERCONTROLLER_H

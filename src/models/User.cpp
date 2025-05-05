//
// Created by christou on 5/5/25.
//

#include "User.h"

User::User() : id(0), name(""), email("") {
}
User::User(int id, std::string name, std::string email)
        : id(id), name(std::move(name)), email(std::move(email)) {

}
json User::to_json() const {
        json j;
        j["id"] = id;
        j["name"] = name;
        j["email"] = email;
        return j;
}
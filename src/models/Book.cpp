//
// Created by christou on 5/5/25.
//

#include "Book.h"

// Default constructor
Book:: Book() : id(0), title(""), author("") {}

// Constructor with parameters
Book:: Book(int id, std::string title, std::string author)
    : id(id), title(std::move(title)), author(std::move(author)) {}
json Book:: to_json() const {
    json j;
    j["id"] = id;
    j["title"] = title;
    j["author"] = author;
    return j;
}
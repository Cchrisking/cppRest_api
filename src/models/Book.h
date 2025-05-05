//
// Created by christou on 5/5/25.
//

#ifndef BOOK_H
#define BOOK_H



#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Book {
public:
    int id;
    std::string title;
    std::string author;
    // Default constructor
    Book();
    // Constructor with parameters
    Book(int id, std::string title, std::string author);
    // Convert Book to JSON
    json to_json() const ;
    // Create Book from JSON
    static Book from_json(const json& j) {
        Book book;
        book.id = j["id"];
        book.title = j["title"];
        book.author = j["author"];
        return book;
    }
};

#endif //BOOK_H

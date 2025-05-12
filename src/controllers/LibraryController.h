//
// Created by christou on 5/5/25.
//

#ifndef LIBRARYCONTROLLER_H
#define LIBRARYCONTROLLER_H



#pragma once

#include <vector>
#include <crow.h>
#include "../models/Book.h"

class LibraryController {
private:
    std::vector<Book> books;
    int next_id;
    // Helper methode to find a book by ID
    std::vector<Book>::iterator find_book_by_id(int book_id);

public:
    // Constructeur par defaut
    LibraryController();
    // API Endpoints
    crow::response get_all_books(const crow::request& req);
    crow::response create_book(const crow::request& req);
    crow::response get_book_by_id(int id);
    crow::response update_book(const crow::request& req, int id);
    crow::response delete_book(int id);
    // Enregistrer les routes avec Crow app
    void register_routes(crow::SimpleApp& app);
};


#endif //LIBRARYCONTROLLER_H

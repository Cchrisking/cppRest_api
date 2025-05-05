//
// Created by christou on 5/5/25.
//

#include "LibraryController.h"
#include "LibraryController.h"
#include <algorithm>
#include <iostream>

// Constructor - Initialize with some example books
LibraryController::LibraryController() : next_id(3) {
    books = {
        {1, "The Hitchhiker's Guide to the Galaxy", "Douglas Adams"},
        {2, "Pride and Prejudice", "Jane Austen"}
    };
}

// Find a book by ID
std::vector<Book>::iterator LibraryController::find_book_by_id(int book_id) {
    return std::find_if(books.begin(), books.end(),
                        [book_id](const Book& b) { return b.id == book_id; });
}

// Get all books
crow::response LibraryController::get_all_books(const crow::request& req) {
    // Return all books as JSON
    json j_array = json::array();
    for (auto& book : books) {
        j_array.push_back(book.to_json());
    }
    return crow::response(j_array.dump(4)); // Pretty print with indent of 4
}

// Create a new book
crow::response LibraryController::create_book(const crow::request& req) {
    try {
        // Parse the request body to get new book data
        auto x = json::parse(req.body);
        if (!x.contains("title") || !x.contains("author")) {
            return crow::response(400, "Missing required fields: title and author");
        }

        // Create a new book with auto-generated ID
        Book newBook = {
            next_id++,
            x["title"].get<std::string>(),
            x["author"].get<std::string>()
        };
        books.push_back(newBook);

        // Return the newly created book as confirmation
        json response = {
            {"message", "Book added successfully"},
            {"book", newBook.to_json()}
        };
        return crow::response(201, response.dump(4));
    } catch (const std::exception& e) {
        return crow::response(400, std::string{"Invalid JSON: "} + e.what());
    }
}

// Get a book by ID
crow::response LibraryController::get_book_by_id(int id) {
    auto it = find_book_by_id(id);
    if (it == books.end()) {
        return crow::response(404, "Book not found");
    }
    // Return the specific book as JSON
    return crow::response(it->to_json().dump(4));
}

// Update a book
crow::response LibraryController::update_book(const crow::request& req, int id) {
    auto it = find_book_by_id(id);
    if (it == books.end()) {
        return crow::response(404, "Book not found");
    }

    try {
        // Parse the request body to get updated book data
        auto x = json::parse(req.body);
        // Update the book's fields if they are present in the request
        if (x.contains("title")) {
            it->title = x["title"].get<std::string>();
        }
        if (x.contains("author")) {
            it->author = x["author"].get<std::string>();
        }
        // Return the updated book as confirmation
        json response = {
            {"message", "Book updated successfully"},
            {"book", it->to_json()}
        };
        return crow::response(200, response.dump(4));
    } catch (const std::exception& e) {
        return crow::response(400, std::string{"Invalid JSON: "} + e.what());
    }
}

// Delete a book
crow::response LibraryController::delete_book(int id) {
    auto it = find_book_by_id(id);
    if (it == books.end()) {
        return crow::response(404, "Book not found");
    }

    // Remove the book from the vector
    books.erase(it);

    json response = {
        {"message", "Book deleted successfully"},
        {"id", id}
    };
    return crow::response(200, response.dump(4));
}

// Register routes with the Crow app
void LibraryController::register_routes(crow::SimpleApp& app) {
    // GET all books and POST new books
    CROW_ROUTE(app, "/books")
    .methods("GET"_method, "POST"_method)
    ([this](const crow::request& req) {
        if (req.method == "GET"_method) {
            return this->get_all_books(req);
        }
        else if (req.method == "POST"_method) {
            return this->create_book(req);
        }
        return crow::response(405, "Method not allowed");
    });
    // GET, PUT, DELETE a specific book by ID
    CROW_ROUTE(app, "/books/<int>")
    .methods("GET"_method, "PUT"_method, "DELETE"_method)
    ([this](const crow::request& req, int book_id) {
        if (req.method == "GET"_method) {
            return this->get_book_by_id(book_id);
        }
        else if (req.method == "PUT"_method) {
            return this->update_book(req, book_id);
        }
        else if (req.method == "DELETE"_method) {
            return this->delete_book(book_id);
        }
        return crow::response(405, "Method not allowed");
    });
}
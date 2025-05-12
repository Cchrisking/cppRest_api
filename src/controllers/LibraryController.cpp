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
    crow::response response;
    // Add CORS headers
    response.add_header("Access-Control-Allow-Origin", "*");
    response.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    response.add_header("Access-Control-Allow-Headers", "Content-Type");
    json j_array = json::array();
    for (auto& book : books) {
        j_array.push_back(book.to_json());
    }
    // Set response body and code
    response.body = j_array.dump(4);
    response.code = 200;
    return response; // Pretty print with indent of 4
}

// Create a new book
crow::response LibraryController::create_book(const crow::request& req) {
    crow::response response;
    response.add_header("Access-Control-Allow-Origin", "*");
    response.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    response.add_header("Access-Control-Allow-Headers", "Content-Type");
    try {
        // Parse the request body to get new book data
        auto x = json::parse(req.body);
        if (!x.contains("title") || !x.contains("author")) {
          response.code = 400;
          response.body = "Missing required fields: title and author";
            return response;

        }

        // Create a new book with auto-generated ID
        Book newBook = {
            next_id++,
            x["title"].get<std::string>(),
            x["author"].get<std::string>()
        };
        books.push_back(newBook);

        // Return the newly created book as confirmation
        json jresponse = {
            {"message", "Book added successfully"},
            {"book", newBook.to_json()}
        };
        response.code = 201;
        response.body = jresponse.dump(4);
    } catch (const std::exception& e) {
      response.code = 400;
      response.body = std::string{"Invalid JSON: "} + e.what();
        return response;
    }
}
// Get a book by ID
crow::response LibraryController::get_book_by_id(int id) {
    auto it = find_book_by_id(id);
    crow::response response;
    response.add_header("Access-Control-Allow-Origin", "*");
    response.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
    response.add_header("Access-Control-Allow-Headers", "Content-Type");
    if (it == books.end()) {
      response.code = 404;
      response.body = "No books found";
        return response;
    }
    // Return the specific book as JSON
    response.body = it->to_json().dump(4);
    return response;
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
  crow::response response;
  response.add_header("Access-Control-Allow-Origin", "*");
  response.add_header("Access-Control-Allow-Methods", "DELETE");
  response.add_header("Access-Control-Allow-Headers", "Content-Type");
    auto it = find_book_by_id(id);
    if (it == books.end()) {
      response.code = 404;
      response.body = "No books found";
      return response;
    }
    // Remove the book from the vector
    books.erase(it);
    json jresponse = {
        {"message", "Book deleted successfully"},
        {"id", id}
    };
    response.code = 200;
    response.body = jresponse.dump(4);
    return response;
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
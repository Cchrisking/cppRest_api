#include <crow.h>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <algorithm>

using json = nlohmann::json;

struct Book {
    int id;
    std::string title;
    std::string author;
};

// Initialize with some example books
std::vector<Book> books = {
    {1, "The Hitchhiker's Guide to the Galaxy", "Douglas Adams"},
    {2, "Pride and Prejudice", "Jane Austen"}
};

// Counter for generating new book IDs
int next_id = 3;

int main() {
    crow::SimpleApp app;

    // GET all books and POST new books
    CROW_ROUTE(app, "/books")
    .methods("GET"_method, "POST"_method)
    ([&](const crow::request& req) {
        if (req.method == "GET"_method) {
            // Return all books as JSON
            json j_array = json::array();
            for (auto& book : books) {
                json j_book = {
                    {"id", book.id},
                    {"title", book.title},
                    {"author", book.author}
                };
                j_array.push_back(j_book);
            }
            return crow::response(j_array.dump(4)); // Pretty print with indent of 4
        }
        else if (req.method == "POST"_method) {
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
                    {"book", {
                        {"id", newBook.id},
                        {"title", newBook.title},
                        {"author", newBook.author}
                    }}
                };
                return crow::response(201, response.dump(4));
            } catch (const std::exception& e) {
                return crow::response(400, std::string{"Invalid JSON: "} + e.what());
            }
        }
        return crow::response(405, "Method not allowed");
    });

    // GET, PUT, DELETE a specific book by ID
    CROW_ROUTE(app, "/books/<int>")
    .methods("GET"_method, "PUT"_method, "DELETE"_method)
    ([&](const crow::request& req, int book_id) {
        // Find the book with the given ID
        auto it = std::find_if(books.begin(), books.end(),
                              [book_id](const Book& b) { return b.id == book_id; });

        if (it == books.end()) {
            return crow::response(404, "Book not found");
        }

        if (req.method == "GET"_method) {
            // Return the specific book as JSON
            json j_book = {
                {"id", it->id},
                {"title", it->title},
                {"author", it->author}
            };
            return crow::response(j_book.dump(4));
        }
        else if (req.method == "PUT"_method) {
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
                    {"book", {
                        {"id", it->id},
                        {"title", it->title},
                        {"author", it->author}
                    }}
                };
                return crow::response(200, response.dump(4));
            } catch (const std::exception& e) {
                return crow::response(400, std::string{"Invalid JSON: "} + e.what());
            }
        }
        else if (req.method == "DELETE"_method) {
            // Remove the book from the vector
            books.erase(it);

            json response = {
                {"message", "Book deleted successfully"},
                {"id", book_id}
            };
            return crow::response(200, response.dump(4));
        }

        return crow::response(405, "Method not allowed");
    });

    // Configure and start the server
    std::cout << "Server starting on http://localhost:8080" << std::endl;
    app.port(8080).multithreaded().run();
}
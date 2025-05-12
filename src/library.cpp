#include <crow.h>
#include <iostream>
#include "controllers/LibraryController.h"

int main() {
    // Create Crow application
    crow::SimpleApp app;
    // Create the library controller
    LibraryController libraryController;
    // Register the library routes with the app
    libraryController.register_routes(app);
    // Add a root route for testing
    CROW_ROUTE(app, "/")([]() {
        return "Book Management API is running!";
    });
    // Set the port, start the server, and run the event loop
    std::cout << "Starting Book Management Server on http://localhost:8080" << std::endl;
    app.port(8080).multithreaded().run();
    return 0;
}
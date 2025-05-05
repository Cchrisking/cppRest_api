//
// Created by christou on 4/24/25.
//

#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H



class User_controller {

};



#endif //USER_CONTROLLER_H

#include "crow_all.h"
#include <iostream>

// Step 1: Define the middleware
struct SimpleLogger {
    struct context {
        // Can store request-specific data if needed
    };

    // Before route handler
    void before_handle(crow::request& req, crow::response& res, context&) {
        std::cout << "[LOG] " << req.method_name() << " " << req.url << std::endl;

        if (req.url == "/forbidden") {
            res.code = 403;
            res.body = "Access forbidden";
            res.end(); // Ends response early
        }
    }

    // After route handler
    void after_handle(crow::request&, crow::response& res, context&) {
        std::cout << "[RESP] Status: " << res.code << std::endl;
    }
};

// Step 2: Declare app with middleware
int main() {
    crow::App<SimpleLogger> app;

    CROW_ROUTE(app, "/")([] {
        return "Welcome!";
    });

    CROW_ROUTE(app, "/hello")([] {
        return "Hello from Crow!";
    });

    CROW_ROUTE(app, "/forbidden")([] {
        return "You shouldn't see this"; // Will be blocked by middleware
    });

    app.port(8080).multithreaded().run();
}
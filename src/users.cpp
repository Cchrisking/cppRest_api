#include <crow.h>
#include <iostream>
#include "controllers/UserController.h"

int main() {
    /** Creation Crow application app*/
    crow::SimpleApp app;
    /** creation du controller */
    UserController userController;
    /** Enregistrer  les user routes avec crow simple app
    * @app
    */
    userController.register_routes(app);

    // Aceuille endpoint racine
    CROW_ROUTE(app, "/")([]() {
        return "API gestionaire d'utilisateurs !";
    });
    // Global OPTIONS handler for any unmatched OPTIONS requests
    CROW_ROUTE(app, "/*")
        .methods(crow::HTTPMethod::OPTIONS)
        ([](const crow::request& req) {
            std::cout << "Global OPTIONS handler for request: in main" <<std::endl;
            crow::response res;
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
            res.add_header("Access-Control-Max-Age", "3600");
            res.code = 204;
            return res;
        });
    // demmarer le server sur le port 9080
    std::cout << "Demmarage du server sur http://localhost:9080" << std::endl;
    app.port(9080).multithreaded().run();
    return 0;
}
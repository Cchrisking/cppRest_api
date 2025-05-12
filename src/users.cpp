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
    CROW_ROUTE(app, "/*").methods(crow::HTTPMethod::OPTIONS)
        ([](const crow::request& req) {
          crow::response res;
          res.set_header("Access-Control-Allow-Origin", "*");
          res.set_header("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
          res.set_header("Access-Control-Allow-Headers", "Content-Type");
          res.code = 204;
          std::cout<<"Gestionaire d'option globale pour requte "<<req.url<<std::endl;

    });
    app.after_request(
        [](const crow::request& req, crow::response& res) {
         res.set_header("Access-Control-Allow-Origin", "*");
         res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT,DELETE,OPTIONS");
         res.set_header("access-control-Allow-Headers", "Content-Type");
         res.set_header("Content-Type", "application/json");
        }
    );
    // demmarer le server sur le port 9080
    std::cout << "Demmarage du server sur http://localhost:9080" << std::endl;
    app.port(9080).multithreaded().run();
    return 0;
}
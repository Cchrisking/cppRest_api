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
    // demmarer le server sur le port 9080
    std::cout << "Demmarage du server sur http://localhost:9080" << std::endl;
    app.port(9080).multithreaded().run();
    return 0;
}
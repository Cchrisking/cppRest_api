#include <iostream>
#include <crow.h>

int main() {
    crow::SimpleApp app;
    CROW_ROUTE(app,"/")([]() {
       return "Hello, World from Crow!";
    });
    CROW_ROUTE(app,"/<string>")([](const crow::request &req, const std::string &value) {
        return crow::response("Hello, World! "+ value);
    });
    CROW_ROUTE(app,"/users").methods("GET"_method,"POST"_method)([](const crow::request &req) {
        if (req.method == "POST"_method) {
            auto admin = req.url_params.get("admin");
            return crow::response("method post passé "+ std::string(admin));
        }else if (req.method == "GET"_method) {
            return crow::response("method get passé");
        }
        return crow::response(400);
    });
    app.port(9080).multithreaded().run();
    return 0;
}

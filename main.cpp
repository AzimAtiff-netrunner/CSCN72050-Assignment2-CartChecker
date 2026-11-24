#include "crow_all.h"
#include <fstream>
#include <sstream>
std::string readFile(std::string path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/styles/<string>")([](const std::string& filename){
        crow::response res;
        res.set_header("Content-Type", "text/css");
        res.body = readFile("../public/styles/" + filename);
        return res;
    });

    CROW_ROUTE(app, "/images/<string>")([](const std::string& filename){
        crow::response res;
        res.set_header("Content-Type", "image/jpeg");
        res.body =  readFile("../public/images/" + filename);
        return res;
    });
    
    CROW_ROUTE(app, "/")//home
        ([]() {
        return readFile("../public/index.html");
     });

    CROW_ROUTE(app, "/product/<int>")
        ([](int id) {
        if (id == 1) {
            return readFile("../public/product1.html");
        }
        else if (id == 2) {
            return readFile("../public/product2.html");
        }
        else if (id == 3) {
            return readFile("../public/product3.html");
        }
        else if (id == 4) {
            return readFile("../public/product4.html");
        }
        else {
            std::cout << "Unable to continue to page" << std::endl; 
            return readFile("../public/index.html");
        }
      });

    CROW_ROUTE(app, "/confirm") ([]() {
        return readFile("../public/confirm.html");
    });

    //add item to cart.txt
    CROW_ROUTE(app, "/add/<int>/<string>") ([](int id, std::string quantity) {
        std::ofstream file("../cart.txt", std::ios::app);

        if (!file) {
            std::cout << "Error opening cart.txt" << std::endl;
            return crow::response(500, "Internal Server Error");
        }

       file << "Product: " << id << " Quantity: " << quantity << std::endl;
       crow::response res;
       res.code = 303;
       res.set_header("Location", "/confirm");

       return res;  
    });

    //displaying cart items
    CROW_ROUTE(app, "/cart")([]() {
        std::ifstream file("../cart.txt");
        std::string page = readFile("../public/cart.html");
        std::string cartContent;

        if (!file.is_open()) {
            cartContent = "<p>Cart file not found.</p>";
        }else{
            std::string line;
            bool empty = true;

            while (std::getline(file, line)) {
                empty = false;
                cartContent += "<li>" + line + "</li>";
            }

            if (empty) {
                cartContent = "<p>Your cart is empty.</p>";
            }
        }
        std::string mark = "<!-- CART ITEMS -->";

        size_t pos = page.find(mark);
        if (pos != std::string::npos) {
            page.replace(pos, mark.length(), cartContent);
        }

        return crow::response(page);
    });

    CROW_ROUTE(app, "/checkout") ([]() {
        crow::response res;
        res.code = 401;
        res.body = readFile("../public/checkout.html");
        return res;
    });

    CROW_ROUTE(app, "/login") ([](const crow::request& req) {

        const char* user = req.url_params.get("user");
        const char* pass = req.url_params.get("pass");

        if (user && pass && std::string(user) == "ILikeBoxes" && std::string(pass) == "Boxer") {

            crow::response res;
            res.code = 202;
            res.set_header("Content-Type", "text/html");
            res.body = readFile("../public/redirect.html");
            return res;
        }
        crow::response res;
        res.code = 303;
        res.set_header("Location", "/unauthorized");
        return res;
    });

    //purchased items after login
    CROW_ROUTE(app, "/purchased")([]() {

        std::ifstream file("../cart.txt");
        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string page = readFile("../public/purchased.html");

        std::string mark = "<!-- PURCHASED -->";

        size_t pos = page.find(mark);
        if (pos != std::string::npos) {
            page.replace(pos, mark.length(), "<pre>" + buffer.str() + "</pre>");
        }

        return crow::response(page);
    });

    CROW_ROUTE(app, "/unauthorized")([]() {
        crow::response res;
        res.code = 401;
        res.body = readFile("../public/Unauthorized.html");
        return res;
    });


    app.port(23500).multithreaded().run();
    return 1;
}
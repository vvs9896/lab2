#include <iostream>
#include "crow_all.h"
#include <vector>
#include "json.hpp"
#include <cmath>
#include <list>

std::vector<int> matrix1_demo;
std::vector<int> matrix2_demo;
int got_data = 0;

template<typename T>
std::list<T> vectorToList(const std::vector<T>& vec) {
    return std::list<T>(vec.begin(), vec.end());
}

std::list<int> matrixMultiply(const std::list<int>& matrix1, const std::list<int>& matrix2, int size) {
    std::list<int> result(size * size, 0);

    auto it = result.begin();

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            auto it1 = matrix1.begin();
            std::advance(it1, i * size);

            auto it2 = matrix2.begin();
            std::advance(it2, j);

            for (int k = 0; k < size; ++k) {
                *it += *it1 * *it2;
                std::advance(it1, 1);
                std::advance(it2, size);
            }

            ++it;
        }
    }

    return result;
}

bool isPerfectSquare(int num) {
    int squareRoot = std::sqrt(num);
    return squareRoot * squareRoot == num;
}

int handleProducerMessage(int num, std::vector<int>& buf) {
    if (num == -1) {
        return 1;
    } else {
        buf.push_back(num);
        return 0;
    }
}

std::pair<int, int> getData(const std::string& requestContent) {
    try {
        nlohmann::json json = nlohmann::json::parse(requestContent);

        int messageType = json["message_type"];
        int messageContent = json["message_content"];

        return std::pair<int, int>(messageType, messageContent);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return std::pair<int, int>(-1, 0);
    }
}

int main() {
    crow::SimpleApp app;
    crow::logger::setLogLevel(crow::LogLevel::Warning);

    CROW_ROUTE(app, "/").methods("POST"_method)(
            [&](const crow::request& req, crow::response& res) {

                auto tmp = getData(req.body);
                int prod_type = tmp.first;
                int prod_data = tmp.second;

                if (prod_type == 1) {
                    matrix1_demo.push_back(prod_data);
                } else if (prod_type == 2) {
                    matrix2_demo.push_back(prod_data);
                } else {
                    std::cerr << "Unknown producer type" << std::endl;
                }

                std::string responseContent = "Received: " + std::to_string(prod_data);
                res.code = 200;
                res.set_header("Content-Type", "text/plain");
                res.body = responseContent;
                res.end();
            }
    );

    CROW_ROUTE(app, "/end").methods("POST"_method)(
            [&](const crow::request& req, crow::response& res) {
                got_data += 1;

                if (got_data == 2) {
                    if (isPerfectSquare(matrix1_demo.size()) && isPerfectSquare(matrix2_demo.size())) {
                        int size = std::sqrt(matrix1_demo.size());
                        std::list<int> result = matrixMultiply(vectorToList(matrix1_demo), vectorToList(matrix2_demo), size);
                        std::cout << "Matrix multiplication completed." << std::endl;
                    }

                    res.code = 200;
                    res.body = "All data received. Computation complete.";
                    res.end();
                } else {
                    res.code = 200;
                    res.body = "Waiting for more data.";
                    res.end();
                }
            }
    );


    std::cout << "Starting server..." << std::endl;

    app.port(8080).multithreaded().run();
}
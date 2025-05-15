//
// Created by christou on 5/15/25.
//
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <random>
#include <iomanip>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono;

// Global counters for statistics
std::atomic<int> success_count{0};
std::atomic<int> error_count{0};
std::atomic<int> total_requests{0};
std::atomic<long long> total_response_time{0};
std::atomic<int> status_200{0};
std::atomic<int> status_201{0};
std::atomic<int> status_204{0};
std::atomic<int> status_400{0};
std::atomic<int> status_404{0};
std::atomic<int> status_405{0};

// Configuration
const std::string BASE_URL = "http://localhost:8080";
const int THREADS = 50;  // Number of concurrent threads
const int REQUESTS_PER_THREAD = 200;  // Requests per thread (50 * 200 = 10,000 total)
const int DURATION_SECONDS = 10;  // Duration of the test

// Response data structure
struct Response {
    int status_code;
    std::string body;
    long response_time_ms;
};

// Callback for CURL write function
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), total_size);
    return total_size;
}

// Perform HTTP request
Response perform_request(const std::string& method, const std::string& endpoint, const std::string& body = "") {
    Response response;
    CURL* curl = curl_easy_init();
    std::string response_string;

    if (!curl) {
        response.status_code = -1;
        response.body = "Failed to initialize CURL";
        return response;
    }

    auto start_time = high_resolution_clock::now();

    // Set common options
    curl_easy_setopt(curl, CURLOPT_URL, (BASE_URL + endpoint).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // Set headers
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set HTTP method
    if (method == "GET") {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    } else if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    } else if (method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    } else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else if (method == "OPTIONS") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
    }

    // Perform request
    CURLcode res = curl_easy_perform(curl);
    auto end_time = high_resolution_clock::now();

    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
        response.body = response_string;
    } else {
        response.status_code = -1;
        response.body = curl_easy_strerror(res);
    }

    response.response_time_ms = duration_cast<milliseconds>(end_time - start_time).count();

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

// Worker thread function
void worker_thread(int thread_id, bool* should_stop) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> method_dist(0, 4);
    std::uniform_int_distribution<> id_dist(1, 10);
    std::uniform_int_distribution<> book_dist(0, 4);

    std::vector<std::string> sample_books = {
        R"({"title": "The Great Gatsby", "author": "F. Scott Fitzgerald"})",
        R"({"title": "To Kill a Mockingbird", "author": "Harper Lee"})",
        R"({"title": "1984", "author": "George Orwell"})",
        R"({"title": "The Catcher in the Rye", "author": "J.D. Salinger"})",
        R"({"title": "Brave New World", "author": "Aldous Huxley"})"
    };

    while (!(*should_stop)) {
        int method_type = method_dist(gen);
        Response res;

        switch (method_type) {
            case 0:  // GET all books
                res = perform_request("GET", "/books");
                break;
            case 1:  // GET single book
                res = perform_request("GET", "/books/" + std::to_string(id_dist(gen)));
                break;
            case 2:  // POST new book
                res = perform_request("POST", "/books", sample_books[book_dist(gen)]);
                break;
            case 3:  // PUT update book
                res = perform_request("PUT", "/books/" + std::to_string(id_dist(gen)),
                                    sample_books[book_dist(gen)]);
                break;
            case 4:  // DELETE book
                res = perform_request("DELETE", "/books/" + std::to_string(id_dist(gen)));
                break;
        }

        // Update statistics
        total_requests++;
        if (res.status_code > 0) {
            success_count++;
            total_response_time += res.response_time_ms;

            switch (res.status_code) {
                case 200: status_200++; break;
                case 201: status_201++; break;
                case 204: status_204++; break;
                case 400: status_400++; break;
                case 404: status_404++; break;
                case 405: status_405++; break;
            }
        } else {
            error_count++;
        }
    }
}

// Test CORS preflight
void test_cors() {
    std::cout << "\n=== Testing CORS (OPTIONS) ===" << std::endl;

    Response res = perform_request("OPTIONS", "/books");
    std::cout << "OPTIONS /books - Status: " << res.status_code
              << " Time: " << res.response_time_ms << "ms" << std::endl;

    res = perform_request("OPTIONS", "/books/1");
    std::cout << "OPTIONS /books/1 - Status: " << res.status_code
              << " Time: " << res.response_time_ms << "ms" << std::endl;
}

// Main test function
void run_load_test() {
    std::cout << "=== Book API Load Test ===" << std::endl;
    std::cout << "Threads: " << THREADS << std::endl;
    std::cout << "Durée: " << DURATION_SECONDS << " secondes" << std::endl;
    std::cout << "Début du test..." << std::endl;

    std::vector<std::thread> threads;
    bool should_stop = false;

    auto start_time = high_resolution_clock::now();

    // Start worker threads
    for (int i = 0; i < THREADS; i++) {
        threads.emplace_back(worker_thread, i, &should_stop);
    }

    // Run for specified duration
    std::this_thread::sleep_for(seconds(DURATION_SECONDS));

    // Stop all threads
    should_stop = true;
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end_time - start_time).count();

    // Calculate and display results
    std::cout << "\n=== Test Results ===" << std::endl;
    std::cout << "Durée totale: " << duration << " secondes" << std::endl;
    std::cout << "Requetes totale: " << total_requests.load() << std::endl;
    std::cout << "Requestes reussis: " << success_count.load() << std::endl;
    std::cout << "Requestes echoué: " << error_count.load() << std::endl;
    std::cout << "Requestes par seconde: " << total_requests.load() / duration << std::endl;

    if (success_count > 0) {
        std::cout << "Moyenne de response temps: "
                  << total_response_time.load() / success_count.load() << "ms" << std::endl;
    }

    std::cout << "\n=== Distribution de code de status ===" << std::endl;
    std::cout << "200 OK: " << status_200.load() << std::endl;
    std::cout << "201 Création: " << status_201.load() << std::endl;
    std::cout << "204 vide: " << status_204.load() << std::endl;
    std::cout << "400 Mauvaise Requetes: " << status_400.load() << std::endl;
    std::cout << "404 NOn trouvée: " << status_404.load() << std::endl;
    std::cout << "405 Methode Non Authorisé: " << status_405.load() << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize curl globally
    curl_global_init(CURL_GLOBAL_ALL);

    // Test CORS first
    test_cors();

    // Run load test
    run_load_test();

    // Cleanup
    curl_global_cleanup();

    return 0;
}
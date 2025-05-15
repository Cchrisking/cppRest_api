//
// Created by christou on 5/15/25.
//

#include "test_book.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono;

// Test configuration
struct TestConfig {
    int target_rps = 1000;           // Target requests per second
    int duration_seconds = 10;       // Test duration
    int num_threads = 100;           // Number of worker threads
    std::string base_url = "http://localhost:8080";
    bool verbose = false;
};

// Statistics
struct Statistics {
    std::atomic<long> total_requests{0};
    std::atomic<long> successful_requests{0};
    std::atomic<long> failed_requests{0};
    std::atomic<long long> total_latency_ms{0};
    std::atomic<long> min_latency_ms{LONG_MAX};
    std::atomic<long> max_latency_ms{0};

    std::mutex mutex;
    std::map<int, long> status_codes;
    std::vector<long> latencies;
};

class BookAPITester {
private:
    TestConfig config;
    Statistics stats;
    std::atomic<bool> running{false};
    std::mutex rate_limit_mutex;
    std::condition_variable rate_limit_cv;
    std::atomic<int> requests_this_second{0};
    high_resolution_clock::time_point second_start;

    static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    void perform_request(const std::string& method, const std::string& endpoint,
                        const std::string& body = "") {
        // Rate limiting
        {
            std::unique_lock<std::mutex> lock(rate_limit_mutex);
            auto now = high_resolution_clock::now();
            auto elapsed = duration_cast<seconds>(now - second_start).count();

            if (elapsed >= 1) {
                requests_this_second = 0;
                second_start = now;
            }

            if (requests_this_second >= config.target_rps) {
                rate_limit_cv.wait_until(lock, second_start + seconds(1));
                requests_this_second = 0;
                second_start = high_resolution_clock::now();
            }

            requests_this_second++;
        }

        CURL* curl = curl_easy_init();
        if (!curl) return;

        std::string response_string;
        auto start = high_resolution_clock::now();

        // Setup CURL
        curl_easy_setopt(curl, CURLOPT_URL, (config.base_url + endpoint).c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set method
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
        }

        // Perform request
        CURLcode res = curl_easy_perform(curl);
        auto end = high_resolution_clock::now();
        auto latency = duration_cast<milliseconds>(end - start).count();

        stats.total_requests++;

        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            stats.successful_requests++;
            stats.total_latency_ms += latency;

            // Update min/max latency
            long current_min = stats.min_latency_ms.load();
            while (current_min > latency &&
                   !stats.min_latency_ms.compare_exchange_weak(current_min, latency));

            long current_max = stats.max_latency_ms.load();
            while (current_max < latency &&
                   !stats.max_latency_ms.compare_exchange_weak(current_max, latency));

            // Store latency for percentile calculation
            {
                std::lock_guard<std::mutex> lock(stats.mutex);
                stats.latencies.push_back(latency);
                stats.status_codes[response_code]++;
            }

            if (config.verbose) {
                std::cout << method << " " << endpoint << " - "
                         << response_code << " (" << latency << "ms)" << std::endl;
            }
        } else {
            stats.failed_requests++;
            if (config.verbose) {
                std::cout << method << " " << endpoint << " - FAILED: "
                         << curl_easy_strerror(res) << std::endl;
            }
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    void worker_thread() {
        std::vector<std::string> endpoints = {
            "/books",
            "/books/1",
            "/books/2",
            "/books/3"
        };

        std::vector<std::pair<std::string, std::string>> operations = {
            {"GET", "/books"},
            {"GET", "/books/1"},
            {"POST", "/books"},
            {"PUT", "/books/1"},
            {"DELETE", "/books/3"}
        };

        std::string book_json = R"({"title": "Test Book", "author": "Test Author"})";

        int op_index = 0;
        while (running) {
            auto& op = operations[op_index % operations.size()];
            std::string body = (op.first == "POST" || op.first == "PUT") ? book_json : "";

            perform_request(op.first, op.second, body);
            op_index++;
        }
    }

public:
    BookAPITester(const TestConfig& cfg) : config(cfg) {}

    void run() {
        std::cout << "Starting Book API Load Test" << std::endl;
        std::cout << "Target RPS: " << config.target_rps << std::endl;
        std::cout << "Duration: " << config.duration_seconds << " seconds" << std::endl;
        std::cout << "Threads: " << config.num_threads << std::endl;
        std::cout << "URL: " << config.base_url << std::endl;
        std::cout << std::endl;

        curl_global_init(CURL_GLOBAL_ALL);

        running = true;
        second_start = high_resolution_clock::now();
        auto test_start = high_resolution_clock::now();

        // Start worker threads
        std::vector<std::thread> threads;
        for (int i = 0; i < config.num_threads; i++) {
            threads.emplace_back(&BookAPITester::worker_thread, this);
        }

        // Monitor progress
        auto last_report = test_start;
        while (duration_cast<seconds>(high_resolution_clock::now() - test_start).count()
               < config.duration_seconds) {
            std::this_thread::sleep_for(milliseconds(100));

            auto now = high_resolution_clock::now();
            if (duration_cast<seconds>(now - last_report).count() >= 1) {
                auto elapsed = duration_cast<seconds>(now - test_start).count();
                auto current_rps = stats.total_requests.load() / (elapsed ? elapsed : 1);
                std::cout << "\rElapsed: " << elapsed << "s | "
                         << "Total: " << stats.total_requests << " | "
                         << "RPS: " << current_rps << " | "
                         << "Success: " << stats.successful_requests << " | "
                         << "Failed: " << stats.failed_requests
                         << std::flush;
                last_report = now;
            }
        }

        running = false;

        // Wait for threads to finish
        for (auto& t : threads) {
            t.join();
        }

        report_results();
        curl_global_cleanup();
    }

    void report_results() {
        std::cout << "\n\n=== Final Results ===" << std::endl;
        std::cout << "Total Requests: " << stats.total_requests << std::endl;
        std::cout << "Successful: " << stats.successful_requests << std::endl;
        std::cout << "Failed: " << stats.failed_requests << std::endl;

        if (stats.successful_requests > 0) {
            auto avg_latency = stats.total_latency_ms.load() / stats.successful_requests.load();
            std::cout << "\nLatency Statistics:" << std::endl;
            std::cout << "  Average: " << avg_latency << "ms" << std::endl;
            std::cout << "  Min: " << stats.min_latency_ms << "ms" << std::endl;
            std::cout << "  Max: " << stats.max_latency_ms << "ms" << std::endl;

            // Calculate percentiles
            std::lock_guard<std::mutex> lock(stats.mutex);
            if (!stats.latencies.empty()) {
                std::sort(stats.latencies.begin(), stats.latencies.end());
                auto p50 = stats.latencies[stats.latencies.size() * 0.5];
                auto p90 = stats.latencies[stats.latencies.size() * 0.9];
                auto p99 = stats.latencies[stats.latencies.size() * 0.99];

                std::cout << "  P50: " << p50 << "ms" << std::endl;
                std::cout << "  P90: " << p90 << "ms" << std::endl;
                std::cout << "  P99: " << p99 << "ms" << std::endl;
            }

            std::cout << "\nStatus Code Distribution:" << std::endl;
            for (const auto& [code, count] : stats.status_codes) {
                std::cout << "  " << code << ": " << count << std::endl;
            }
        }

        auto actual_rps = stats.total_requests.load() / config.duration_seconds;
        std::cout << "\nActual RPS: " << actual_rps << std::endl;
    }
};

int main(int argc, char* argv[]) {
    TestConfig config;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--rps" && i + 1 < argc) {
            config.target_rps = std::stoi(argv[++i]);
        } else if (arg == "--duration" && i + 1 < argc) {
            config.duration_seconds = std::stoi(argv[++i]);
        } else if (arg == "--threads" && i + 1 < argc) {
            config.num_threads = std::stoi(argv[++i]);
        } else if (arg == "--url" && i + 1 < argc) {
            config.base_url = argv[++i];
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --rps <n>      Target requests per second (default: 1000)" << std::endl;
            std::cout << "  --duration <n> Test duration in seconds (default: 10)" << std::endl;
            std::cout << "  --threads <n>  Number of worker threads (default: 100)" << std::endl;
            std::cout << "  --url <url>    Base URL (default: http://localhost:8080)" << std::endl;
            std::cout << "  --verbose      Enable verbose output" << std::endl;
            return 0;
        }
    }

    BookAPITester tester(config);
    tester.run();

    return 0;
}
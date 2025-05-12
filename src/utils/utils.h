// Create a new file called Utils.h
#pragma once
#include <string>
#include <regex>

namespace Utils {
    bool is_valid_email(const std::string& email) {
        const std::regex pattern(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
        return std::regex_match(email, pattern);
    }

    std::string sanitize_input(const std::string& input) {
        std::string safe = input;
        // Remove HTML tags
        safe = std::regex_replace(safe, std::regex("<.*?>"), "");
        // Additional sanitization if needed
        return safe;
    }
}
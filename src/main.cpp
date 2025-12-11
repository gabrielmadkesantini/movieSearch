
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

#include "context.hpp"
#include "data_loader.hpp"
#include "queries.hpp"

/* ------------------------------------------------------------------
   Helpers
------------------------------------------------------------------ */

// Remove espaços nas pontas
namespace {
std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    if (start == s.size()) return "";

    std::size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}
}

// Parse de tags com suporte a aspas simples:
// tags 'dark hero' drama  -> ["dark hero", "drama"]
static std::vector<std::string> parseTagsLine(const std::string& line) {
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;

    for (char c : line) {
        if (c == '\'') {
            inQuotes = !inQuotes;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)) && !inQuotes) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }

    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

/* ------------------------------------------------------------------
   MAIN
------------------------------------------------------------------ */

int main() {

    DataContext ctx;

    std::cerr << "Loading movies..." << std::endl;
    data_loader::loadMovies("data/movies.csv", ctx);

    std::cerr << "Loading ratings..." << std::endl;
    data_loader::loadRatings("data/ratings.csv", ctx);

    std::cerr << "Loading tags..." << std::endl;
    data_loader::loadTags("data/tags.csv", ctx);

    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;

        if (!(iss >> cmd)) continue;

        // ---------------- PREFIX ----------------
        if (cmd == "prefix") {
            std::string rest;
            std::getline(iss, rest);
            std::string prefix = trim(rest);

            if (!prefix.empty()) {
                queries::queryPrefix(ctx, prefix);
            }
        }

        // ---------------- USER ----------------
        else if (cmd == "user") {
            std::string userToken;

            if (!(iss >> userToken)) continue;

            try {
                int userId = std::stoi(userToken);
                queries::queryUser(ctx, userId);
            }
            catch (...) {
                std::cerr << "Invalid user id\n";
            }
        }

        // ---------------- TOP ----------------
        else if (cmd == "top") {
            std::string nToken;

            if (!(iss >> nToken)) continue;

            int n = 0;
            try {
                n = std::stoi(nToken);
            }
            catch (...) {
                continue;
            }

            std::string rest;
            std::getline(iss, rest);
            std::string genre = trim(rest);

            if (!genre.empty() && n > 0) {
                queries::queryTop(ctx, n, genre);
            }
        }

        // ---------------- TAGS ----------------
        else if (cmd == "tags") {

            std::string rest;
            std::getline(iss, rest);

            auto tags = parseTagsLine(rest);

            if (!tags.empty()) {
                queries::queryTags(ctx, tags);
            }
        }

        // ---------------- UNKNOWN ----------------
        else {
            std::cerr << "Unknown command\n";
        }
    }

    return 0;
}

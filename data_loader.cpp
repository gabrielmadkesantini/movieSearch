#include "data_loader.hpp"

/*
RESPONSABILIDADE DESTE ARQUIVO

- Ler os arquivos CSV:
    - movies.csv
    - ratings.csv
    - tags.csv

- Alinhar estes dados com:
    - MovieHashTable
    - UserHashTable
    - TagHashTable
    - TitleTrie

DEVE IMPLEMENTAR:

1. loadMovies(path, ctx)
   - Abrir CSV
   - Ignorar cabeçalho
   - Para cada linha:
        parse movieId, title, genres
        extrair ano do título (se existir)
        inserir movie no ctx.movies
        inserir título no ctx.trie

2. loadRatings(path, ctx)
   - Abrir CSV
   - Ignorar cabeçalho
   - Para cada linha:
        parse userId, movieId, rating
        encontrar Movie em ctx.movies
        atualizar:
            movie.ratingCount++
            movie.ratingSum += rating
        inserir rating na estrutura de ctx.users

3. loadTags(path, ctx)
   - Abrir CSV
   - Ignorar cabeçalho
   - Para cada linha:
        parse movieId e tag
        normalizar tag (lowercase/trim)
        chamar ctx.tags.addMovie(tag, movieId)
*/



#include "data_loader.hpp"

#include <fstream>
#include <sstream>
#include <cctype>

namespace {

// Trim spaces from both ends
std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    if (start == s.size()) return "";

    std::size_t end = s.size() - 1;
    while (end > start && std::isspace(static_cast<unsigned char>(s[end]))) {
        --end;
    }
    return s.substr(start, end - start + 1);
}

// To lowercase
std::string toLower(const std::string& s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

// Extract year from title like "Movie Name (1995)". Returns 0 if not found.
int extractYear(const std::string& title) {
    if (title.size() < 6) return 0;

    std::size_t close = title.rfind(')');
    if (close == std::string::npos || close < 5) return 0;

    std::size_t open = title.rfind('(', close);
    if (open == std::string::npos || close - open != 5) return 0;

    int year = 0;
    for (std::size_t i = open + 1; i < close; ++i) {
        if (!std::isdigit(static_cast<unsigned char>(title[i]))) {
            return 0;
        }
        year = year * 10 + (title[i] - '0');
    }
    return year;
}

} // namespace

namespace data_loader {

void loadMovies(const std::string& path, DataContext& ctx) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    // Discard header
    if (!std::getline(file, line)) {
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // movieId is before first comma
        std::size_t firstComma = line.find(',');
        if (firstComma == std::string::npos) continue;

        std::string movieIdStr = line.substr(0, firstComma);
        std::string rest = line.substr(firstComma + 1);

        int movieId = 0;
        try {
            movieId = std::stoi(movieIdStr);
        } catch (...) {
            continue;
        }

        std::string title;
        std::string genres;

        if (!rest.empty() && rest[0] == '"') {
            // Title is quoted
            std::size_t endQuote = rest.find('"', 1);
            if (endQuote == std::string::npos) {
                // Malformed line
                continue;
            }
            title = rest.substr(1, endQuote - 1);
            std::size_t afterTitlePos = endQuote + 1;
            if (afterTitlePos < rest.size() && rest[afterTitlePos] == ',') {
                ++afterTitlePos;
            }
            if (afterTitlePos <= rest.size()) {
                genres = rest.substr(afterTitlePos);
            }
        } else {
            // Title not quoted: goes until next comma
            std::size_t secondComma = rest.find(',');
            if (secondComma == std::string::npos) {
                // No genres field
                title = rest;
                genres.clear();
            } else {
                title = rest.substr(0, secondComma);
                genres = rest.substr(secondComma + 1);
            }
        }

        title = trim(title);
        genres = trim(genres);
        int year = extractYear(title);

        Movie& m = ctx.movies.insertOrGet(movieId);
        m.movieId = movieId;
        m.title = title;
        m.genres = genres;
        m.year = year;

        ctx.trie.insert(title, movieId);
    }
}

void loadRatings(const std::string& path, DataContext& ctx) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    // Discard header
    if (!std::getline(file, line)) {
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string userIdStr, movieIdStr, ratingStr;

        if (!std::getline(ss, userIdStr, ',')) continue;
        if (!std::getline(ss, movieIdStr, ',')) continue;
        if (!std::getline(ss, ratingStr, ',')) continue;
        // Remaining fields (timestamp etc.) can be ignored

        int userId = 0;
        int movieId = 0;
        float rating = 0.0f;

        try {
            userId = std::stoi(userIdStr);
            movieId = std::stoi(movieIdStr);
            rating = std::stof(ratingStr);
        } catch (...) {
            continue;
        }

        Movie& m = ctx.movies.insertOrGet(movieId);
        m.ratingCount += 1;
        m.ratingSum += static_cast<double>(rating);

        User& u = ctx.users.insertOrGet(userId);
        u.userId = userId;
        u.ratings.push_back(UserRating{movieId, rating});
    }
}

void loadTags(const std::string& path, DataContext& ctx) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    // Discard header
    if (!std::getline(file, line)) {
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string userIdStr, movieIdStr, tagStr, timestampStr;

        if (!std::getline(ss, userIdStr, ',')) continue;
        if (!std::getline(ss, movieIdStr, ',')) continue;
        if (!std::getline(ss, tagStr, ',')) continue;
        // timestamp can be ignored; may or may not be present
        std::getline(ss, timestampStr);

        int movieId = 0;
        try {
            movieId = std::stoi(movieIdStr);
        } catch (...) {
            continue;
        }

        std::string normalizedTag = toLower(trim(tagStr));
        if (normalizedTag.empty()) continue;

        ctx.tags.addMovie(normalizedTag, movieId);
    }
}

} // namespace data_loader

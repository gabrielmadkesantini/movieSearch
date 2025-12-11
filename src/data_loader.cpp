#include "data_loader.hpp"




#include <fstream>
#include <sstream>
#include <cctype>

namespace {

// Funcao auxiliar para remover o espaço em branco da palavra
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

// Deixa em minusculo a string
std::string toLower(const std::string& s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

// Pega o ano do filme quando estiver num formato como: "Movie Name (1995)". Se não achar retorna 0.
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
//adiciona os filmes na tabela hash e insere os titulos na trie
void loadMovies(const std::string& path, DataContext& ctx) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    // Ignora a header do csv
    if (!std::getline(file, line)) {
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // O id fo filme esta antes da vírgula
        std::size_t firstComma = line.find(',');
        if (firstComma == std::string::npos) continue;

        std::string movieIdStr = line.substr(0, firstComma);
        std::string rest = line.substr(firstComma + 1);

        int movieId = 0;
        try {
            //converte de string pra inteiro
            movieId = std::stoi(movieIdStr);
        } catch (...) {
            continue;
        }

        std::string title;
        std::string genres;

        if (!rest.empty() && rest[0] == '"') {
            // Verifica se o titulo contém aspas
            std::size_t endQuote = rest.find('"', 1);
            if (endQuote == std::string::npos) {
                // npos --> posição invalida
                continue;
            }
            title = rest.substr(1, endQuote - 1);
            //verifica se fecha aspas, as ignora e vai para a proxima virgula
            std::size_t afterTitlePos = endQuote + 1;
            if (afterTitlePos < rest.size() && rest[afterTitlePos] == ',') {
                ++afterTitlePos;
            }
            if (afterTitlePos <= rest.size()) {
                genres = rest.substr(afterTitlePos);
            }
        } else {
            // Caso de titulo sem aspas
            std::size_t secondComma = rest.find(',');
            if (secondComma == std::string::npos) {
                // Pega os generos dos filmes
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

        // Insere o filme na tabela hash de filmes referenciando uma instânia da classe Movie e
        //chamando a função que adiciona ou retorna o filme
        Movie& m = ctx.movies.insertOrGet(movieId);
        m.movieId = movieId;
        m.title = title;
        m.genres = genres;
        m.year = year;

        // Insere o título na trie para buscas por prefixo
        ctx.trie.insert(title, movieId);
    }
}

void loadRatings(const std::string& path, DataContext& ctx) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    if (!std::getline(file, line)) {
        return;
    }
    
    //percorre o arquivo linha por linha
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string userIdStr, movieIdStr, ratingStr;

        if (!std::getline(ss, userIdStr, ',')) continue;
        if (!std::getline(ss, movieIdStr, ',')) continue;
        if (!std::getline(ss, ratingStr, ',')) continue;
        // Ignora demais campos que podem ser ignorados, como timestamp, por exemplo

        int userId = 0;
        int movieId = 0;
        float rating = 0.0f;

        //faz o parse para int dos dados lidos
        try {
            userId = std::stoi(userIdStr);
            movieId = std::stoi(movieIdStr);
            rating = std::stof(ratingStr);
        } catch (...) {
            continue;
        }

        //para cada filme achado (vai apenas executar o "get" do insertOrGet, pois os filmes ja foram inseridos no loadMovies)
        //atualiza a contagem de ratings e a soma dos ratings, nao cria uma nova tabela, apenas atualiza os valores
        Movie& m = ctx.movies.insertOrGet(movieId);
        m.ratingCount += 1;
        m.ratingSum += static_cast<double>(rating);

        //insere a avaliação do usuário na tabela hash de usuários (agora sim, cria uma tabela para registro de cada usuario e suas avaliações)
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
    // Ignora a header do csv
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
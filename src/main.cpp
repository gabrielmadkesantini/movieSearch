#include <iostream>
#include <sstream>
#include "context.hpp"
#include "data_loader.hpp"
#include "queries.hpp"

/*
RESPONSABILIDADE DESTE ARQUIVO

- Inicializar o sistema
- Carregar todos os CSV
- Abrir o loop de comandos
- Direcionar cada comando à consulta correta

DEVE IMPLEMENTAR:

1. Criar DataContext.

2. Chamar:
    data_loader::loadMovies
    data_loader::loadRatings
    data_loader::loadTags

3. Loop de leitura:
    while(getline(cin, line))

4. Parse de comandos:

    - prefix <texto>
        -> queries::queryPrefix

    - user <userId>
        -> queries::queryUser

    - top <N> <genre>
        -> queries::queryTop

    - tags <t1> <t2> ...
        -> queries::queryTags

5. Tratar erros:
    - comando inválido
    - entrada incompleta
*/


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "context.hpp"
#include "data_loader.hpp"
#include "queries.hpp"

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
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) {
            continue;
        }

        if (cmd == "prefix") {
            std::string rest;
            std::getline(iss, rest);
            std::string prefix = trim(rest);
            if (!prefix.empty()) {
                queries::queryPrefix(ctx, prefix);
            }
        } else if (cmd == "user") {
            std::string userToken;
            if (!(iss >> userToken)) {
                continue;
            }
            try {
                int userId = std::stoi(userToken);
                queries::queryUser(ctx, userId);
            } catch (...) {
                continue;
            }
        } else if (cmd == "top") {
            std::string nToken;
            if (!(iss >> nToken)) {
                continue;
            }
            int n = 0;
            try {
                n = std::stoi(nToken);
            } catch (...) {
                continue;
            }
            std::string rest;
            std::getline(iss, rest);
            std::string genre = trim(rest);
            if (!genre.empty() && n > 0) {
                queries::queryTop(ctx, n, genre);
            }
        } else if (cmd == "tags") {
            std::vector<std::string> tags;
            std::string tag;
            while (iss >> tag) {
                tags.push_back(tag);
            }
            if (!tags.empty()) {
                queries::queryTags(ctx, tags);
            }
        } else {
            std::cerr << "Unknown command" << std::endl;
        }
    }

    return 0;
}

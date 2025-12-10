#include "queries.hpp"

/*
RESPONSABILIDADE DESTE ARQUIVO

- Implementar todas as consultas exigidas.

DEVE IMPLEMENTAR:

1. queryPrefix(ctx, prefix)
   - Obter lista de movieIds via TRIE:
        ctx.trie.searchPrefix(prefix)
   - Para cada movieId:
        buscar Movie em ctx.movies
        compor estrutura temporária com:
            título, gêneros, média global, ratingCount
   - Ordenar por média global (desc)
   - Imprimir conforme padrão

------------------------------------------------------

2. queryUser(ctx, userId)
   - Buscar User em ctx.users
   - Para cada avaliação do usuário:
        buscar Movie
        montar estrutura com:
            userRating, globalAvg, ratingCount
   - Ordenar por:
        1ª nota do usuário (desc)
        2ª média global (desc)
   - Limitar 20 resultados
   - Imprimir

------------------------------------------------------

3. queryTop(ctx, N, genre)
   - Percorrer todos os filmes
   - Selecionar os que:
        contêm 'genre'
        ratingCount >= 1000
   - Calcular média global
   - Ordenar por média global (desc)
   - Imprimir top N

------------------------------------------------------

4. queryTags(ctx, tags[])
   - Para cada tag:
        obter lista de movieIds da TagHashTable
   - Fazer interseção entre as listas
   - Para cada movieId na interseção:
        buscar Movie e calcular média
   - Ordenar por média global (desc)
   - Imprimir
*/


#include "queries.hpp"
#include "sort_utils.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace {

// Helpers for tag normalization
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

    std::string toLower(const std::string& s) {
        std::string result;
        result.reserve(s.size());
        for (char c : s) {
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
        return result;
    }

    // Normaliza a tag vinda do comando `tags`
    std::string normalizeTag(const std::string& raw) {
        std::string tag = trim(raw);

        // se vier com aspas simples nas pontas, remove
        if (tag.size() >= 2 && tag.front() == '\'' && tag.back() == '\'') {
            tag = tag.substr(1, tag.size() - 2);
        }

        tag = trim(tag);
        tag = toLower(tag);
        return tag;
    }

    std::string extractGenres(const std::string& s) {
        auto pos = s.find(',');
        if (pos == std::string::npos) return s;
        return s.substr(0, pos);
    }

    std::string extractYear(const std::string& s) {
        auto pos = s.find(',');
        if (pos == std::string::npos) return "";
        return s.substr(pos + 1);
    }


} // namespace


namespace queries {

void queryPrefix(DataContext& ctx, const std::string& prefix) {
    struct PrefixResult {
        int movieId;
        std::string title;
        std::string genres;
        double avg;
        int ratingCount;
    };

    auto ids = ctx.trie.searchPrefix(prefix);
    std::vector<PrefixResult> results;
    results.reserve(ids.size());

    for (int id : ids) {
        Movie* m = ctx.movies.find(id);
        if (!m) continue;
        if (m->ratingCount <= 0) continue;

        double avg = m->ratingSum / static_cast<double>(m->ratingCount);
        results.push_back(PrefixResult{
            m->movieId,
            m->title,
            m->genres,
            avg,
            m->ratingCount
        });
    }

    if (results.empty()) {
        return;
    }

    sort_utils::quickSort(results, [](const PrefixResult& a, const PrefixResult& b) {
        if (a.avg != b.avg) return a.avg > b.avg;
        if (a.ratingCount != b.ratingCount) return a.ratingCount > b.ratingCount;
        return a.movieId < b.movieId;
    });

    std::cout << std::fixed << std::setprecision(6);

    if (results.empty()) {
        return;
    }

    std::cout
        << std::setw(6)  << "ID"
        << " | " << std::setw(40) << "Title"
        << " | " << std::setw(25) << "Genres"
        << " | " << std::setw(6)  << "Year"
        << " | " << std::setw(10) << "AvgRate"
        << " | " << std::setw(8)  << "count"
        << '\n';

    std::cout << std::string(105, '-') << '\n';


    for (const auto& r : results) {
        std::string genres = extractGenres(r.genres);
        std::string year   = extractYear(r.genres);

        std::cout
            << std::setw(6)  << r.movieId
            << " | " << std::setw(40) << r.title.substr(0,40)
            << " | " << std::setw(25) << genres.substr(0,25)
            << " | " << std::setw(6)  << year
            << " | " << std::setw(10) << r.avg
            << " | " << std::setw(8)  << r.ratingCount
            << '\n';
    }


}

void queryUser(DataContext& ctx, int userId) {
    struct UserResult {
        int movieId;
        std::string title;
        std::string genres;
        float userRating;
        double globalAvg;
        int ratingCount;
    };


    User* user = ctx.users.find(userId);
    if (!user) {
        std::cout << "User not found\n";
        return;
    }

    std::vector<UserResult> results;
    results.reserve(user->ratings.size());

    for (const auto& ur : user->ratings) {
        Movie* m = ctx.movies.find(ur.movieId);
        if (!m) continue;
        if (m->ratingCount <= 0) continue;

        double avg = m->ratingSum / static_cast<double>(m->ratingCount);
        results.push_back(UserResult{
            m->movieId,
            m->title,
            m->genres,
            ur.rating,
            avg,
            m->ratingCount
        });

    }

    if (results.empty()) {
        return;
    }

    sort_utils::quickSort(results, [](const UserResult& a, const UserResult& b) {
        if (a.userRating != b.userRating) return a.userRating > b.userRating;
        if (a.globalAvg != b.globalAvg) return a.globalAvg > b.globalAvg;
        return a.movieId < b.movieId;
    });

    std::cout << std::fixed << std::setprecision(6);

    int limit = static_cast<int>(results.size());
    if (limit > 20) limit = 20;

    if (limit == 0) {
        return;
    }

    // Cabeçalho específico de USER
    std::cout
        << std::setw(6)  << "ID"
        << " | " << std::setw(40) << "Title"
        << " | " << std::setw(25) << "Genres"
        << " | " << std::setw(6)  << "Year"
        << " | " << std::setw(10) << "UserRate"
        << " | " << std::setw(10) << "GlobalAvg"
        << " | " << std::setw(8)  << "count"
        << '\n';

    std::cout << std::string(110, '-') << '\n';

    for (int i = 0; i < limit; ++i) {
        const auto& r = results[i];
        std::string genres = extractGenres(r.genres);
        std::string year   = extractYear(r.genres);

        std::cout
            << std::setw(6)  << r.movieId
            << " | " << std::setw(40) << r.title.substr(0,40)
            << " | " << std::setw(25) << genres.substr(0,25)
            << " | " << std::setw(6)  << year
            << " | " << std::setw(10) << r.userRating
            << " | " << std::setw(10) << r.globalAvg
            << " | " << std::setw(8)  << r.ratingCount
            << '\n';
    }


}

void queryTop(DataContext& ctx, int n, const std::string& genre) {
    struct TopResult {
        int movieId;
        std::string title;
        std::string genres;
        double avg;
        int ratingCount;
    };

    const auto& table = ctx.movies.rawTable();
    std::vector<TopResult> results;
    results.reserve(table.size());

    // Varre todos os filmes na hash
    for (const auto& entry : table) {
        if (!entry.occupied || entry.deleted) continue;

        const Movie& m = entry.value;

        // Requisitos do enunciado
        if (m.ratingCount < 1000) continue;
        if (m.ratingCount <= 0) continue;
        if (!genre.empty() && m.genres.find(genre) == std::string::npos) continue;

        double avg = m.ratingSum / static_cast<double>(m.ratingCount);

        results.push_back(TopResult{
            m.movieId,
            m.title,
            m.genres,
            avg,
            m.ratingCount
        });
    }

    if (results.empty()) {
        return;
    }

    // Ordena por média global desc, depois ratingCount desc, depois movieId asc
    sort_utils::quickSort(results, [](const TopResult& a, const TopResult& b) {
        if (a.avg != b.avg) return a.avg > b.avg;
        if (a.ratingCount != b.ratingCount) return a.ratingCount > b.ratingCount;
        return a.movieId < b.movieId;
    });

    std::cout << std::fixed << std::setprecision(6);

    int limit = static_cast<int>(results.size());
    if (n < limit) limit = n;
    if (limit == 0) {
        return;
    }

    // Cabeçalho
    std::cout
        << std::setw(6)  << "ID"
        << " | " << std::setw(40) << "Title"
        << " | " << std::setw(25) << "Genres"
        << " | " << std::setw(6)  << "Year"
        << " | " << std::setw(10) << "AvgRate"
        << " | " << std::setw(8)  << "Ratings"
        << '\n';

    std::cout << std::string(110, '-') << '\n';

    // Linhas (respeitando o limite N)
    for (int i = 0; i < limit; ++i) {
        const auto& r = results[i];

        std::string genres = extractGenres(r.genres); // antes da vírgula
        std::string year   = extractYear(r.genres);   // depois da vírgula

        std::cout
            << std::setw(6)  << r.movieId
            << " | " << std::setw(40) << r.title.substr(0, 40)
            << " | " << std::setw(25) << genres.substr(0, 25)
            << " | " << std::setw(6)  << year
            << " | " << std::setw(10) << r.avg
            << " | " << std::setw(8)  << r.ratingCount
            << '\n';
    }
}


void queryTags(DataContext& ctx, const std::vector<std::string>& tags) {
    struct TagResult {
        int movieId;
        std::string title;
        std::string genres;
        double avg;
        int ratingCount;
    };

    if (tags.empty()) {
        return;
    }

 // Get movie lists for each tag
    std::vector<std::vector<int>> tagMovieLists;
    tagMovieLists.reserve(tags.size());

    for (const auto& t : tags) {
        std::string norm = normalizeTag(t);
        if (norm.empty()) {
            tagMovieLists.push_back(std::vector<int>{});
            continue;
        }
        tagMovieLists.push_back(ctx.tags.getMovies(norm));
    }


    // If any list is empty, intersection is empty
    for (const auto& lst : tagMovieLists) {
        if (lst.empty()) {
            return;
        }
    }

    // Find index of smallest list
    std::size_t smallestIdx = 0;
    for (std::size_t i = 1; i < tagMovieLists.size(); ++i) {
        if (tagMovieLists[i].size() < tagMovieLists[smallestIdx].size()) {
            smallestIdx = i;
        }
    }

    const auto& baseList = tagMovieLists[smallestIdx];
    std::vector<int> intersection;
    intersection.reserve(baseList.size());

    for (int id : baseList) {
        bool inAll = true;
        for (std::size_t j = 0; j < tagMovieLists.size(); ++j) {
            if (j == smallestIdx) continue;
            const auto& lst = tagMovieLists[j];
            bool found = false;
            for (int otherId : lst) {
                if (otherId == id) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                inAll = false;
                break;
            }
        }
        if (inAll) {
            intersection.push_back(id);
        }
    }

    if (intersection.empty()) {
        return;
    }

    std::vector<TagResult> results;
    results.reserve(intersection.size());

    for (int id : intersection) {
        Movie* m = ctx.movies.find(id);
        if (!m) continue;
        if (m->ratingCount <= 0) continue;

        double avg = m->ratingSum / static_cast<double>(m->ratingCount);
        results.push_back(TagResult{
            m->movieId,
            m->title,
            m->genres,
            avg,
            m->ratingCount
        });
    }

    if (results.empty()) {
        return;
    }

    sort_utils::quickSort(results, [](const TagResult& a, const TagResult& b) {
        if (a.avg != b.avg) return a.avg > b.avg;
        if (a.ratingCount != b.ratingCount) return a.ratingCount > b.ratingCount;
        return a.movieId < b.movieId;
    });

        std::cout << std::fixed << std::setprecision(6);

    std::cout
        << std::setw(6)  << "ID"
        << " | " << std::setw(40) << "Title"
        << " | " << std::setw(25) << "Genres"
        << " | " << std::setw(6)  << "Year"
        << " | " << std::setw(10) << "AvgRate"
        << " | " << std::setw(8)  << "count"
        << '\n';

    std::cout << std::string(105, '-') << '\n';


    for (const auto& r : results) {
        std::string genres = extractGenres(r.genres);
        std::string year   = extractYear(r.genres);

        std::cout
            << std::setw(6)  << r.movieId
            << " | " << std::setw(40) << r.title.substr(0,40)
            << " | " << std::setw(25) << genres.substr(0,25)
            << " | " << std::setw(6)  << year
            << " | " << std::setw(10) << r.avg
            << " | " << std::setw(8)  << r.ratingCount
            << '\n';
    }

}

} // namespace queries

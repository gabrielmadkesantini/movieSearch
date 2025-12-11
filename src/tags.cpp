#include "tags.hpp"
#include <cstddef>
#include <stdexcept>

TagHashTable::TagHashTable(std::size_t capacity) : table(), count(0) {
    std::size_t cap = capacity == 0 ? 1 : capacity;
    table.resize(cap);
}

// Hash de string: combina valores ASCII dos caracteres
std::size_t TagHashTable::hash(const std::string& s) const {
    // multiplicative string hash
    std::size_t h = 0;
    const std::size_t base = 131;
    for (unsigned char c : s) {
        h = h * base + c;
    }
    return h % table.size();
}

// Sondagem linear
std::size_t TagHashTable::probe(std::size_t index, std::size_t step) const {
    return (index + step) % table.size();
}

void TagHashTable::addMovie(const std::string& tag, int movieId) {
    if (table.empty()) return;

    std::size_t h = hash(tag);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        TagHashEntry& entry = table[idx];

        if (entry.occupied) {
            // Slot ocupado: se for a mesma tag (e não marcada como deletada), atualiza a lista de filmes
            if (!entry.deleted && entry.key == tag) {
                bool exists = false;
                for (int id : entry.movieIds) {
                    if (id == movieId) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    entry.movieIds.push_back(movieId);
                }
                return;
            }
            // Caso contrário, continua sondando
        } else {
            // Slot vazio: insere nova tag aqui
            entry.key = tag;
            entry.movieIds.clear();
            entry.movieIds.push_back(movieId);
            entry.occupied = true;
            entry.deleted  = false;
            ++count;
            return;
        }
    }

    // Se chegou aqui, tabela cheia (situação anômala)
    throw std::runtime_error("TagHashTable::addMovie – Hash table full");
}

std::vector<int> TagHashTable::getMovies(const std::string& tag) const {
    if (table.empty()) return {};

    std::size_t h = hash(tag);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        const TagHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            // Slot nunca usado → tag não existe
            return {};
        }

        if (entry.occupied && !entry.deleted && entry.key == tag) {
            return entry.movieIds;
        }
        // Senão continua sondando
    }

    return {};
}

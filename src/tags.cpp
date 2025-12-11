#include "tags.hpp"
#include <cstddef>

TagHashTable::TagHashTable(std::size_t capacity) : table(), count(0) {
    std::size_t cap = capacity == 0 ? 1 : capacity;
    table.resize(cap);
}

std::size_t TagHashTable::hash(const std::string& s) const {
    // Multiplicative string hash
    std::size_t h = 0;
    const std::size_t base = 131;
    for (unsigned char c : s) {
        h = h * base + c;
    }
    return h % table.size();
}

std::size_t TagHashTable::probe(std::size_t index, std::size_t step) const {
    // Linear probing
    return (index + step) % table.size();
}

void TagHashTable::addMovie(const std::string& tag, int movieId) {
    if (table.empty()) return;

    std::size_t h = hash(tag);
    std::size_t firstDeletedIndex = static_cast<std::size_t>(-1);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        TagHashEntry& entry = table[idx];

        if (entry.occupied) {
            //verifica se a tag ja existe na tabela e verifica se o id do filme ja esta associado a essa tag
            if (entry.key == tag) {
                
                bool exists = false;
                //percorre a lista de ids dos filmes associados a essa tag e verifica se ja existe
                for (int id : entry.movieIds) {
                    if (id == movieId) {
                        exists = true;
                        break;
                    }
                }
                //caso nao exisitr adiciona o id do filme na lista
                if (!exists) {
                    //adiciona no final da lista
                    entry.movieIds.push_back(movieId);
                }
                return;
            }
            // esta ocupado com uma tag diferente, continua para o proximo passo
        } else {
                // Slot esta vazio, entao pode inserir a nova tag aqui
                TagHashEntry& insertEntry = table[idx];
                insertEntry.key = tag;
                insertEntry.movieIds.clear();
                insertEntry.movieIds.push_back(movieId);
                insertEntry.occupied = true;
                insertEntry.deleted = false;
                ++count;
                return;
        }
    }
}

std::vector<int> TagHashTable::getMovies(const std::string& tag) const {
    if (table.empty()) return {};

    std::size_t h = hash(tag);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        const TagHashEntry& entry = table[idx];

        if (!entry.occupied) {
            // Tag nao foi encontrada
            return {};
        }

        if (entry.occupied && entry.key == tag) {
            // Tag nao foi encontrada
            return entry.movieIds;
        }
        // Senao cokntinua a busca fazendo p "probe" qeu avança para o proximo indice que pode esta livre ou nao
    }

    return {};
}
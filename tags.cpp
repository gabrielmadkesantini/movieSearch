#include "tags.hpp"

/*
RESPONSABILIDADE DESTE ARQUIVO

- Implementar a classe TagHashTable.
- Controlar associação: tag -> lista de movieIds.
- Hash manual para strings.
- NÃO usar unordered_map ou map.

DEVE IMPLEMENTAR:

1. Construtor TagHashTable(capacity)
   - Inicializar array de entradas
   - Iniciar contador

2. hash(string)
   - Hash simples para string (ex: multiplicativo)

3. probe(index, step)
   - Estratégia de colisão

4. addMovie(tag, movieId)
   - Se a tag existir:
        adicionar movieId à lista se ainda não estiver nela
   - Se não existir:
        criar nova entrada de tag
        inicializar lista com esse movieId

5. getMovies(tag)
   - Retornar lista de filmes associados à tag
   - Retornar vetor vazio se não existir
*/

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
            if (!entry.deleted && entry.key == tag) {
                // Tag already exists: add movieId if not present
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
            // occupied with different key or deleted; keep probing
        } else {
            // Not occupied
            if (entry.deleted) {
                // Remember first deleted slot to reuse if key not found later
                if (firstDeletedIndex == static_cast<std::size_t>(-1)) {
                    firstDeletedIndex = idx;
                }
                // continue probing to ensure tag is not already in table
            } else {
                // Truly empty slot, can insert here
                if (firstDeletedIndex != static_cast<std::size_t>(-1)) {
                    idx = firstDeletedIndex;
                }
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

    // Fallback if table is full: insert/overwrite at index 0 or firstDeletedIndex if available
    std::size_t idx = (firstDeletedIndex != static_cast<std::size_t>(-1))
                      ? firstDeletedIndex
                      : 0;
    TagHashEntry& entry = table[idx];
    if (!entry.occupied || entry.deleted) {
        ++count;
    }
    entry.key = tag;
    entry.movieIds.clear();
    entry.movieIds.push_back(movieId);
    entry.occupied = true;
    entry.deleted = false;
}

std::vector<int> TagHashTable::getMovies(const std::string& tag) const {
    if (table.empty()) return {};

    std::size_t h = hash(tag);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        const TagHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            // Never occupied: tag not found
            return {};
        }

        if (entry.occupied && !entry.deleted && entry.key == tag) {
            return entry.movieIds;
        }
        // Otherwise, continue probing
    }

    return {};
}

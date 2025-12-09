#include "movie.hpp"

/*
RESPONSABILIDADE DESTE ARQUIVO

- Implementar a classe MovieHashTable.
- Criar uma tabela de hash manual usando open addressing.
- NÃO usar std::map, unordered_map ou set.

DEVE IMPLEMENTAR:

1. Construtor MovieHashTable(capacity)
   - Inicializar vetor table com tamanho = capacity
   - Zerar contador de elementos

2. hash(int key)
   - Gerar índice inicial baseado no movieId

3. probe(index, step)
   - Retornar próximo índice em caso de colisão (linear ou quadrático)

4. insertOrGet(movieId)
   - Se movieId existir na tabela:
        retornar referência ao Movie já existente
   - Se não existir:
        alocar nova posição livre
        inicializar Movie.movieId = movieId
        incrementar contador
        retornar referência ao Movie criado

5. find(movieId)
   - Procurar elemento na tabela
   - Retornar ponteiro para Movie
   - Retornar nullptr se não existir

6. rawTable()
   - Retornar referência ao vetor de MovieHashEntry
*/



#include "movie.hpp"

#include <cstddef>

MovieHashTable::MovieHashTable(std::size_t capacity) : table(), count(0) {
    std::size_t cap = capacity == 0 ? 1 : capacity;
    table.resize(cap);
    // MovieHashEntry has default member initializers, so all fields are already in the desired state.
}

std::size_t MovieHashTable::hash(int key) const {
    return static_cast<std::size_t>(key) % table.size();
}

std::size_t MovieHashTable::probe(std::size_t index, std::size_t step) const {
    // Linear probing
    return (index + step) % table.size();
}

Movie& MovieHashTable::insertOrGet(int movieId) {
    std::size_t h = hash(movieId);
    std::size_t firstDeletedIndex = static_cast<std::size_t>(-1);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        MovieHashEntry& entry = table[idx];

        if (entry.occupied) {
            if (!entry.deleted && entry.key == movieId) {
                // Found existing entry
                return entry.value;
            }
            // occupied but different key or deleted; continue probing
        } else {
            // Not occupied
            if (entry.deleted) {
                // Mark first deleted slot to potentially reuse
                if (firstDeletedIndex == static_cast<std::size_t>(-1)) {
                    firstDeletedIndex = idx;
                }
                // Keep probing to ensure the key doesn't already exist
            } else {
                // Truly empty slot: we can stop searching
                if (firstDeletedIndex != static_cast<std::size_t>(-1)) {
                    idx = firstDeletedIndex;
                }
                MovieHashEntry& insertEntry = table[idx];
                insertEntry.key = movieId;
                insertEntry.value.movieId = movieId;
                insertEntry.occupied = true;
                insertEntry.deleted = false;
                ++count;
                return insertEntry.value;
            }
        }
    }

    // In theory, we should never reach here if capacity is sufficient.
    // As a fallback, insert at position 0.
    MovieHashEntry& entry = table[0];
    if (!entry.occupied || entry.deleted) {
        entry.key = movieId;
        entry.value.movieId = movieId;
        entry.occupied = true;
        entry.deleted = false;
        ++count;
    }
    return entry.value;
}

Movie* MovieHashTable::find(int movieId) {
    std::size_t h = hash(movieId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        MovieHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            // Never occupied: key not present
            return nullptr;
        }

        if (entry.occupied && !entry.deleted && entry.key == movieId) {
            return &entry.value;
        }
        // Otherwise, continue probing
    }

    return nullptr;
}

const Movie* MovieHashTable::find(int movieId) const {
    std::size_t h = hash(movieId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        const MovieHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            // Never occupied: key not present
            return nullptr;
        }

        if (entry.occupied && !entry.deleted && entry.key == movieId) {
            return &entry.value;
        }
        // Otherwise, continue probing
    }

    return nullptr;
}

std::vector<MovieHashEntry>& MovieHashTable::rawTable() {
    return table;
}

const std::vector<MovieHashEntry>& MovieHashTable::rawTable() const {
    return table;
}


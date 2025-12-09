#include "users.hpp"

/*
RESPONSABILIDADE DESTE ARQUIVO

- Implementar a classe UserHashTable.
- Controlar associação: userId -> lista de avaliações (UserRating).
- Implementação deve ser baseada em hash manual.
- NÃO usar std::map, unordered_map ou set.

DEVE IMPLEMENTAR:

1. Construtor UserHashTable(capacity)
   - Inicializar vetor de UserHashEntry
   - Zerar contador

2. hash(int key)
   - Hash simples para distribuir userId

3. probe(index, step)
   - Estratégia de colisão

4. insertOrGet(userId)
   - Se usuário já existir:
        retornar referência
   - Se não existir:
        criar novo User
        definir userId
        vetor de ratings começa vazio
        retornar referência

5. find(userId)
   - Buscar usuário na hash
   - Retornar ponteiro ou nullptr
*/


#include "users.hpp"

#include <cstddef>

UserHashTable::UserHashTable(std::size_t capacity) : table(), count(0) {
    std::size_t cap = capacity == 0 ? 1 : capacity;
    table.resize(cap);
}

std::size_t UserHashTable::hash(int key) const {
    return static_cast<std::size_t>(key) % table.size();
}

std::size_t UserHashTable::probe(std::size_t h, std::size_t step) const {
    return (h + step) % table.size();
}

User& UserHashTable::insertOrGet(int userId) {
    std::size_t h = hash(userId);
    std::size_t firstDeletedIndex = static_cast<std::size_t>(-1);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        UserHashEntry& entry = table[idx];

        if (entry.occupied) {
            if (!entry.deleted && entry.key == userId) {
                return entry.value;
            }
        } else {
            if (entry.deleted) {
                if (firstDeletedIndex == static_cast<std::size_t>(-1)) {
                    firstDeletedIndex = idx;
                }
            } else {
                if (firstDeletedIndex != static_cast<std::size_t>(-1)) {
                    idx = firstDeletedIndex;
                }
                UserHashEntry& insertEntry = table[idx];
                insertEntry.key = userId;
                insertEntry.value.userId = userId;
                insertEntry.value.ratings.clear();
                insertEntry.occupied = true;
                insertEntry.deleted = false;
                ++count;
                return insertEntry.value;
            }
        }
    }

    std::size_t idx = (firstDeletedIndex != static_cast<std::size_t>(-1))
                      ? firstDeletedIndex
                      : 0;
    UserHashEntry& entry = table[idx];
    if (!entry.occupied || entry.deleted) {
        entry.key = userId;
        entry.value.userId = userId;
        entry.value.ratings.clear();
        entry.occupied = true;
        entry.deleted = false;
        ++count;
    }
    return entry.value;
}

User* UserHashTable::find(int userId) {
    std::size_t h = hash(userId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        UserHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            return nullptr;
        }

        if (entry.occupied && !entry.deleted && entry.key == userId) {
            return &entry.value;
        }
    }

    return nullptr;
}

const User* UserHashTable::find(int userId) const {
    std::size_t h = hash(userId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        const UserHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            return nullptr;
        }

        if (entry.occupied && !entry.deleted && entry.key == userId) {
            return &entry.value;
        }
    }

    return nullptr;
}

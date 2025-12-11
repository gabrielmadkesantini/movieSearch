#include "users.hpp"
#include <cstddef>
#include <stdexcept>

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

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        UserHashEntry& entry = table[idx];

        if (entry.occupied) {
            // Achou o usuário certo
            if (!entry.deleted && entry.key == userId) {
                return entry.value;
            }
            // Caso contrário, continua probing
        } else {
            // Slot livre → inserir aqui
            entry.key               = userId;
            entry.value.userId      = userId;
            entry.value.ratings.clear();
            entry.occupied          = true;
            entry.deleted           = false;
            ++count;

            return entry.value;
        }
    }

    // Se chegou aqui, a tabela está cheia (não deveria acontecer no trabalho)
    throw std::runtime_error("UserHashTable::insertOrGet – Hash table full");
}

User* UserHashTable::find(int userId) {
    std::size_t h = hash(userId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        UserHashEntry& entry = table[idx];

        // Se achou um slot que nunca foi usado → não existe
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

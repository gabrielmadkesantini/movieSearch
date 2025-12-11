#include "movie.hpp"

#include <cstddef>
#include <stdexcept>

// Construtor que inicializa a tabela hash com a capacidade especificada
MovieHashTable::MovieHashTable(std::size_t capacity) : table(), count(0) {
    std::size_t cap = capacity == 0 ? 1 : capacity;
    table.resize(cap);
}

// Calcula o índice inicial baseado na chave (movieId)
std::size_t MovieHashTable::hash(int key) const {
    return static_cast<std::size_t>(key) % table.size();
}

// Realiza o cálculo do próximo índice em caso de colisão usando sondagem linear
std::size_t MovieHashTable::probe(std::size_t index, std::size_t step) const {
    return (index + step) % table.size();
}

Movie& MovieHashTable::insertOrGet(int movieId) {
    // índice base calculado a partir do movieId
    std::size_t h = hash(movieId);

    // sondagem linear
    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        MovieHashEntry& entry = table[idx];

        if (entry.occupied) {
            // Esse id já foi adicionado na tabela, retorna a referência ao Movie existente
            if (!entry.deleted && entry.key == movieId) {
                return entry.value;
            }
            // se for outro id ou marcado como deleted, continua sondando
        } else {
            // Slot vazio: insere aqui
            entry.key              = movieId;
            entry.value.movieId    = movieId;
            entry.occupied         = true;
            entry.deleted          = false;
            ++count;
            return entry.value;
        }
    }

    // Se chegou aqui, a tabela está cheia (situação anômala para o nosso contexto)
    throw std::runtime_error("MovieHashTable::insertOrGet – Hash table full");
}

// Percorre a tabela hash para encontrar o movieId especificado
Movie* MovieHashTable::find(int movieId) {
    std::size_t h = hash(movieId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        MovieHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            // Slot nunca usado: podemos concluir que o elemento não existe
            return nullptr;
        }

        if (entry.occupied && !entry.deleted && entry.key == movieId) {
            return &entry.value;
        }
        // Continua procurando em caso de colisão ou entrada deletada
    }

    return nullptr;
}

// Versão const de find
const Movie* MovieHashTable::find(int movieId) const {
    std::size_t h = hash(movieId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        const MovieHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            // Slot nunca usado: elemento não existe
            return nullptr;
        }

        if (entry.occupied && !entry.deleted && entry.key == movieId) {
            return &entry.value;
        }
        // Continua procurando em caso de colisão ou entrada deletada
    }

    return nullptr;
}

// Retorna referência ao array da tabela hash
std::vector<MovieHashEntry>& MovieHashTable::rawTable() {
    return table;
}

// Retorna referência constante ao array da tabela hash
const std::vector<MovieHashEntry>& MovieHashTable::rawTable() const {
    return table;
}

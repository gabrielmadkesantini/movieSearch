#include "movie.hpp"

#include <cstddef>


// Construtor que inicializa a tabela hash com a capacidade especificada
MovieHashTable::MovieHashTable(std::size_t capacity) : table(), count(0) {
    std::size_t cap = capacity == 0 ? 1 : capacity;
    table.resize(cap);
}

//Calcula o índice inicial baseado na chave (movieId)
std::size_t MovieHashTable::hash(int key) const {
    return static_cast<std::size_t>(key) % table.size();
}

//Realiza o calculo do próximo índice em caso de colisão usando sondagem linear
std::size_t MovieHashTable::probe(std::size_t index, std::size_t step) const {
    return (index + step) % table.size();
}

Movie& MovieHashTable::insertOrGet(int movieId) {
    //pega valor hash reepectivo ao moveId
    std::size_t h = hash(movieId);

    //Armazena o índice da primeira posição deletada encontrada durante a sondagem
    std::size_t firstDeletedIndex = static_cast<std::size_t>(-1);

    //executa o loop de sondagem para encontrar a posição correta(lembrando, essa parte aqui na verdade sempre começa em 0 pois é chamada
    // no arquivo loader.cpp quando o filme ainda não foi adicionado na tabela, e adicionao o id atual calculuado pelo hash obtido pelo id do filme)
    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        MovieHashEntry& entry = table[idx];

        if (entry.occupied) {
            //Esse id ja foi adicionado na tabela, retorna a referência ao Movie existente
            if (entry.key == movieId) {
                return entry.value;
            }
        } else {
                // Esta vazio, realiza a inserção e atualiza o contador
                MovieHashEntry& insertEntry = table[idx];
                insertEntry.key = movieId;
                insertEntry.value.movieId = movieId;
                insertEntry.occupied = true;
                insertEntry.deleted = false;
                ++count;
                return insertEntry.value;
            
        }
    }

    //Essa parte aqui na verdade nem vai ser utilizada, pois apenas e alcançada se a tabela estiver cheia
    //e nesse caso o ideal seria fazer um rehash, mas como o enunciado não pede, deixei assim mesmo
    //Insere na primeira posição disponível (deletada ou não ocupada)
    MovieHashEntry& entry = table[0];
    return entry.value;
}

//percorre a tabela hash para encontrar o movieId especificado
Movie* MovieHashTable::find(int movieId) {
    std::size_t h = hash(movieId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        MovieHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            //Elemento nao encontrado
            return nullptr;
        }

        if (entry.occupied && !entry.deleted && entry.key == movieId) {
            return &entry.value;
        }
        // Continua procurando em caso de colisão ou entrada deletada
    }

    return nullptr;
}

//percorre a tabela hash para encontrar o movieId especificado (caso em que a tabela seja declarada como constante)
const Movie* MovieHashTable::find(int movieId) const {
    std::size_t h = hash(movieId);

    for (std::size_t step = 0; step < table.size(); ++step) {
        std::size_t idx = probe(h, step);
        const MovieHashEntry& entry = table[idx];

        if (!entry.occupied && !entry.deleted) {
            //Elemento nao encontrado
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
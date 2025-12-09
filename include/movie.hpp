#pragma once

#include <string>
#include <vector>

// NÃO usar std::map, std::unordered_map, std::set etc.

struct Movie {
    int movieId;
    std::string title;
    std::string genres;
    int year;

    int ratingCount = 0;
    double ratingSum = 0.0;
};

struct MovieHashEntry {
    bool occupied = false;
    bool deleted  = false;
    int key = -1;
    Movie value;
};

class MovieHashTable {
public:
    explicit MovieHashTable(std::size_t capacity);

    Movie& insertOrGet(int movieId);
    Movie* find(int movieId);
    const Movie* find(int movieId) const;

    std::vector<MovieHashEntry>& rawTable();
    const std::vector<MovieHashEntry>& rawTable() const;

private:
    std::vector<MovieHashEntry> table;
    std::size_t count;

    std::size_t hash(int key) const;
    std::size_t probe(std::size_t h, std::size_t step) const;
};

#pragma once

#include <vector>
#include <string>

struct TagHashEntry {
    bool occupied = false;
    bool deleted = false;
    std::string key;
    std::vector<int> movieIds;
};

class TagHashTable {
public:
    explicit TagHashTable(std::size_t capacity);

    void addMovie(const std::string& tag, int movieId);
    std::vector<int> getMovies(const std::string& tag) const;

private:
    std::vector<TagHashEntry> table;
    std::size_t count;

    std::size_t hash(const std::string& s) const;
    std::size_t probe(std::size_t h, std::size_t step) const;
};

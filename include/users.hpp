#pragma once

#include <vector>

struct UserRating {
    int movieId;
    float rating;
};

struct User {
    int userId;
    std::vector<UserRating> ratings;
};

struct UserHashEntry {
    bool occupied = false;
    bool deleted = false;
    int key = -1;
    User value;
};

class UserHashTable {
public:
    explicit UserHashTable(std::size_t capacity);

    User& insertOrGet(int userId);
    User* find(int userId);
    const User* find(int userId) const;

private:
    std::vector<UserHashEntry> table;
    std::size_t count;

    std::size_t hash(int key) const;
    std::size_t probe(std::size_t h, std::size_t step) const;
};

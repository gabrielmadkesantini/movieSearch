#pragma once

#include "movie.hpp"
#include "users.hpp"
#include "tags.hpp"
#include "trie.hpp"

struct DataContext {
    MovieHashTable movies;
    UserHashTable users;
    TagHashTable tags;
    TitleTrie trie;

    DataContext(
        std::size_t movieCap = 30000,
        std::size_t userCap  = 300000,
        std::size_t tagCap   = 500000
    );
};

#include <iostream>
#include "trie.hpp"

int main() {

    TitleTrie trie;

    trie.insert("Toy Story", 1);
    trie.insert("Toy Story 2", 2);
    trie.insert("The Matrix", 3);
    trie.insert("Matrix Reloaded", 4);

    auto r1 = trie.searchPrefix("Toy");
    auto r2 = trie.searchPrefix("The");
    auto r3 = trie.searchPrefix("Matrix");
    auto r4 = trie.searchPrefix("X"); // deve ser vazio

    std::cout << "Prefix Toy -> ";
    for (int id : r1) std::cout << id << " ";
    std::cout << "\n";

    std::cout << "Prefix The -> ";
    for (int id : r2) std::cout << id << " ";
    std::cout << "\n";

    std::cout << "Prefix Matrix -> ";
    for (int id : r3) std::cout << id << " ";
    std::cout << "\n";

    std::cout << "Prefix X -> ";
    for (int id : r4) std::cout << id << " ";
    std::cout << "\n";

    return 0;
}

#pragma once

#include <string>
#include <vector>

struct TrieNode {
    bool isTerminal = false;
    std::vector<int> movieIds;
    TrieNode* children[128]{};

    TrieNode();
};

class TitleTrie {
public:
    TitleTrie();
    ~TitleTrie();

    void insert(const std::string& title, int movieId);
    std::vector<int> searchPrefix(const std::string& prefix) const;

private:
    TrieNode* root;

    void collect(TrieNode* node, std::vector<int>& out) const;
    void freeNode(TrieNode* node);
};

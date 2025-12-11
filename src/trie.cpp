#include "trie.hpp"

//Inicializa children com nullptr e isTerminal = false
TrieNode::TrieNode() : isTerminal(false), movieIds() {
    for (int i = 0; i < 128; ++i) {
        children[i] = nullptr;
    }
}

//Inicializa a raiz da TRIE como um nodo
TitleTrie::TitleTrie() {
    root = new TrieNode();
}

TitleTrie::~TitleTrie() {
    freeNode(root);
}

void TitleTrie::insert(const std::string& title, int movieId) {
    TrieNode* current = root;

    for (char ch : title) {
        unsigned char index = static_cast<unsigned char>(ch);
        if (index >= 128) {
            // Assumindo apenas ASCII como especificado.
            continue;
        }

        if (current->children[index] == nullptr) {
            current->children[index] = new TrieNode();
        }
        current = current->children[index];
    }

    current->isTerminal = true;
    current->movieIds.push_back(movieId);
}

std::vector<int> TitleTrie::searchPrefix(const std::string& prefix) const {
    std::vector<int> result;
    const TrieNode* current = root;

    for (char ch : prefix) {
        unsigned char index = static_cast<unsigned char>(ch);
        if (index >= 128) {
            // Prefixo com caractere fora de ASCII: não encontra nada.
            return {};
        }

        if (current->children[index] == nullptr) {
            return {};
        }
        current = current->children[index];
    }

    collect(const_cast<TrieNode*>(current), result);
    return result;
}

void TitleTrie::collect(TrieNode* node, std::vector<int>& out) const {
    if (node == nullptr) {
        return;
    }

    if (node->isTerminal) {
        out.insert(out.end(), node->movieIds.begin(), node->movieIds.end());
    }

    for (int i = 0; i < 128; ++i) {
        if (node->children[i] != nullptr) {
            collect(node->children[i], out);
        }
    }
}

void TitleTrie::freeNode(TrieNode* node) {
    if (node == nullptr) {
        return;
    }

    for (int i = 0; i < 128; ++i) {
        if (node->children[i] != nullptr) {
            freeNode(node->children[i]);
        }
    }

    delete node;
}

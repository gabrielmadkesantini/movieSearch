#include "trie.hpp"

/*
RESPONSABILIDADE DESTE ARQUIVO

- Implementar TRIE de títulos dos filmes.
- Cada nodo armazena:
    - Ponteiros para 128 caracteres ASCII possíveis.
    - Flag de final de título.
    - Lista de movieIds terminando naquele nodo.

DEVE IMPLEMENTAR:

1. Construtor TrieNode()
   - Inicializar children com nullptr
   - isTerminal = false

2. Construtor TitleTrie()
   - Criar root

3. Destruidor ~TitleTrie()
   - Liberar toda a memória recursivamente

4. freeNode(node)
   - Função auxiliar para deletar a TRIE inteira

5. insert(title, movieId)
   - Percorrer caractere por caractere
   - Criar nodos conforme necessário
   - Ao final:
        marcar isTerminal = true
        adicionar movieId à lista do nodo

6. searchPrefix(prefix)
   - Percorrer até o nodo do prefixo
   - Usar collect() para:
        percorrer toda subárvore
        carregar todos movieIds encontrados

7. collect(node, out)
   - Busca recursiva em profundidade:
        se node->isTerminal == true:
            adicionar todos movieIds do nodo ao vetor de saída
        visitar todos os filhos
*/


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

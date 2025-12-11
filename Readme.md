# Documentação do Projeto

Este projeto implementa um sistema de indexação e consulta de filmes utilizando estruturas otimizadas como Tabelas Hash e TRIE, além de leitura eficiente de grandes CSVs do MovieLens.

A seguir, cada componente do sistema é documentado separadamente.

## movie.cpp — Tabela Hash de Filmes

Responsável pela implementação da tabela hash que armazena informações sobre filmes.

- Indexa cada filme pelo movieId.
- Armazena: título, gêneros, ano, soma das notas e quantidade de avaliações.
- Inserções e buscas possuem tempo esperado O(1).
- Permite acesso para iteração completa da tabela, usado nas consultas como top filmes.

## trie.cpp — TRIE para Títulos de Filmes

Implementa a TRIE utilizada para busca por prefixo.

- Armazena cada título caractere a caractere.
- Nós terminais contêm uma lista de `movieId`s correspondentes ao título completo.
- Suporta consultas de prefixo muito rápidas.
- Utilizada na consulta prefix.

## users.cpp — Tabela Hash de Usuários

Implementa a estrutura hash que mapeia usuários para suas avaliações.

- Mapeia userId → User.
- Cada usuário contém uma lista de avaliações: (movieId, rating).
- Utilizada na consulta user.
- Permite recuperar rapidamente o histórico de avaliações de um usuário específico.

## tags.cpp — Tabela Hash de Tags

Estrutura responsável por armazenar listas de filmes associados a cada tag.

- Mapeia tag (string) → lista de movieIds.
- Base da consulta tags, que busca filmes por múltiplas tags.
- Suporta interseção rápida das listas para filtrar apenas filmes que possuam todas as tags dadas.

## data_loader.cpp — Leitura dos Arquivos CSV

Gerencia a importação dos dados dos arquivos:

- movies.csv
- ratings.csv
- tags.csv

Durante o carregamento:

- Insere filmes na Tabela Hash de Filmes.
- Insere títulos na TRIE.
- Atualiza soma e contagem de notas dos filmes.
- Popula a Tabela Hash de Usuários.
- Normaliza e adiciona tags na Tabela Hash de Tags.

Serve como etapa inicial para construir todo o DataContext.

## queries.cpp — Implementação das Consultas

Contém todas as operações que o usuário pode solicitar:

- Busca de títulos por prefixo (via TRIE).
- Consulta do histórico de avaliações de um usuário.
- Listagem dos top filmes por gênero.
- Busca de filmes por múltiplas tags (via interseção).
- Ordenações auxiliares e formatação da saída.

É o “cérebro” da parte interativa do projeto.

## main.cpp — Entrada do Programa

Arquivo principal responsável por:

- Inicializar o DataContext.
- Carregar todas as estruturas chamando o data_loader.
- Ler comandos digitados pelo usuário.
- Encaminhar cada comando para a função apropriada em queries.cpp.

## sort_utils.cpp — Utilidades de Ordenação

Arquivo auxiliar para rotinas de ordenação do sistema.

- Inclui sort_utils.hpp, contendo templates e funções de ordenação.
- Usado como suporte interno para ordenação nas consultas.
- Não implementa nenhuma estrutura de dados principal.
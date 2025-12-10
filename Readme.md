movie.cpp
Responsável pela implementação da Tabela Hash de Filmes.
Armazena os dados de cada filme indexados por movieId: título, gêneros, ano, soma das avaliações e quantidade de avaliações. Permite inserção e busca em tempo O(1) esperado, além de acesso à tabela completa para iteração nas consultas.

trie.cpp
Implementa a TRIE de títulos dos filmes.
Armazena todos os títulos caractere a caractere e, nos nós terminais, guarda a lista de movieIds associados ao título completo. É utilizada para a busca eficiente por prefixo na consulta “prefix”.

users.cpp
Implementa a Tabela Hash de Usuários.
Mapeia userId para uma estrutura User que contém uma lista de avaliações do usuário, cada uma com movieId e nota. É utilizada para responder à consulta “user”, permitindo encontrar rapidamente o histórico de avaliações de um usuário específico.

tags.cpp
Implementa a Tabela Hash de Tags.
Mapeia cada tag (string) para uma lista de movieIds que receberam aquela marcação. Dá suporte à consulta “tags”, permitindo buscar filmes associados a uma ou mais tags por meio da interseção dessas listas.

data_loader.cpp
Responsável pela leitura dos arquivos movies.csv, ratings.csv e tags.csv.
Durante o carregamento:
– Insere filmes na Tabela Hash de Filmes e seus títulos na TRIE.
– Atualiza soma e contagem de notas dos filmes a partir das avaliações e preenche a Tabela Hash de Usuários.
– Normaliza as tags e popula a Tabela Hash de Tags associando tags aos filmes correspondentes.

queries.cpp
Implementa todas as consultas do sistema:
– Busca por prefixo de título usando a TRIE.
– Consulta do histórico de avaliações de um usuário usando a Tabela Hash de Usuários.
– Consulta de top filmes por gênero usando a Tabela Hash de Filmes.
– Consulta de filmes por múltiplas tags usando a Tabela Hash de Tags e interseção de listas.
Também realiza ordenações auxiliares e a formatação da saída.

main.cpp
Arquivo principal do programa.
Inicializa o DataContext, carrega todos os CSVs por meio do data_loader e executa o loop de leitura de comandos do usuário, direcionando cada comando para a query correspondente.

sort_utils.cpp
Arquivo auxiliar de ordenação.
Inclui o header sort_utils.hpp, que contém implementações template de funções de ordenação usadas nas consultas. Serve apenas como suporte técnico e não implementa uma estrutura de dados específica.

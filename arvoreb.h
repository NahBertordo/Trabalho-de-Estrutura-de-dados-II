#ifndef ARVOREB_H // Macro de guarda: evita que o arquivo seja incluido mais de uma vez na compilação
#define ARVOREB_H // Define a constante para o controle do #ifndef

#define T 3 // Ordem minima da arvore B, Cada no (menos a raiz) devete ter entre T-1 e 2T-1 chaves

typedef struct NoArvoreB NoArvoreB;

// ------Prototipos das funções------
// Funcoes do TAD
NoArvoreB *criarNo(int folha);          // Aloca memoria para um novo no e inicializa seus valores padrões
void inserir(int k, long pos);          // Função principal de inserção: coordena a lógica de divisões (split) e crescimento da arvore
long buscar(NoArvoreB *x, int k);       // Percorre a árvore recursivamente para encontrar para encontrar uma matricula 'k' e retorna sua posição no arquivo
void liberarArvore(NoArvoreB *x);       // Percorre toda a árvore liberamdo a memória alocada com malloc para evitar vazamento de memória
void gravarNo(NoArvoreB *x, FILE *out); // Função auxiliar para percorrer a árvore e escrever sua estrutura em um arquivo de texto para visualização

// Diz ao compilador que a variavel 'RAIZ' existe, mas sua definicao real (alocação) está no arquivo.c
// Variavel global para a RAIZ
extern NoArvoreB *RAIZ;

#endif // Fim do bloco de proteção ARVOREB_H
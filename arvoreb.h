#ifndef ARVOREB_H // Macro de guarda: evita que o arquivo seja incluido mais de uma vez na compilação
#define ARVOREB_H // Define a constante para o controle do #ifndef

#define T 3 // Ordem minima da arvore B, Cada no (menos a raiz) devete ter entre T-1 e 2T-1 chaves

// Estrutura de um registro, os dados que serão salvos no arquivo binario
typedef struct
{
    int matricula;     // Chave primaria(Id unico do usuario)
    char nome[50];     // Nome do aluno
    char telefone[20]; // Telefone de contato
} Cadastro;

// Estrutura de uma página(nó) da arvore B
typedef struct NoArvoreB
{
    int n;                           // Guarda a quantidade atual de chaves inseridas neste nó
    int chaves[2 * T - 1];           // Guarda as matriculas no vetor (indices de busca)
    long pos[2 * T - 1];             // Guarda o deslocamento no vetor (byte offset) do registro no arquivo binario
    struct NoArvoreB *filhos[2 * T]; // Vetor de ponteiros para os nos filhos (sempre n + 1 filhos)
    int folha;                       // Booleano: 1 se o nó for uma folha (nao tem filhos), 0 caso contrario
} NoArvoreB;

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
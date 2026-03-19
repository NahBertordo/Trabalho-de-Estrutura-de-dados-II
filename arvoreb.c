#include <stdio.h>
#include <stdlib.h>
#include "arvoreb.h"

// Estrutura de uma página(nó) da arvore B
typedef struct NoArvoreB
{
    int n;                           // Guarda a quantidade atual de chaves inseridas neste nó
    int chaves[2 * T - 1];           // Guarda as matriculas no vetor (indices de busca)
    long pos[2 * T - 1];             // Guarda o deslocamento no vetor (byte offset) do registro no arquivo binario
    struct NoArvoreB *filhos[2 * T]; // Vetor de ponteiros para os nos filhos (sempre n + 1 filhos)
    int folha;                       // Booleano: 1 se o nó for uma folha (nao tem filhos), 0 caso contrario
} NoArvoreB;

NoArvoreB *RAIZ = NULL; // Define a variavel global que aponta para o topo da árvore

// Aloca momória para um nó e limpa os ponteiros de filhos
NoArvoreB *criarNo(int folha)
{
    NoArvoreB *no = (NoArvoreB *)malloc(sizeof(NoArvoreB)); // Aloca o espaco na memoria RAM para a estrutura do nó
    no->folha = folha;                                      // Define se o nó é folha (1) ou interno (0)
    no->n = 0;                                              // Inicializa com zero chaves
    // Garante que todos os ponteiros de filhos comecem apontando para o vazio (NULL)
    for (int i = 0; i < 2 * T; i++)
        no->filhos[i] = NULL;

    // Inicializa as chaves com -1 (ou 0) para indicar que estão vazia
    for (int i = 0; i < (2 * T - 1); i++)
    {
        no->chaves[i] = -1; // Altere para 0 se preferir
    }

    return no;
}

// Busca uma matricula na arvore, procurando por uma chave 'k' comecando pelo nó 'x'
long buscar(NoArvoreB *x, int k)
{
    // Árvore vazia ou chave não encontrada
    if (x == NULL)
        return -1;
    int i = 0;
    // Percorre as chaves do nó atual enquanto a chave buscada for maior
    while (i < x->n && k > x->chaves[i])
        i++;
    // Se achou a chave no indice atual, retorna a posicao dela no arquivo de dados
    if (i < x->n && k == x->chaves[i])
        return x->pos[i];
    // Se chegou na folha e não achou, a chave não existe na árvore
    if (x->folha)
        return -1;
    return buscar(x->filhos[i], k); // Caso contrário, desce recursivamente para o filho apropriado
}

// Divide o nó 'y' (que está cheio) ao meio e soobe a mediana para o nó 'pai'
static void dividirFilho(NoArvoreB *pai, int i, NoArvoreB *y)
{
    NoArvoreB *z = criarNo(y->folha); // Cria um novo nó 'z' para receber a metade direita de 'y'
    z->n = T - 1;                     // O novo nó terá T-1 chaves

    // Copia as ultimas T-1 chaves de 'y' para 'z'
    for (int j = 0; j < T - 1; j++)
    {
        z->chaves[j] = y->chaves[j + T];
        z->pos[j] = y->pos[j + T];
    }
    // Se 'y' não for folha, 'z' também precisa levar os filhos correspondentes
    if (!y->folha)
    {
        for (int j = 0; j < T; j++)
            z->filhos[j] = y->filhos[j + T];
    }
    y->n = T - 1; // Reduz o número de chaves em 'y' (metade esquerda)
    // Abre espaço no nó pai para o novo ponterio do filho 'z'
    for (int j = pai->n; j >= i + 1; j--)
        pai->filhos[j + 1] = pai->filhos[j];
    pai->filhos[i + 1] = z; // Liga o pai ao novo nó 'z'
    // Abre espaço no nó pai para a chave que vai subir de 'y'
    for (int j = pai->n - 1; j >= i; j--)
    {
        pai->chaves[j + 1] = pai->chaves[j];
        pai->pos[j + 1] = pai->pos[j];
    }
    // Sobe a chave mediana de 'y' para o 'pai'
    pai->chaves[i] = y->chaves[T - 1];
    pai->pos[i] = y->pos[T - 1];
    pai->n++; // incrementa contador de chaves do pai
}

// Inser a chave 'k' assumindo que o nó 'x' tem espaço disponível
static void inserirNaoCheio(NoArvoreB *x, int k, long pos)
{
    int i = x->n - 1;
    if (x->folha) // Se for folha, basta achar a posiçao e inserir ordenado
    {
        // Desloca chaves maiores para a direita
        while (i >= 0 && k < x->chaves[i])
        {
            x->chaves[i + 1] = x->chaves[i];
            x->pos[i + 1] = x->pos[i];
            i--;
        }
        // Insere a nova chave na posição correta
        x->chaves[i + 1] = k;
        x->pos[i + 1] = pos;
        x->n++;
    }
    else // Se não for folha, precisa descer na árvore
    {
        // Acha  qual filho deve receber a chave
        while (i >= 0 && k && x->chaves[i])
            i--;
        i++;
        // Se o filho escolhido estiver cheio, divide-o antes de descer
        if (x->filhos[i]->n == 2 * T - 1)
        {
            dividirFilho(x, i, x->filhos[i]);
            // Após a divisão, decide qual dos dois novos nós receberá a chave
            if (k > x->chaves[i])
                i++;
        }
        inserirNaoCheio(x->filhos[i], k, pos);
    }
}

// Função para inserir: trata o crescimento da raiz
void inserir(int k, long pos)
{
    if (RAIZ == NULL) // Caso a árvore esteja totalmente vazia
    {
        RAIZ = criarNo(1);
        RAIZ->chaves[0] = k;
        RAIZ->pos[0] = pos;
        RAIZ->n = 1;
    }
    else
    { // Se a raiz estiver cheia, a árvore cresce para cima (ganha um novo nível)
        if (RAIZ->n == 2 * T - 1)
        {
            NoArvoreB *s = criarNo(0); // Novo nó que será a nova raiz
            s->filhos[0] = RAIZ;       // Antiga raiz vira filha da nova
            dividirFilho(s, 0, RAIZ);  // Divide a antiga raiz
            // Escolhe qual lado da nova raiz receberá o valor
            int i = (k > s->chaves[0]) ? 1 : 0;
            inserirNaoCheio(s->filhos[i], k, pos);
            RAIZ = s; // Atuliza o ponteiro global da raiz
        }
        else
        {
            inserirNaoCheio(RAIZ, k, pos); // Se a raiz tem espaço, apenas insere normalmente
        }
    }
}

// Remove todos os nós da memória (Pós-ordem)
void liberarArvore(NoArvoreB *x)
{
    if (x == NULL)
        return;
    if (!x->folha)
    {
        // Primeiro libera todos os filhos recursivamente
        for (int i = 0; i <= x->n; i++)
            liberarArvore(x->filhos[i]);
    }
    free(x); // Depois libera o próprio nó
}

// Percorre a árvore e implrime as chaves em um arquivo de texto para debug
void gravarNo(NoArvoreB *x, FILE *out)
{
    if (x == NULL)
        return;
    fprintf(out, "No %p: ", (void *)x); // imprime o endereço de memória do nó
    for (int i = 0; i < x->n; i++)
    {
        // Imprime a mátricula e a posição no arquivo binário
        fprintf(out, "[%d @ %ld] ", x->chaves[i], x->pos[i]);
    }
    fprintf(out, "\n");
    if (!x->folha)
    {
        // Continua gravando os filhos recursivamente
        for (int i = 0; i <= x->n; i++)
        {
            if (x->filhos[i] != NULL)
                gravarNo(x->filhos[i], out);
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include "arvoreb.h"

FILE *arquivoRegistros; // Ponteiro global para gerenciar o arquivo binário onde os dados (Cadastro) ficam salvos

// Reconstrói a Árvore B lendo o arquivo binário do início ao fim
void carregarArvore()
{
    rewind(arquivoRegistros); // Volta o cursor do arquivo para o byte zero
    Cadastro cad;
    long pos;
    // Lê um registro por vez até chegar ao fim do arquivo
    while (fread(&cad, sizeof(Cadastro), 1, arquivoRegistros) == 1)
    {
        // Calcula a posição onde o registro começou: posiçaõ atual - tamanho  do registro
        pos = ftell(arquivoRegistros) - sizeof(Cadastro);
        // Insere a matrícula e sua posição física na árvore B (índice em memória RAM)
        inserir(cad.matricula, pos);
    }
}

// Interface para entrada de dados e salvamento no disco + índice
void cadastrar()
{
    Cadastro cad;

    printf("Matricula: ");
    // Lê a matrícula a verifica se a entrada é um número interiro válido
    if (scanf("%d", &cad.matricula) != 1)
    {
        while (getchar() != '\n')
            ; // Limpa o lixo d buffer do teclado
        printf("Entrada invalida!\n");
        return;
    }

    // Verifica se matricula já esta cadastrada
    if (buscar(RAIZ, cad.matricula) != -1)
    {
        printf("Erro: Matricula %d ja cadastrada!\n", cad.matricula);
        return;
    }

    // Lê strings com espaços usando %[^\n]
    printf("Nome: ");
    scanf(" %[^\n]", cad.nome);
    printf("Telefone: ");
    scanf(" %[^\n]", cad.telefone);

    // Posiciona o cursor n ofiinal do arquivo binário para adicionar o novo registro
    fseek(arquivoRegistros, 0, SEEK_END);
    long pos = ftell(arquivoRegistros); // Pega o endereço (byte) desta nova posição

    // Escreve os dados estruturados no arquivo binário
    fwrite(&cad, sizeof(Cadastro), 1, arquivoRegistros);
    fflush(arquivoRegistros); // Garante que o windows descarregue o buffer para o disco imediatamente

    // Atualiza o índice (Árvore B) com a nova matrícula e sua posição no arquivo
    inserir(cad.matricula, pos);
    printf("Cadastro realizado!n");
}

// busca um registro usando a Árvore B para acesso rápido (sem ler o arquivo todo)
void pesquisar()
{
    int mat;
    printf("Matricula: ");
    scanf("%d", &mat);
    // Busca o deslocamento (byte offset) na Árvore B (O(log n))
    long pos = buscar(RAIZ, mat);
    if (pos == -1)
    {
        printf("Matricula nao encontrada.\n");
        return;
    }
    // Se achou a posição, pula direto para o byte correto no arquivo (Acesso Aleatório)
    Cadastro cad;
    fseek(arquivoRegistros, pos, SEEK_SET);
    fread(&cad, sizeof(Cadastro), 1, arquivoRegistros); // Lê apenas os dados daquele aluno
    printf("Nome: %s | Telefone: %s\n", cad.nome, cad.telefone);
}

// Salva a estrutura visual da árvore em um arquivo de texto para conferência
void gravar(const char *nomeArquivo)
{
    FILE *out = fopen(nomeArquivo, "w");
    if (!out)
    {
        printf("Erro ao abri arquivo de gravacao!\n");
        return;
    }
    fprintf(out, "RAIZ: %p\n", (void *)RAIZ);
    gravarNo(RAIZ, out); // Chama a funçaõ recursiva do arvoreb.c
    fclose(out);
    printf("Arvore gravada sem %s\n", nomeArquivo);
}

// Programa principal
int main(int argc, char *argv[])
{
    // Verifica se o usuário passou o nomes dos arquivos via terminal
    if (argc < 3)
    {
        printf("Uso: %s <dados.bin> <arvore.txt\n", argv[0]);
        return 1;
    }

    // Tenta abrir para leitura e escrita binária. Se não existir, cria o arquivo.
    arquivoRegistros = fopen(argv[1], "r+b");
    if (!arquivoRegistros)
        arquivoRegistros = fopen(argv[1], "w+b");
    if (!arquivoRegistros)
    {
        printf("Erro ao abrir arquivo de registro.\n");
        return 1;
    }

    // carrega o índice na memória assim que o programa abre
    carregarArvore();

    int opcao;
    do
    {
        printf("\n------------------- MENU -------------------\n");
        printf("1 - Cadastrar\n2 - Pesquisar\n3 - Gravar\n4 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1:
            cadastrar();
            break;
        case 2:
            pesquisar();
            break;
        case 3:
            gravar(argv[2]);
            break;
        case 4:
            liberarArvore(RAIZ); // Limpa a me,ória RAM antes de fechar
            break;
        default:
            printf("Opcao invalida!\n");
        }
    } while (opcao != 4);

    fclose(arquivoRegistros); // Fecha o arquivo de dados
    printf("Programa encerrado.\n");
    return 0;
}

// Comandos usados:
// gcc -o programa.exe main.c arvoreb.c
// ./programa.exe dados.bin arvore.txt
// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definindo limites
#define MAX_LINHAS 100
#define TAM_LINHAS 256

#define MAX_ALFABETO 50
#define TAM_ALFABETO 50 

#define MAX_ESTADOS 50
#define TAM_ESTADOS 50 

#define MAX_TRANSICOES 100
#define TAM_TRANSICOES 100

#define MAX_PALAVRAS 100
#define TAM_PALAVRAS 100

//Para armazenar as linhas
typedef struct{
    char texto[MAX_LINHAS][TAM_LINHAS]; //matriz
    int qtd; //qtd de linhas
}ListaDeLinhas;

typedef struct{
    char alfabeto[MAX_ALFABETO][TAM_ALFABETO];
    int qtd_alfabeto;

    char estados[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_estados;

    char estado_inicial[TAM_ESTADOS]; // Apenas 1 estado inicial

    char estados_finais[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_finais;

    char transicoes[MAX_TRANSICOES][TAM_TRANSICOES];
    int qtd_transicoes;

    char palavras[MAX_PALAVRAS][TAM_PALAVRAS];
    int qtd_palavras;
}afnd;

//Função que lê o arquivo, ignora as linhas com '#' e armazena as outras no struct
void carregarArquivo(const char *Nomearquivo, ListaDeLinhas *lista){
    FILE *arquivo = fopen(Nomearquivo, "r");
    lista->qtd =0;
    if(arquivo==NULL){
        printf("Erro: Não foi possível abrir o arquivo!");
        return;
    }
    char linha[TAM_LINHAS];
    while(fgets(linha, TAM_LINHAS, arquivo) != NULL){
        if(linha[0]=='#' || linha[0] == '\n' || linha[0] == '\r'){
            continue; //força o loop a continuar, ignorando essa linha
        }
        if(lista->qtd >= MAX_LINHAS){ // Se a lista encher, para de ler
            printf("Limite de linhas atingido!");
            break;
        }
        // Copia a linha para a lista de listas, onde armazenaremos as linhas para usá-las depois
        strcpy(lista->texto[lista->qtd], linha);//destino -> origem, ou seja, veio do buffer "linha" e vai para a lista de lista
        lista->qtd++;
    }
    fclose(arquivo);
}

void imprimeLinhas(ListaDeLinhas *lista){
    for(int i=0; i<lista->qtd; i++){
        printf("%s", lista->texto[i]);
    }
}

void processarAfnd(ListaDeLinhas *lista, afnd *afnd) {
    afnd->qtd_alfabeto = 0;
    afnd->qtd_estados = 0;
    afnd->qtd_finais = 0;
    afnd->qtd_transicoes = 0;
    afnd->qtd_palavras = 0;

    for (int i = 0; i < lista->qtd; i++) {
        char buffer[TAM_LINHAS];
        strcpy(buffer, lista->texto[i]);
        char *token = strtok(buffer, " \t\r\n");
        if (token == NULL) continue;

        if (strcmp(token, "A") == 0) {
            while ((token = strtok(NULL, " \t\r\n"))) {
                strcpy(afnd->alfabeto[afnd->qtd_alfabeto++], token);
            }
        } else if (strcmp(token, "Q") == 0) {
            while ((token = strtok(NULL, " \t\r\n"))) {
                strcpy(afnd->estados[afnd->qtd_estados++], token);
            }
        } else if (strcmp(token, "q") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) strcpy(afnd->estado_inicial, token);
        } else if (strcmp(token, "F") == 0) {
            while ((token = strtok(NULL, " \t\r\n"))) {
                strcpy(afnd->estados_finais[afnd->qtd_finais++], token);
            }
        } else if (strcmp(token, "T") == 0) {
            char *origem = strtok(NULL, " \t\r\n");
            char *simbolo = strtok(NULL, " \t\r\n");
            char *destino = strtok(NULL, " \t\r\n");
            if (origem && simbolo && destino) {
                sprintf(afnd->transicoes[afnd->qtd_transicoes++], "%s %s %s", origem, simbolo, destino);
            }
        } else if (strcmp(token, "P") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) strcpy(afnd->palavras[afnd->qtd_palavras++], token);
            else strcpy(afnd->palavras[afnd->qtd_palavras++], ""); 
        }
    }
}

int ehEstadoFinal(afnd *afnd, char *estado) {
    for(int i=0; i < afnd->qtd_finais; i++) {
        if(strcmp(afnd->estados_finais[i], estado) == 0) {
            return 1;
        }
    }
    return 0;
}


int simular(afnd *afnd, char *estadoAtual, char *palavra, int indice) {
    //as transições são porcessadas aqui
    if (indice == (int)strlen(palavra)) {
        return ehEstadoFinal(afnd, estadoAtual);
    }

    char simboloAtual[2] = {palavra[indice], '\0'};

    for (int i = 0; i < afnd->qtd_transicoes; i++) {
        char copia[TAM_TRANSICOES];
        strcpy(copia, afnd->transicoes[i]);
        
        char *origem = strtok(copia, " ");
        char *lido = strtok(NULL, " ");
        char *destino = strtok(NULL, " ");

        if (strcmp(origem, estadoAtual) == 0 && strcmp(lido, simboloAtual) == 0) {
            // Se este caminho levar à aceitação, retorna 1 (Verdadeiro)
            if (simular(afnd, destino, palavra, indice + 1)) {
                return 1;
            }
        }
    }
    return 0; // Se testou todos os caminhos e nenhum chegou ao fim aceito, rejeita
}

void processarPalavras(afnd *afnd, const char *fileSaida) {
    FILE *saida = fopen(fileSaida, "w");
    if (saida == NULL) {
        printf("Erro: Nao foi possivel criar o arquivo de saida %s\n", fileSaida);
        return;
    }
    for (int i = 0; i < afnd->qtd_palavras; i++) {
        if (simular(afnd, afnd->estado_inicial, afnd->palavras[i], 0)) {
            fprintf(saida, "N aceita a palavra <%s>\n", afnd->palavras[i]);
        } else {
            fprintf(saida, "N rejeita a palavra <%s>\n", afnd->palavras[i]);
        }
    }
    fclose(saida);
}


int main(){
    ListaDeLinhas entrada;
    afnd afnd;
    const char *file = "saida.txt";
    carregarArquivo("e1.txt", &entrada);
    processarAfnd(&entrada, &afnd);
    processarPalavras(&afnd, file);
    return 0;
}
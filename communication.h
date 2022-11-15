//
// Created by Vasco on 11/15/2022.
//
#define TAM_MAX 50
#ifndef SO_2022_ISEC_SUPPORT_H
#define SO_2022_ISEC_SUPPORT_H
struct LigacaoServidor{
    int userPID;
    char alavra[TAM_MAX];
};
struct LigacaoCliente {//resposta do cliente
    char nome[TAM_MAX];
    char palavra[TAM_MAX];
    int userPID;
    char gameName[TAM_MAX];
    int gamePID;
    int gamescore;
    int gpFD[2];
    int pgFD[2];
    int status; // status-> 0 para primeira mensagem (onde o user é registado), 1 para tudo o resto
    int locked;
};
#endif



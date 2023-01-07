//
// Created by Vasco on 11/15/2022.
//
#define TAM_MAX 100
#define TAM_ITEM 200
#ifndef SO_2022_ISEC_SUPPORT_H
#define SO_2022_ISEC_SUPPORT_H
struct LigacaoServidor{
    int userPID;
    char palavra[TAM_MAX];
    char Todoitem[31][TAM_ITEM];
    int  valor;
};
struct LigacaoCliente {//resposta do cliente
    char user[TAM_MAX];
    char palavra[TAM_MAX];
    char password[TAM_MAX];
    char sell[TAM_ITEM];
    int userPID;
    int id;
    int bidding;
    int gpFD[2];
    int pgFD[2];
    int status; // status-> 0 para primeira mensagem (onde o user é registado), 1 para tudo o resto
    int locked;
};
#endif



//
// Created by Vasco on 11/15/2022.
//
#define TAM_MAX 50
#ifndef SO_2022_ISEC_SUPPORT_STRUCTURES_H
#define SO_2022_ISEC_SUPPORT_STRUCTURES_H
#define MAX_ITEMS 50
#define MAX_USERS 20
#define MAX_PROM 10
struct Cliente{
    char username[TAM_MAX];
    char password[TAM_MAX];
    int saldo;
};
struct Item{
    int id;
    char nome[TAM_MAX];
    char categoria[TAM_MAX];
    int valor_atual;
    int valor_compra;
    int tempo_leilao;
    char username_vendedor[TAM_MAX];
    char username_comprador[TAM_MAX];

};
#endif //SO_2022_ISEC_SUPPORT_STRUCTURES_H

#ifndef __FileLib_Tro
#define __FileLib_Tro

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FILE_SUFIX ".banana"

#define FILE_STATUS_INACTIVE 0
#define FILE_STATUS_WRITE 1
#define FILE_STATUS_READ 2

/**
 * \brief Abre um arquivo para leitura, caso o arquivo não exista, ele será criado.
 * 
 * \param path Caminho para o arquivo.
 * 
 * \return Quantidade de dados no arquivo, -1 caso algum erro tenha acontecido.
 */
int loadFile(char * path);

/**
 * \brief Abre um arquivo para escrita, caso o arquivo não exista, ele não será aberto.
 * 
 * \param path Caminho para o arquivo.
 * \param n Quantidade de dados que serão escritos.
 * 
 * \return True caso tudo tenha dado certo, false caso o contrário.
 */
bool saveFile(char * path);

/**
 * \brief Fecha o arquivo aberto.
 * 
 * \return True caso tudo tenha dado certo, false caso algum erro aconteceu.
 */
bool closeFile();

/**
 * \brief Função interna do FileLib, adiciona o nome e dado pra pilha de dados.
 * 
 * \param name Nome do dado que será pilhado.
 * \param data Dado que será pilhado.
 * 
 * \return True caso tudo tenha dado certo, false caso contrário.
 */
bool addToDump(char * name, char * data);

/**
 * \brief Escreve um dado em um arquivo aberto com loadFile.
 * 
 * \param data Dado que será escrito.
 * 
 * \return True caso tudo tenha dado certo, false caso o contrário.
 */
bool writeData(char * name, char * data);

/**
 * \brief Lê um dado em um arquivo aberto com readFile.
 * 
 * \return NULL caso algúm erro tenha acontecido, valor do dado caso tudo tenha dado certo.
 */
char * readData(char * name);

/**
 * \brief Inicializa o sistema de arquivos.
 * 
 * \return Sempre true.
 */
bool fileInit();

/**
 * \brief Finaliza o sistema de arquivos.
 * 
 * \return Sempre true.
 */
bool fileShut();

#endif
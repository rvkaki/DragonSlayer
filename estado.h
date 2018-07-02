#ifndef ___ESTADO_H___
#define ___ESTADO_H___

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
@file estado.h
Definição do estado e das funções que convertem estados em ficheiros e vice-versa
*/

/** \brief Número máximo de inimigos */
#define MAX_INIMIGOS		30

/** \brief Número máximo de obstáculos */
#define MAX_OBSTACULOS		20

/** \brief Número máximo de scores */
#define NUM_SCORES			5

/** \brief Número inicial de vidas */
#define VIDAS				5

/** \brief Caminho do ficheiro de estado */
#define FICHEIRO_ESTADO		"/var/www/html/estado"

/**
\brief Estrutura que armazena uma posição
*/
typedef struct posicao {
	/** \brief Coordenada x da posição */
	int x;
	/** \brief Coordenada y da posição */
	int y;
} POSICAO;

/**
\brief Estrutura que armazena a posição e o tipo de um inimigo
*/
typedef struct inimigo {
	POSICAO posicao;
	/** \brief Tipo do inimigo (1 spriggan, 0 dragão) */
	int spriggan;
} INIMIGO;

/**
\brief Estrutura que armazena o estado do jogo
*/
typedef struct estado {
	/** \brief Posição do jogador */
	POSICAO jog;
	/** \brief Número de inimigos */
	int num_inimigos;
	/** \brief Número de obstáculos */
	int num_obstaculos;
	/** \brief Posição da poção */
	POSICAO pocao;
	/** \brief Array com a posição dos inimigos */
	INIMIGO inimigo[MAX_INIMIGOS];
	/** \brief Array com a posição dos obstáculos */
	POSICAO obstaculo[MAX_OBSTACULOS];
	/** \brief Posição da entrada */
	POSICAO entrada;
	/** \brief Posição da saída */
	POSICAO saida;
	/** \brief Nível atual */
	int nivel;
	/** \brief Score atual */
	int score_atual;
	/** \brief Array com os scores */
	int scores[NUM_SCORES];
	/** \brief Índice do último score (0-4 se está nos scores, -1 se não, -2 se é o mais recente) */
	int idx_ultimo_score;
	/** \brief Número de vidas */
	int vidas;
	/** \brief Número de inimigos mortos */
	int inimigos_mortos;
	/** \brief Ecrã a ser mostrado (0 tabuleiro, 1 menu, 2 melhores scores, 3 ajuda) */
	int mostrar_ecra;
	/** \brief Nível de dificuldade (1 hardcore, 0 normal) */
	int hardcore;
	/** \brief Mostrar casas atacadas */
	int mostrar_casas_atacadas;
} ESTADO;

/**
\brief Função que converte um estado no ficheiro de estado
@param e o estado
*/
void estado2ficheiro(ESTADO e);

/**
\brief Função que converte o conteúdo do ficheiro de estado num estado 
@returns o estado correspondente ao conteúdo do ficheiro de estado
*/
ESTADO ficheiro2estado();

/**
\brief Função que aplica uma ação ao ficheiro de estado
@param acao a ação a aplicar
@param x coordenada x
@param y coordenada y
*/
void aplicar_acao(char *acao, int x, int y);

#endif

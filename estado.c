#include "estado.h"

/////////////////////////////////////// Funções de DragonSlayer.c ///////////////////////////////////////
ESTADO inicializar(int nivel, int score, int *scores, int vidas, int inimigos_mortos, int mostrar_ecra, int hardcore, int mostrar_casas_atacadas, int idx_ultimo_score);
ESTADO move_inimigos(ESTADO e, int a, int b);
int tem_inimigo(ESTADO e, int x, int y);
int posicao_igual(POSICAO p, int x, int y);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@file estado.c
Código do estado e das funções que convertem estados em ficheiros e vice-versa
*/

/**
\brief Função que mata um inimigo
@param e estado
@param x coordenada x do inimigo
@param y coordenada y do inimigo
@returns estado
*/
ESTADO mata_inimigo(ESTADO e, int x, int y){
	int i;
	for (i = 0; i < e.num_inimigos; i++) {
		if (posicao_igual(e.inimigo[i].posicao, x, y)) {
			e.inimigo[i] = e.inimigo[--e.num_inimigos];
			break;
		}
	}
	e.inimigos_mortos++;

	return e;
}

/**
\brief Função que atualiza o array de scores
@param e estado
@returns estado
*/
ESTADO atualiza_scores(ESTADO e) {
	int i = 0, aux, aux2, idx_ultimo_score = -2;
	while (i < NUM_SCORES){
		if (e.score_atual > e.scores[i]) {
			idx_ultimo_score = i;
			aux = e.scores[i];
			e.scores[i] = e.score_atual;
			i++;
			break;
		}
		i++;
	}

	while (i < NUM_SCORES){
		aux2 = e.scores[i];
		e.scores[i] = aux;
		aux = aux2;
		i++;
	}

	e.idx_ultimo_score = idx_ultimo_score;

	return e;
}

ESTADO ficheiro2estado() {
	FILE *f;
	f = fopen(FICHEIRO_ESTADO, "r");
	if (f == NULL) {
		perror("Erro a ler o ficheiro de estado");
		exit(1);
	}

	ESTADO e;
	int *p = (int *) &e;
	unsigned int i;
	int d;

	for(i = 0; i < (sizeof(ESTADO) / sizeof(int)); i++) {
		if (fscanf(f, "%d", &d) != 1) {
			e = inicializar(0, 0, NULL, VIDAS, 0, 1, 0, 0, -1);
			estado2ficheiro(e);
			break;
		}
		p[i] = d;
	}

	fclose(f);

	return e;
}

void estado2ficheiro(ESTADO e) {
	FILE *f;
	f = fopen(FICHEIRO_ESTADO, "w");
	if (f == NULL) {
		perror("Erro a escrever o ficheiro de estado");
		exit(1);
	}

	int *p = (int *) &e;
	unsigned int i;

	for(i = 0; i < (sizeof(ESTADO) / sizeof(int)); i++)
		fprintf(f, "%d\n", p[i]);

	fclose(f);
}

void aplicar_acao(char *acao, int x, int y) {
	ESTADO e = ficheiro2estado();

	if (strcmp(acao, "move_casa") == 0) {
		e = move_inimigos(e, x, y);
		e.jog.x = x;
		e.jog.y = y;
	}

	else if(strcmp(acao, "apanha_pocao") == 0){
		if (tem_inimigo(e, x, y)) {
			e.score_atual += 5;
			e = mata_inimigo(e, x, y);
		}
		e = move_inimigos(e, x, y);
		e.jog.x = x;
		e.jog.y = y;
		e.vidas++;
		e.pocao.x = -1;
		e.pocao.y = -1;
	}

	else if (strcmp(acao, "move_saida") == 0) {
		if (e.nivel < 10) {
			e.score_atual += 10;
			e.vidas += 3;
			e = inicializar(++e.nivel, e.score_atual, e.scores, e.vidas, e.inimigos_mortos, 0, e.hardcore, e.mostrar_casas_atacadas, -1);
		} else {
			e.score_atual += 10;
			e.score_atual += e.vidas * 2;
			e = atualiza_scores(e);
			e = inicializar(0, e.score_atual, e.scores, VIDAS, 0, 2, 0, 0, e.idx_ultimo_score);
		}
	}

	else if (strcmp(acao, "ataca_inimigo") == 0) {
		e.score_atual += 5;
		e = mata_inimigo(e, x, y);
		e = move_inimigos(e, x, y);
		e.jog.x = x;
		e.jog.y = y;
	}

	else if (strcmp(acao, "jogo_normal") == 0) {
		e = inicializar(0, 0, e.scores, VIDAS, 0, 0, 0, 0, -1);
	}

	else if (strcmp(acao, "hardcore") == 0) {
		e = inicializar(0, 0, e.scores, VIDAS, 0, 0, 1, 0, -1);
	}	

	else if (strcmp(acao, "menu") == 0) {
		e.idx_ultimo_score = -1;
		e.mostrar_ecra = 1;
	}

	else if (strcmp(acao, "melhores_scores") == 0) {
		e.mostrar_ecra = 2;
	}

	else if (strcmp(acao, "ajuda") == 0) {
		e.mostrar_ecra = 3;
	}

	else if (strcmp(acao, "reset") == 0) {
		e = inicializar(0, 0, NULL, VIDAS, 0, 1, 0, 0, -1);
	}

	else if (strcmp(acao, "alternar_casas_atacadas") == 0) {
		if (e.mostrar_casas_atacadas == 1)
			e.mostrar_casas_atacadas = 0;
		else
			e.mostrar_casas_atacadas = 1;
	}

	else {
		e.mostrar_ecra = 1;
	}

	if (e.vidas <= 0){
		e = atualiza_scores(e);
		e = inicializar(0, e.score_atual, e.scores, VIDAS, 0, 2, 0, 0, e.idx_ultimo_score);
	}

	estado2ficheiro(e);
}

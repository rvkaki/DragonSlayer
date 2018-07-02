#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cgi.h"
#include "estado.h"

/**
@file DragonSlayer.c
Código da lógica relacionada com o jogo e com a impressão no ecrã
*/

/** \brief Número de linhas e colunas */
#define TAM			15

/** \brief Número de píxeis por casa */
#define ESCALA		40

/**
\brief Função que verifica se uma posição está dentro do tabuleiro de jogo
@param x coluna
@param y linha
@returns 1 se sim, 0 se não
*/
int posicao_valida(int x, int y) {
	return x >= 0 && y >= 0 && x < TAM && y < TAM;
}

/**
\brief Função que verificam se uma posição é igual a duas coordenadas
@param p posição
@param x coluna
@param y linha
@returns 1 se sim, 0 se não
*/
int posicao_igual(POSICAO p, int x, int y) {
	return p.x == x && p.y == y;
}

/**
\brief Funções que verificam se a posição de um inimigo é igual a duas coordenadas
@param p posição do inimigo
@param x coluna
@param y linha
@returns 1 se sim, 0 se não
*/
int posicao_igual_inimigo(INIMIGO p, int x, int y) {
	return p.posicao.x == x && p.posicao.y == y;
}

/**
\brief Função que verifica se uma posição é adjacente a outra
@param p1 posição 1
@param p2 posição 2
@returns 1 se sim, 0 se não
*/
int posicao_adjacente(POSICAO p1, POSICAO p2){
	int dx, dy;
	for (dx = -1; dx <= 1; dx++)
		for (dy = -1; dy <= 1; dy++)
			if ((p1.x + dx == p2.x) && (p1.y + dy == p2.y))
				return 1;

	return 0;
}

/**
\brief Função que verifica se uma casa tem um inimigo
@param e estado
@param x coluna
@param y linha
@returns 1 se sim, 0 se não
*/
int tem_inimigo(ESTADO e, int x, int y) {
	int i;
	for (i = 0; i < e.num_inimigos; i++)
		if (posicao_igual(e.inimigo[i].posicao, x, y))
			return 1;

	return 0;
}

/**
\brief Função que testa se um dragão tem caminho livre para atacar
@param e estado
@param offset offset
@param p posição do dragão
@param x 1 se dx, 0 se dy
@returns 1 se sim, 0 se não
*/
int pathclear(ESTADO e, int offset, INIMIGO p, int x){
	if (offset > 0){
		for (offset = offset - 1; offset > 0; offset--)
			if (x) {
				if (tem_inimigo(e, p.posicao.x + offset, p.posicao.y))
					return 0;
			} else {
				if (tem_inimigo(e, p.posicao.x, p.posicao.y + offset))
					return 0;
			}
	} else {
		for (offset = offset + 1; offset < 0; offset++)
			if (x) {
				if (tem_inimigo(e, p.posicao.x + offset, p.posicao.y))
					return 0;
			} else {
				if (tem_inimigo(e, p.posicao.x, p.posicao.y + offset))
					return 0;
			}
	}

	return 1;
}

/**
\brief Função que verifica se uma posição está ao alcance do ataque de um dragão
@param e estado
@param p1 posição 1
@param p2 posição 2
@returns 1 se sim, 0 se não
*/
int alcance_dragao(ESTADO e, POSICAO p1, INIMIGO p2){
	int dx, dy;

	for (dx = -3; dx <= 3; dx++)
		if ((p1.x == p2.posicao.x + dx) && (p1.y == p2.posicao.y) && pathclear(e, dx, p2, 1))
			return 1;

	for (dy = -3; dy <= 3; dy++)
		if ((p1.x == p2.posicao.x) && (p1.y == p2.posicao.y + dy) && pathclear(e, dy, p2, 0))
			return 1;

	return 0;
}

/**
\brief Função que verifica se uma casa tem um obstáculo
@param e estado
@param x coluna
@param y linha
@returns 1 se válida, 0 se não
*/
int tem_obstaculo(ESTADO e, int x, int y) {
	int i;
	for (i = 0; i < e.num_obstaculos; i++)
		if (posicao_igual(e.obstaculo[i], x, y))
			return 1;

	return 0;
}

/**
\brief Função que verifica se uma casa tem a poção
@param e estado
@param x coluna
@param y linha
@returns 1 se válida, 0 se não
*/
int tem_pocao(ESTADO e, int x, int y) {
	return posicao_igual(e.pocao, x, y);

	return 0;
}

/**
\brief Função que verifica se uma casa tem o jogador
@param e estado
@param x coluna
@param y linha
@returns 1 se sim, 0 se não
*/
int tem_jogador(ESTADO e, int x, int y) {
	return posicao_igual(e.jog, x, y);
}

/**
\brief Função que verifica se uma casa tem a saída
@param e estado
@param x coluna 
@param y linha
@returns 1 se sim, 0 se não
*/
int tem_saida(ESTADO e, int x, int y) {
	return posicao_igual(e.saida, x, y);
}

/**
\brief Função que verifica se uma casa tem a entrada
@param e estado
@param x coluna 
@param y linha
@returns 1 se sim, 0 se não
*/
int tem_entrada(ESTADO e, int x, int y) {
	if (e.nivel == 0)
		return 0;
	else
		return posicao_igual(e.entrada, x, y);
}

/**
\brief Função que verifica se uma posição está ocupada
@param e estado
@param x coluna
@param y linha
@returns 1 se sim, 0 se não
*/
int posicao_ocupada(ESTADO e, int x, int y) {
	if (tem_inimigo(e, x, y))
		return 1;
	if (tem_obstaculo(e, x, y))
		return 1;
	if (tem_jogador(e, x, y))
		return 1;
	if (tem_saida(e, x, y))
		return 1;
	if (tem_entrada(e, x, y))
		return 1;

	return 0;
}

/**
\brief Função que define a posição inicial do jogador
@param e estado
@returns estado alterado
*/
ESTADO inicializar_jogador(ESTADO e) {
	if (e.nivel == 0) {
		e.jog.x = 0;
		e.jog.y = 0;
	} else {
		e.jog.x = 1;
		e.jog.y = 0;
	}

	return e;
}

/**
\brief Função que define a posição de um inimigo
@param e estado
@returns estado alterado
*/
ESTADO inicializar_inimigo(ESTADO e) {
	int x, y;
	do {
		x = random() % TAM;
		y = random() % TAM;
	} while (posicao_ocupada(e, x, y) || (x < 3 && y < 2));

	e.inimigo[e.num_inimigos].posicao.x = x;
	e.inimigo[e.num_inimigos].posicao.y = y;
	if(e.num_inimigos < e.nivel -4)
		e.inimigo[e.num_inimigos].spriggan = 0;
	else 
		e.inimigo[e.num_inimigos].spriggan = 1;
	e.num_inimigos++;

	return e;
}

/**
\brief Função que define a posição de todos os inimigos
@param e estado
@param num número de inimigos
@returns estado alterado
*/
ESTADO inicializar_inimigos(ESTADO e, int num) {
	e.num_inimigos = 0;

	int i;
	for (i = 0; i < num; i++)
		e = inicializar_inimigo(e);

	return e;
}

/**
\brief Função que define a posição de um obstáculo
@param e estado
@returns estado alterado
*/
ESTADO inicializar_obstaculo(ESTADO e) {
	int x, y;
	do {
		x = random() % TAM;
		y = random() % TAM;
	} while (posicao_ocupada(e, x, y) || (x < 3 && y < 2));

	e.obstaculo[e.num_obstaculos].x = x;
	e.obstaculo[e.num_obstaculos].y = y;
	e.num_obstaculos++;

	return e;
}

/**
\brief Função que define a posição de todos os obstáculos
@param e estado
@param num número de inimigos
@returns estado alterado
*/
ESTADO inicializar_obstaculos(ESTADO e, int num) {
	e.num_obstaculos = 0;

	int i;
	for (i = 0; i < num; i++)
		e = inicializar_obstaculo(e);

	return e;
}

/**
\brief Função que define a posição de uma poção
@param e estado
@returns estado alterado
*/
ESTADO inicializar_pocao(ESTADO e) {
	int x, y;
	do {
		x = random() % TAM;
		y = random() % TAM;
	} while (posicao_ocupada(e, x, y) || (x < 3 && y < 2));

	e.pocao.x = x;
	e.pocao.y = y;

	return e;
}

/**
\brief Função que inicializa os scores
@param e estado
@param *scores array de scores
@returns estado alterado
*/
ESTADO inicializar_scores(ESTADO e, int *scores) {
	if (scores == NULL) return e;

	int i;
	for (i = 0; i < NUM_SCORES; i++)
		e.scores[i] = scores[i];

	return e;
}

/**
\brief Função que cria um nível
@param nivel nível
@param score score
@param scores array de scores
@param vidas vidas
@param inimigos_mortos inimigos mortos
@param mostrar_ecra 0 tabuleiro, 1 menu, 2 melhores scores, 3 ajuda
@param mostrar_casas_atacadas 1 sim, 0 não
@param idx_ultimo_score índice do último score
@returns estado alterado
*/
ESTADO inicializar(int nivel, int score, int *scores, int vidas, int inimigos_mortos, int mostrar_ecra, int hardcore, int mostrar_casas_atacadas, int idx_ultimo_score) {
	ESTADO e;
	memset(&e, 0, sizeof(ESTADO));
	e.nivel = nivel;
	e.score_atual = score;
	e.vidas = vidas;
	e.inimigos_mortos = inimigos_mortos;
	e.mostrar_ecra = mostrar_ecra;
	e.hardcore = hardcore;
	e.mostrar_casas_atacadas = mostrar_casas_atacadas;
	e.idx_ultimo_score = idx_ultimo_score;
	e.entrada.x = 0;
	e.entrada.y = 0;
	e.saida.x = TAM-1;
	e.saida.y = TAM-1;
	e = inicializar_jogador(e);
	e = inicializar_inimigos(e, 10 + e.nivel*2);
	e = inicializar_obstaculos(e, 20);
	e = inicializar_pocao(e);
	e = inicializar_scores(e, scores);

	return e;
}

/**
\brief Função que processa o URL
@param *args URL
@returns estado
*/
ESTADO ler_estado(char *args) {
	ESTADO e;
	char acao[32];
	int x = 0, y = 0, lidos;
	lidos = sscanf(args, "%[^,],%d,%d", acao, &x, &y);

	if (lidos >= 1)
		aplicar_acao(acao, x, y);
	else
		aplicar_acao("menu", x, y);

	e = ficheiro2estado();

	return e;
}

/**
\brief Função que imprime o tabuleiro de jogo
*/
void imprime_tabuleiro() {
	int x, y;
	for(y = 0; y < TAM; y++)
		for(x = 0; x < TAM; x++)
			IMAGEM(x, y, ESCALA, "rect_gray1.png");
}

/**
\brief Função que imprime a entrada
@param e estado
*/
void imprime_entrada(ESTADO e) {
	if (e.nivel > 0)
		IMAGEM(e.entrada.x, e.entrada.y, ESCALA, "stone_stairs_up.png");
}

/**
\brief Função que imprime a saída
@param e estado
*/
void imprime_saida(ESTADO e) {
	IMAGEM(e.saida.x, e.saida.y, ESCALA, "stone_stairs_down.png");
}

/**
\brief Função que imprime uma casa transparente
@param x coluna
@param y linha
*/
void imprime_casa_transparente(int x, int y) {
	QUADRADO_TRANSPARENTE(x, y, ESCALA);
}

/**
\brief Função que imprime uma ação do jogador
@param e estado
@param dx coluna da ação em relação ao jogador
@param dy linha da ação em relação ao jogador
*/
void imprime_acao(ESTADO e, int dx, int dy) {
	int x = e.jog.x + dx;
	int y = e.jog.y + dy;
	char link[2048];
	char acao[32];

	if (!posicao_valida(x, y) || tem_obstaculo(e, x, y) || tem_jogador(e, x, y) || tem_entrada(e, x, y))
		return;

	if (tem_pocao(e, x, y)) {
		strcpy(acao, "apanha_pocao");
	}

	else if (tem_inimigo(e, x, y)) {
		strcpy(acao, "ataca_inimigo");
	}

	else if (tem_saida(e, x, y)) {
		strcpy(acao, "move_saida");
	}

	else {
		strcpy(acao, "move_casa");
	}

	sprintf(link, "http://localhost/cgi-bin/DragonSlayer?%s,%d,%d", acao, x, y);
	ABRIR_LINK(link);
	imprime_casa_transparente(x, y);
	FECHAR_LINK;
}

/**
\brief Função que imprime as ações do jogador
@param e estado
*/
void imprime_acoes(ESTADO e) {
	int dx, dy;
	for (dx = -1; dx <= 1; dx++)
		for (dy = -1; dy <= 1; dy++)
			imprime_acao(e, dx, dy);
}

/**
\brief Função que imprime o jogador
@param e estado
*/
void imprime_jogador(ESTADO e) {
	IMAGEM(e.jog.x, e.jog.y, ESCALA, "demigod_m.png");
	IMAGEM(e.jog.x, e.jog.y, ESCALA, "leg_armor00.png");
	IMAGEM(e.jog.x, e.jog.y, ESCALA, "dragonarm_brown.png");
	IMAGEM(e.jog.x, e.jog.y, ESCALA, "heavy_sword.png");
	imprime_acoes(e);
}

/**
\brief Função que imprime os inimigos
@param e estado
*/
void imprime_inimigos(ESTADO e) {
	int i;
	for(i = 0; i < e.num_inimigos; i++)
		if(e.inimigo[i].spriggan)
			IMAGEM(e.inimigo[i].posicao.x, e.inimigo[i].posicao.y, ESCALA, "spriggan.png");
		else
			IMAGEM(e.inimigo[i].posicao.x, e.inimigo[i].posicao.y, ESCALA, "draco-base-red.png");
}

/**
\brief Função que imprime as casas atacadas
@param e estado
*/
void imprime_casas_atacadas(ESTADO e) {
	if (e.mostrar_casas_atacadas == 0) {
		ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?alternar_casas_atacadas");
		TEXTO((TAM+1)*ESCALA, (TAM-1)*ESCALA, "#000000", "Mostrar casas atacadas");
		FECHAR_LINK;
	} else {
		ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?alternar_casas_atacadas");
		TEXTO((TAM+1)*ESCALA, (TAM-1)*ESCALA, "#000000", "Ocultar casas atacadas");
		FECHAR_LINK;

		int x, y, k;
		for (y = 0; y < TAM; y++) {
			for (x = 0; x < TAM; x++) {
				POSICAO casa = {x, y};
				for (k = 0; k < e.num_inimigos; k++) {
					INIMIGO p2 = e.inimigo[k];
					if (((p2.spriggan && posicao_adjacente(casa, p2.posicao)) || (!p2.spriggan && alcance_dragao(e, casa, p2))) && !posicao_igual(p2.posicao, casa.x, casa.y) && !tem_obstaculo(e, x, y) && !tem_saida(e, x, y) && !tem_entrada(e, x, y)) {
						QUADRADO_VERMELHO(x, y, ESCALA);
					}
				}
			}
		}
	}
}

/**
\brief Função que imprime os obstáculos
@param e estado
*/
void imprime_obstaculos(ESTADO e) {
	int i;
	for(i = 0; i < e.num_obstaculos; i++)
		IMAGEM(e.obstaculo[i].x, e.obstaculo[i].y, ESCALA, "lava1.png");
}

/** 
\brief Função que imprime a poção
@param e estado
*/
void imprime_pocao(ESTADO e) {
	if (e.pocao.x != -1 && e.pocao.y != -1)
		IMAGEM(e.pocao.x, e.pocao.y, ESCALA, "ruby.png");
}

/**
\brief Função que imprime o score atual
@param score_atual o score atual
*/
void imprime_score(int score_atual) {
	char s1[1000];
	sprintf(s1, "Score: %d", score_atual);
	TEXTO((TAM+1)*ESCALA, 20, "#000000", s1);
}

/**
\brief Função que imprime o nível atual
@param nivel nível atual
*/
void imprime_nivel(int nivel) {
	char s1[1000];
	sprintf(s1, "Nivel: %d", nivel);
	TEXTO((TAM+1)*ESCALA, 60, "#000000", s1);	
}

/**
\brief Função que imprime o número de inimigos mortos
@param inimigos_mortos número de inimigos mortos
*/
void imprime_inimigos_mortos(int inimigos_mortos) {
	char s1[1000];
	sprintf(s1, "Inimigos mortos: %d", inimigos_mortos);
	TEXTO((TAM+1)*ESCALA, 100, "#000000", s1);
}

/**
\brief Função que imprime o número de vidas
@param vidas número de vidas
*/
void imprime_vidas(int vidas) {
	if (vidas >= 40){
		char s1[1000];
		sprintf(s1, "Vidas: %d", vidas);
		TEXTO(0, (TAM+1)*ESCALA, "#000000", s1);
	} else {
		TEXTO(0, (TAM+1)*ESCALA, "#000000", "Vidas:");
		for(int i = 0; i<vidas; i++)
			printf("<image x=%d y=%d width=%d height=%d xlink:href=%s />\n", 60 + i*20, 625, 20, 20, "http://localhost/Images/Heart.png");
	}	
}

/**
\brief Função que imprime o menu
*/
void imprime_menu() {
	IMAGEM(0, 0, TAM*ESCALA, "MenuBackground.jpg");

	ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?jogo_normal");
	TEXTO((TAM-10)*ESCALA, (TAM/2 - 1)*ESCALA, "#00ff00", "Jogo Normal");
	FECHAR_LINK;

	ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?hardcore");
	TEXTO((TAM-10)*ESCALA, (TAM/2)*ESCALA, "#ff0000", "Jogo HARDCORE");
	FECHAR_LINK;

	ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?ajuda");
	TEXTO((TAM-10)*ESCALA, (TAM/2 + 1)*ESCALA, "#ffffff", "Ajuda");
	FECHAR_LINK;

	ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?melhores_scores");
	TEXTO((TAM-10)*ESCALA, (TAM/2 + 2)*ESCALA, "#ffffff", "Melhores Scores");
	FECHAR_LINK;
}

/**
\brief Função que imprime o botão de regresso ao menu
*/
void imprime_regressar_menu() {
	ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?menu");
	TEXTO((TAM-1)*ESCALA, 3*ESCALA, "#ffffff", "X");
	FECHAR_LINK;
}

/**
\brief Função que imprime o botão de regresso ao menu durante o jogo
*/
void imprime_regressar_menu_jogo() {
	ABRIR_LINK("http://localhost/cgi-bin/DragonSlayer?menu");
	TEXTO((TAM+1)*ESCALA, TAM*ESCALA, "#0000ff", "Regressar ao menu");
	FECHAR_LINK;
}

/**
\brief Função que imprime os melhores scores
@param e estado
*/
void imprime_melhores_scores(ESTADO e) {
	char s1[1000];
	int i = 0;

	IMAGEM(0, 0, TAM*ESCALA, "MenuBackground.jpg");

	while (i < NUM_SCORES){
		if (i == e.idx_ultimo_score) {
			sprintf(s1, "%d. %d", i + 1, e.scores[i]);
			TEXTO(6*ESCALA, (5 + i)*ESCALA, "#00ff00", s1);
		} else {
			sprintf(s1, "%d. %d", i + 1, e.scores[i]);
			TEXTO(6*ESCALA, (5 + i)*ESCALA, "#ffffff", s1);
		}
		i++;
	}

	if (e.idx_ultimo_score == -2) {
		sprintf(s1, "--. %d", e.score_atual);
		TEXTO(6*ESCALA, 11*ESCALA, "#00ff00", s1);
	}


	imprime_regressar_menu();
}

/**
\brief Função que imprime a página de ajuda
*/
void imprime_ajuda() {
	IMAGEM(0, 0, TAM*ESCALA, "MenuBackground.jpg");

	TEXTO(ESCALA, 3*ESCALA, "#ffffff", "Bem-vindo ao DragonSlayer, o melhor jogo de sempre!");
	TEXTO(ESCALA, 4*ESCALA, "#ffffff", "Comecas com 5 vidas e ganhas 3 vidas por nivel.");
	TEXTO(ESCALA, 5*ESCALA, "#ffffff", "O objetivo e ultrapassares os 11 niveis.");
	TEXTO(ESCALA, 6*ESCALA, "#ffffff", "Pontuacao:");
	TEXTO(2*ESCALA, 7*ESCALA, "#ffffff", "5 pontos por inimigo morto;");
	TEXTO(2*ESCALA, 8*ESCALA, "#ffffff", "10 pontos por nivel concluido.");
	TEXTO(ESCALA, 9*ESCALA, "#ffffff", "A partir do nivel 5 esperam-te surpresas.");
	TEXTO(ESCALA, 10*ESCALA, "#ffffff", "Boa sorte!");

	imprime_regressar_menu();
}

/**
\brief Função que imprime um estado
@param e estado
*/
void imprime_estado(ESTADO e) {
	if (e.mostrar_ecra == 0) {
		imprime_tabuleiro();
		imprime_casas_atacadas(e);
		imprime_pocao(e);
		imprime_inimigos(e);
		imprime_obstaculos(e);
		imprime_entrada(e);
		imprime_saida(e);
		imprime_jogador(e);
		imprime_score(e.score_atual);
		imprime_vidas(e.vidas);
		imprime_nivel(e.nivel);
		imprime_inimigos_mortos(e.inimigos_mortos);
		imprime_regressar_menu_jogo();
	}

	else if (e.mostrar_ecra == 1) {
		imprime_menu();
	}

	else if (e.mostrar_ecra == 2) {
		imprime_melhores_scores(e);
	}

	else if (e.mostrar_ecra == 3) {
		imprime_ajuda();
	}
}

#define sign(x) (x > 0) ? 1 : ((x < 0) ? -1 : 0)

/**
\brief Função que move todos os inimigos
@param e estado
@param novojogx novo x do jogador
@param novojogy novo y do jogador
@returns estado alterado
*/
ESTADO move_inimigos(ESTADO e, int novojogx, int novojogy) {
	int i, x, y, dx, dy;
	POSICAO novojog = {novojogx, novojogy};

	for (i = 0; i < e.num_inimigos; i++){
		if ((e.inimigo[i].spriggan && posicao_adjacente(novojog, e.inimigo[i].posicao)) || (!e.inimigo[i].spriggan && alcance_dragao(e, novojog, e.inimigo[i]))) {
			e.vidas--;
		} else {
			if (e.hardcore){
				dx = novojogx - e.inimigo[i].posicao.x;
				dy = novojogy - e.inimigo[i].posicao.y;
			} else {
				dx = e.jog.x - e.inimigo[i].posicao.x;
				dy = e.jog.y - e.inimigo[i].posicao.y;
			}
			dx = sign(dx);
			dy = sign(dy);
			x = e.inimigo[i].posicao.x + dx;
			y = e.inimigo[i].posicao.y + dy;
			if (!posicao_ocupada(e, x, y) && (x != novojogx || y != novojogy)){
				e.inimigo[i].posicao.x = x;
				e.inimigo[i].posicao.y = y;
			}
		}
	}

	return e;
}

/**
\brief Função onde inicia o programa
*/
int main() {
	srandom(time(NULL));

	ESTADO e = ler_estado(getenv("QUERY_STRING"));

	COMECAR_HTML;
	ABRIR_SVG((TAM+10)*ESCALA, (TAM+2)*ESCALA);

	imprime_estado(e);

	FECHAR_SVG;

	return 0;
}

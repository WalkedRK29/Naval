#include <stdio.h>

#define BOARD_SIZE 10   // tamanho do tabuleiro 10x10
#define SHIP_SIZE 3     // tamanho fixo dos navios
#define SKILL_SIZE 5    // tamanho das matrizes de habilidade (5x5)

// valores do tabuleiro
#define WATER 0         // água
#define SHIP  3         // parte do navio
#define SKILL 5         // área afetada pela habilidade

// orientações dos navios
#define HORIZONTAL           0
#define VERTICAL             1
#define DIAGONAL_DOWN_RIGHT  2
#define DIAGONAL_DOWN_LEFT   3

// Preenche o tabuleiro todo com água (0)
void inicializarTabuleiro(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = WATER;
        }
    }
}

// Imprime o tabuleiro na tela
void imprimirTabuleiro(int board[BOARD_SIZE][BOARD_SIZE]) {
    printf("Legenda: 0 = agua, 3 = navio, 5 = habilidade\n\n");

    // cabeçalho de colunas
    printf("   ");
    for (int j = 0; j < BOARD_SIZE; j++) {
        printf("%2d ", j);
    }
    printf("\n");

    // linhas
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", i); // índice da linha
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%2d ", board[i][j]);
        }
        printf("\n");
    }
}

// Confere se dá pra colocar o navio nas coordenadas passadas
int podePosicionarNavio(int board[BOARD_SIZE][BOARD_SIZE],
                        int linhaInicial,
                        int colunaInicial,
                        int orientacao,
                        int tamanhoNavio) {
    if (orientacao == HORIZONTAL) {
        if (linhaInicial < 0 || linhaInicial >= BOARD_SIZE) return 0;
        if (colunaInicial < 0 || colunaInicial + tamanhoNavio > BOARD_SIZE) return 0;

        for (int i = 0; i < tamanhoNavio; i++) {
            if (board[linhaInicial][colunaInicial + i] != WATER) {
                return 0;
            }
        }
    } else if (orientacao == VERTICAL) {
        if (colunaInicial < 0 || colunaInicial >= BOARD_SIZE) return 0;
        if (linhaInicial < 0 || linhaInicial + tamanhoNavio > BOARD_SIZE) return 0;

        for (int i = 0; i < tamanhoNavio; i++) {
            if (board[linhaInicial + i][colunaInicial] != WATER) {
                return 0;
            }
        }
    } else if (orientacao == DIAGONAL_DOWN_RIGHT) {
        if (linhaInicial < 0 || linhaInicial + tamanhoNavio > BOARD_SIZE) return 0;
        if (colunaInicial < 0 || colunaInicial + tamanhoNavio > BOARD_SIZE) return 0;

        for (int i = 0; i < tamanhoNavio; i++) {
            if (board[linhaInicial + i][colunaInicial + i] != WATER) {
                return 0;
            }
        }
    } else if (orientacao == DIAGONAL_DOWN_LEFT) {
        if (linhaInicial < 0 || linhaInicial + tamanhoNavio > BOARD_SIZE) return 0;
        if (colunaInicial - (tamanhoNavio - 1) < 0 || colunaInicial >= BOARD_SIZE) return 0;

        for (int i = 0; i < tamanhoNavio; i++) {
            if (board[linhaInicial + i][colunaInicial - i] != WATER) {
                return 0;
            }
        }
    } else {
        return 0;
    }

    return 1;
}

// Coloca o navio no tabuleiro de fato
void posicionarNavio(int board[BOARD_SIZE][BOARD_SIZE],
                     int linhaInicial,
                     int colunaInicial,
                     int orientacao,
                     int navio[SHIP_SIZE],
                     int tamanhoNavio) {
    if (orientacao == HORIZONTAL) {
        for (int i = 0; i < tamanhoNavio; i++) {
            board[linhaInicial][colunaInicial + i] = navio[i];
        }
    } else if (orientacao == VERTICAL) {
        for (int i = 0; i < tamanhoNavio; i++) {
            board[linhaInicial + i][colunaInicial] = navio[i];
        }
    } else if (orientacao == DIAGONAL_DOWN_RIGHT) {
        for (int i = 0; i < tamanhoNavio; i++) {
            board[linhaInicial + i][colunaInicial + i] = navio[i];
        }
    } else if (orientacao == DIAGONAL_DOWN_LEFT) {
        for (int i = 0; i < tamanhoNavio; i++) {
            board[linhaInicial + i][colunaInicial - i] = navio[i];
        }
    }
}

// Cone apontando pra baixo (parte de cima da matriz)
void construirMatrizCone(int cone[SKILL_SIZE][SKILL_SIZE]) {
    int centro = SKILL_SIZE / 2; // em 5, é 2

    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {

            // linhas de cima vão abrindo a partir do centro
            if (i <= centro && (j >= centro - i) && (j <= centro + i)) {
                cone[i][j] = 1;
            } else {
                cone[i][j] = 0;
            }
        }
    }
}

// Cruz: linha do meio + coluna do meio
void construirMatrizCruz(int cruz[SKILL_SIZE][SKILL_SIZE]) {
    int centro = SKILL_SIZE / 2;

    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {
            if (i == centro || j == centro) {
                cruz[i][j] = 1;
            } else {
                cruz[i][j] = 0;
            }
        }
    }
}

// Octaedro visto de frente (fica um losango)
void construirMatrizOctaedro(int oct[SKILL_SIZE][SKILL_SIZE]) {
    int centro = SKILL_SIZE / 2;
    int raio = centro;

    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {
            int distLinha = i - centro;
            if (distLinha < 0) distLinha = -distLinha;

            int distCol = j - centro;
            if (distCol < 0) distCol = -distCol;

            if (distLinha + distCol <= raio) {
                oct[i][j] = 1;
            } else {
                oct[i][j] = 0;
            }
        }
    }
}

// Joga a matriz de habilidade (0/1) por cima do tabuleiro
void aplicarHabilidadeNoTabuleiro(int board[BOARD_SIZE][BOARD_SIZE],
                                  int habilidade[SKILL_SIZE][SKILL_SIZE],
                                  int linhaOrigem,
                                  int colunaOrigem) {
    int centro = SKILL_SIZE / 2;

    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {
            if (habilidade[i][j] == 1) {
                int linhaTab = linhaOrigem + (i - centro);
                int colunaTab = colunaOrigem + (j - centro);

                // garante que não sai do tabuleiro
                if (linhaTab >= 0 && linhaTab < BOARD_SIZE &&
                    colunaTab >= 0 && colunaTab < BOARD_SIZE) {

                    // aqui decidi marcar skill só em água pra não “apagar” navio
                    if (board[linhaTab][colunaTab] == WATER) {
                        board[linhaTab][colunaTab] = SKILL;
                    }
                }
            }
        }
    }
}

int main() {
    // tabuleiro principal
    int tabuleiro[BOARD_SIZE][BOARD_SIZE];

    // quatro navios simples de tamanho 3
    int navio1[SHIP_SIZE] = {SHIP, SHIP, SHIP};
    int navio2[SHIP_SIZE] = {SHIP, SHIP, SHIP};
    int navio3[SHIP_SIZE] = {SHIP, SHIP, SHIP};
    int navio4[SHIP_SIZE] = {SHIP, SHIP, SHIP};

    // matrizes das três habilidades
    int habilidadeCone[SKILL_SIZE][SKILL_SIZE];
    int habilidadeCruz[SKILL_SIZE][SKILL_SIZE];
    int habilidadeOctaedro[SKILL_SIZE][SKILL_SIZE];

    inicializarTabuleiro(tabuleiro);

    // ---------- navios (mesmo esquema do nível anterior) ----------

    // Navio 1: horizontal, (0,0) (0,1) (0,2)
    int linhaNavio1 = 0, colunaNavio1 = 0, orientacaoNavio1 = HORIZONTAL;

    // Navio 2: vertical, (0,4) (1,4) (2,4)
    int linhaNavio2 = 0, colunaNavio2 = 4, orientacaoNavio2 = VERTICAL;

    // Navio 3: diagonal descendo pra direita, (4,0) (5,1) (6,2)
    int linhaNavio3 = 4, colunaNavio3 = 0, orientacaoNavio3 = DIAGONAL_DOWN_RIGHT;

    // Navio 4: diagonal descendo pra esquerda, (4,9) (5,8) (6,7)
    int linhaNavio4 = 4, colunaNavio4 = 9, orientacaoNavio4 = DIAGONAL_DOWN_LEFT;

    if (podePosicionarNavio(tabuleiro, linhaNavio1, colunaNavio1, orientacaoNavio1, SHIP_SIZE)) {
        posicionarNavio(tabuleiro, linhaNavio1, colunaNavio1, orientacaoNavio1, navio1, SHIP_SIZE);
    }

    if (podePosicionarNavio(tabuleiro, linhaNavio2, colunaNavio2, orientacaoNavio2, SHIP_SIZE)) {
        posicionarNavio(tabuleiro, linhaNavio2, colunaNavio2, orientacaoNavio2, navio2, SHIP_SIZE);
    }

    if (podePosicionarNavio(tabuleiro, linhaNavio3, colunaNavio3, orientacaoNavio3, SHIP_SIZE)) {
        posicionarNavio(tabuleiro, linhaNavio3, colunaNavio3, orientacaoNavio3, navio3, SHIP_SIZE);
    }

    if (podePosicionarNavio(tabuleiro, linhaNavio4, colunaNavio4, orientacaoNavio4, SHIP_SIZE)) {
        posicionarNavio(tabuleiro, linhaNavio4, colunaNavio4, orientacaoNavio4, navio4, SHIP_SIZE);
    }

    construirMatrizCone(habilidadeCone);
    construirMatrizCruz(habilidadeCruz);
    construirMatrizOctaedro(habilidadeOctaedro);

    // aqui você pode mexer nos pontos de origem pra ver como muda o desenho
    int origemConeLinha = 2;
    int origemConeColuna = 7;

    int origemCruzLinha = 7;
    int origemCruzColuna = 4;

    int origemOctLinha = 5;
    int origemOctColuna = 5;

    aplicarHabilidadeNoTabuleiro(tabuleiro, habilidadeCone, origemConeLinha, origemConeColuna);
    aplicarHabilidadeNoTabuleiro(tabuleiro, habilidadeCruz, origemCruzLinha, origemCruzColuna);
    aplicarHabilidadeNoTabuleiro(tabuleiro, habilidadeOctaedro, origemOctLinha, origemOctColuna);

    printf("Tabuleiro com navios e areas de habilidades:\n\n");
    imprimirTabuleiro(tabuleiro);

    return 0;
}

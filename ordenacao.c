/*
 *  Trabalho de AED - Comparacao do tempo medio de algoritmos de ordenacao
 *
 *  Aluno: João Maerbal Nunes Barros
 *  RA:    202510288
 *  Video: https://www.youtube.com/watch?v=3vfH9RDy3Oo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define NUM_ALGS 5   /* algoritmos comparados */
#define NUM_TAMS 4   /* tamanhos de vetor     */
#define NUM_RUNS 5   /* execucoes (RUNs)      */

int tamanhos[NUM_TAMS] = {10000, 30000, 60000, 90000};
char *nomes[NUM_ALGS] = {"QuickSort", "BubbleSort", "InsertionSort",
                         "SelectionSort", "MergeSort"};

/* tempos[run][algoritmo][tamanho] */
double tempos[NUM_RUNS][NUM_ALGS][NUM_TAMS];
double medias[NUM_ALGS][NUM_TAMS];
double logs[NUM_ALGS][NUM_TAMS];

/* Troca o conteudo de duas posicoes */
void troca(int *a, int *b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}

/* BubbleSort - O(n^2)
 * Compara vizinhos e troca os que estao fora de ordem; a cada passada
 * o maior elemento "borbulha" para o final. Se uma passada nao fizer
 * nenhuma troca, o vetor ja esta ordenado. */
void bubble_sort(int v[], int n)
{
    for (int i = 0; i < n - 1; i++) {
        int trocou = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (v[j] > v[j + 1]) {
                troca(&v[j], &v[j + 1]);
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

/* InsertionSort - O(n^2)
 * O inicio do vetor fica ordenado; cada novo elemento (chave) e
 * deslocado para a esquerda ate encontrar a sua posicao. */
void insertion_sort(int v[], int n)
{
    for (int i = 1; i < n; i++) {
        int chave = v[i];
        int j = i - 1;
        while (j >= 0 && v[j] > chave) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}

/* SelectionSort - O(n^2)
 * A cada passo procura o menor elemento da parte nao ordenada e o
 * coloca na proxima posicao. */
void selection_sort(int v[], int n)
{
    for (int i = 0; i < n - 1; i++) {
        int menor = i;
        for (int j = i + 1; j < n; j++) {
            if (v[j] < v[menor]) menor = j;
        }
        if (menor != i) troca(&v[i], &v[menor]);
    }
}

/* QuickSort - O(n log n) no caso medio
 * Usa o elemento do meio como pivo, deixa os menores a esquerda e os
 * maiores a direita, e ordena cada lado recursivamente. */
void quick_sort(int v[], int ini, int fim)
{
    if (ini >= fim) return;

    int pivo = v[(ini + fim) / 2];
    int i = ini, j = fim;

    while (i <= j) {
        while (v[i] < pivo) i++;   /* acha elemento >= pivo na esquerda */
        while (v[j] > pivo) j--;   /* acha elemento <= pivo na direita  */
        if (i <= j) {
            troca(&v[i], &v[j]);
            i++;
            j--;
        }
    }
    quick_sort(v, ini, j);   /* lado esquerdo */
    quick_sort(v, i, fim);   /* lado direito  */
}

/* Junta as metades ja ordenadas v[ini..meio] e v[meio+1..fim] no vetor
 * auxiliar e copia o resultado de volta para v. */
void intercala(int v[], int aux[], int ini, int meio, int fim)
{
    int i = ini, j = meio + 1, k = ini;

    while (i <= meio && j <= fim) {
        if (v[i] <= v[j]) aux[k++] = v[i++];
        else              aux[k++] = v[j++];
    }
    while (i <= meio) aux[k++] = v[i++];   /* sobras da esquerda */
    while (j <= fim)  aux[k++] = v[j++];   /* sobras da direita  */

    for (k = ini; k <= fim; k++) v[k] = aux[k];
}

/* MergeSort - O(n log n) em todos os casos
 * Divide o vetor ao meio, ordena cada metade e depois intercala. */
void merge_sort(int v[], int aux[], int ini, int fim)
{
    if (ini >= fim) return;
    int meio = (ini + fim) / 2;
    merge_sort(v, aux, ini, meio);
    merge_sort(v, aux, meio + 1, fim);
    intercala(v, aux, ini, meio, fim);
}

/* Chama o algoritmo de numero alg (mesma ordem do vetor nomes) */
void ordenar(int alg, int v[], int n)
{
    if (alg == 0) quick_sort(v, 0, n - 1);
    else if (alg == 1) bubble_sort(v, n);
    else if (alg == 2) insertion_sort(v, n);
    else if (alg == 3) selection_sort(v, n);
    else {
        int *aux = (int *) malloc(n * sizeof(int));
        merge_sort(v, aux, 0, n - 1);
        free(aux);
    }
}

/* Confere se o vetor ficou em ordem crescente */
int esta_ordenado(int v[], int n)
{
    for (int i = 1; i < n; i++)
        if (v[i - 1] > v[i]) return 0;
    return 1;
}

/* Mede o tempo de um algoritmo sobre uma copia do vetor original, para
 * todos os algoritmos ordenarem os mesmos numeros.
 * Mesma conta do modelo do professor (unidade de 0,1 ms). */
double medir(int alg, int original[], int n)
{
    int *vet = (int *) malloc(n * sizeof(int));
    memcpy(vet, original, n * sizeof(int));

    clock_t begin = clock();
    ordenar(alg, vet, n);
    clock_t end = clock();

    if (!esta_ordenado(vet, n))
        printf("ERRO: %s nao ordenou o vetor!\n", nomes[alg]);

    free(vet);
    return (double)(end - begin) * 10000.0 / CLOCKS_PER_SEC;
}

/* Imprime uma tabela: uma linha por algoritmo, uma coluna por tamanho */
void imprimir_tabela(char *titulo, double t[NUM_ALGS][NUM_TAMS])
{
    printf("\n%s\n%-15s", titulo, "");
    for (int k = 0; k < NUM_TAMS; k++) printf("%14d", tamanhos[k]);
    printf("\n");
    for (int a = 0; a < NUM_ALGS; a++) {
        printf("%-15s", nomes[a]);
        for (int k = 0; k < NUM_TAMS; k++) printf("%14.4f", t[a][k]);
        printf("\n");
    }
}

/* Escreve a mesma tabela no CSV. Usa ';' e troca o ponto por virgula
 * para a planilha em portugues reconhecer os numeros. */
void salvar_tabela(FILE *f, char *titulo, double t[NUM_ALGS][NUM_TAMS])
{
    fprintf(f, "%s", titulo);
    for (int k = 0; k < NUM_TAMS; k++) fprintf(f, ";%d", tamanhos[k]);
    fprintf(f, "\n");
    for (int a = 0; a < NUM_ALGS; a++) {
        fprintf(f, "%s", nomes[a]);
        for (int k = 0; k < NUM_TAMS; k++) {
            char num[32];
            sprintf(num, "%.4f", t[a][k]);
            for (int c = 0; num[c] != '\0'; c++)
                if (num[c] == '.') num[c] = ',';
            fprintf(f, ";%s", num);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
}

/* Posicao (em pixels) de um ponto no grafico.
 * Eixo X: um algoritmo a cada 136 px. Eixo Y: log de 0 a 6 em 380 px
 * (o y cresce para baixo no SVG, por isso 440 - ...). */
double pos_x(int a)        { return 148 + a * 136; }
double pos_y(double valor) { return 440 - valor * 380 / 6; }

/* Gera grafico.svg (abre no navegador): algoritmos no eixo X, log da
 * media no eixo Y e uma linha para cada tamanho, como no modelo. */
void gerar_grafico()
{
    char *cores[NUM_TAMS] = {"blue", "orange", "gray", "gold"};
    FILE *f = fopen("grafico.svg", "w");

    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' width='800' height='500' "
               "font-family='Arial' font-size='14'>\n");
    fprintf(f, "<rect width='800' height='500' fill='white'/>\n");
    fprintf(f, "<text x='20' y='250' transform='rotate(-90 20 250)'>Log( Time )</text>\n");

    /* Linhas de grade e numeros do eixo Y: 0, 1, ..., 6 */
    for (int y = 0; y <= 6; y++) {
        fprintf(f, "<line x1='80' y1='%.0f' x2='760' y2='%.0f' stroke='#ddd'/>\n",
                pos_y(y), pos_y(y));
        fprintf(f, "<text x='60' y='%.0f'>%d</text>\n", pos_y(y) + 5, y);
    }

    /* Nomes dos algoritmos no eixo X */
    for (int a = 0; a < NUM_ALGS; a++)
        fprintf(f, "<text x='%.0f' y='470' text-anchor='middle'>%s</text>\n",
                pos_x(a), nomes[a]);

    /* Uma linha por tamanho, ligando os 5 algoritmos, e a legenda */
    for (int k = 0; k < NUM_TAMS; k++) {
        fprintf(f, "<polyline fill='none' stroke='%s' stroke-width='3' points='", cores[k]);
        for (int a = 0; a < NUM_ALGS; a++)
            fprintf(f, "%.0f,%.0f ", pos_x(a), pos_y(logs[a][k]));
        fprintf(f, "'/>\n");

        fprintf(f, "<line x1='%d' y1='30' x2='%d' y2='30' stroke='%s' stroke-width='3'/>\n",
                150 + k * 150, 180 + k * 150, cores[k]);
        fprintf(f, "<text x='%d' y='35'>%d</text>\n", 188 + k * 150, tamanhos[k]);
    }

    fprintf(f, "</svg>\n");
    fclose(f);
}

int main()
{
    char titulo[32];
    srand(time(NULL));

    /* 1) Os 5 RUNs: para cada tamanho, gera um vetor aleatorio e mede
     *    os 5 algoritmos sobre ele */
    for (int r = 0; r < NUM_RUNS; r++) {
        printf("\nRUN %d\n", r + 1);
        for (int k = 0; k < NUM_TAMS; k++) {
            int n = tamanhos[k];
            int *original = (int *) malloc(n * sizeof(int));
            for (int i = 0; i < n; i++)
                original[i] = rand() % 65000 + 1;

            for (int a = 0; a < NUM_ALGS; a++) {
                tempos[r][a][k] = medir(a, original, n);
                printf("Time \"%s\"(N=%d): %lf\n", nomes[a], n, tempos[r][a][k]);
                fflush(stdout);   /* mostra a linha na hora */
            }
            free(original);
        }
    }

    /* 2) Media dos 5 RUNs e log10 da media */
    for (int a = 0; a < NUM_ALGS; a++) {
        for (int k = 0; k < NUM_TAMS; k++) {
            double soma = 0;
            for (int r = 0; r < NUM_RUNS; r++)
                soma += tempos[r][a][k];
            medias[a][k] = soma / NUM_RUNS;
            logs[a][k] = log10(medias[a][k]);
        }
    }

    /* 3) Tabelas no terminal e no CSV */
    FILE *f = fopen("resultados.csv", "w");
    for (int r = 0; r < NUM_RUNS; r++) {
        sprintf(titulo, "RUN %d", r + 1);
        imprimir_tabela(titulo, tempos[r]);
        salvar_tabela(f, titulo, tempos[r]);
    }
    imprimir_tabela("Media (5 Runs)", medias);
    salvar_tabela(f, "Media (5 Runs)", medias);
    imprimir_tabela("Log da media", logs);
    salvar_tabela(f, "Log da media", logs);
    fclose(f);

    /* 4) Grafico */
    gerar_grafico();

    printf("\nTabelas salvas em resultados.csv e grafico em grafico.svg\n");
    return 0;
}

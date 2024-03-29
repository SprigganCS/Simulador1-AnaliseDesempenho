#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <string.h>

#define TEMPO_SIMULACAO 36000
#define INTERVALO_MEDIO_CHEGADA 0.01
#define TAXA_CHEGADA 100
#define TEMPO_SIMULACAO_EM_INTERVALO 360
#define SEED 0
/*
 * @tparam no_eventos: número de eventos;
 * @tparam tempo_anterior: tempo anterior;
 * @tparam soma_areas: soma de áreas;
 */
typedef struct little
{
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;

} little;

/*
 * @tparam e_n_final: array de tempo médio de fila
 * @tparam r_w_final: array de tempo médio de espera
 * @tparam lambda: array do operador lambda
 * @tparam tempo: array de tempos
 * @tparam little: array dos cálculos de little
 * @tparam ocupacao: array de ocupacao da simulacao
 * @tparam max_fila: array dos máximos de fila
 *
 */
typedef struct grafico
{
    double e_n_final[TEMPO_SIMULACAO_EM_INTERVALO];
    double e_w_final[TEMPO_SIMULACAO_EM_INTERVALO];
    double lambda[TEMPO_SIMULACAO_EM_INTERVALO];
    double tempo[TEMPO_SIMULACAO_EM_INTERVALO];
    double little[TEMPO_SIMULACAO_EM_INTERVALO];
    double ocupacao[TEMPO_SIMULACAO_EM_INTERVALO];
    double max_fila[TEMPO_SIMULACAO_EM_INTERVALO];

} grafico;

/*
 * @param N/A
 * @return Número aleatório limitado entre 0 e 1
 */
double aleatorio()
{
    double u = rand() / ((double)RAND_MAX + 1);
    u = 1.0 - u;
    return (u);
}

/*
 * @param num1: primeiro número a ser comparado
 * @param num2: segundo número a ser comparado
 * @return Número menor
 */
double minimo(double num1, double num2)
{
    if (num1 < num2)
    {
        return num1;
    }
    return num2;
}

/*
 * @param num1: primeiro número a ser comparado
 * @param num2: segundo número a ser comparado
 * @return Número maior
 */
double maximo(double num1, double num2)
{
    if (num1 > num2)
    {
        return num1;
    }
    return num2;
}

/*
 * @param struct little
 * @return N/A
 * @details Inicia cada item de little com valor 0.0
 */
void inicia_little(little *l)
{
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

/*
 *@param struct grafico
 *@return N/A
 *@details Inicia cada item de grafico com valor 0.0
 */
void inicia_grafico(grafico *g)
{
    for (int i = 0; i < TEMPO_SIMULACAO_EM_INTERVALO; i++)
    {
        g->e_n_final[i] = 0.0;
        g->e_w_final[i] = 0.0;
        g->lambda[i] = 0.0;
        g->tempo[i] = 0.0;
        g->little[i] = 0.0;
        g->ocupacao[i] = 0.0;
        g->max_fila[i] = 0.0;
    }
}

double calculo_l()
{
    double valor = aleatorio();

    if (valor < 0.1)
    {
        return 1500;
    }
    else if (valor < 0.5)
    {
        return 40;
    }
    else if (valor < 1)
    {
        return 550;
    }
}

/*
 * @param percentual_calculado: taxa da porcentagem
 * @param *grafico: ponteiro para uma struct grafico
 */
void resolve(float percentual_calculado, grafico *grafico)
{
    // Variáveis utilizadas para execução da simulação
    double tempo_simulacao, tempo_decorrido = 0.0, intervalo_medio_chegada, tempo_medio_servico;

    // Variáveis utilizadas para captação de resultados da simulação
    double chegada, servico, coleta = TAXA_CHEGADA;
    // Variaǘel necessária para soma de tempo de serviço da simulação
    double soma_tempo_servico = 0.0, tempo_servico = 0.0, atraso_transmissao = 0.0;

    // Variáveis de controle para fila.
    unsigned long int fila = 0, max_fila = 0;

    /*
     * Estruturas para cálculo da Lei de Little, AKA taxa de chegada * media do tempo de espera dentro do sistema
     */
    little e_n, e_w_chegada, e_w_saida;

    // Inicializa as estruturas de little
    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);

    // Iniciando uma semente aleatória estática para garantir os mesmos cálculos em qualquer máquina
    srand(SEED);

    // Defininindo o tempo de simulação conforme o solicitado na atividade
    tempo_simulacao = TEMPO_SIMULACAO;

    // Definindo o intervalo de chegada como 5/s
    intervalo_medio_chegada = INTERVALO_MEDIO_CHEGADA;

    // Taxas conforme cálculo para 80%, 90% 95% e 99%
    tempo_medio_servico = percentual_calculado;

    // Gerando o tempo de chegada da primeira requisicao.
    chegada = (-1.0 / (1.0 / intervalo_medio_chegada)) * log(aleatorio());

    // Índice de controle dos arrays da struct de dados
    int index = 0;

    // EDIT: taxas médias do link para cada ocupação, quanto maior a largura, menos ocupado
    double link[4] = {73500, 55125, 46421, 44545};

    // Looping de simulação
    while (tempo_decorrido <= tempo_simulacao)
    {
        /*
         * Cálculo do tempo decorrido com base na fila.
         * Eventualmente, fila == 0 (esta ocioso), onde o servico pode ser desprezado
         * Havendo fila tempo_decorrido deve ser o menor entre chegada, serviço e tempo de coleta
         * Um caso a mais deve ser tratando dentro da cadeia de ifs (coleta)
         */
        tempo_decorrido = !fila ? minimo(chegada, coleta) : minimo(minimo(chegada, coleta), servico);

        // Se chegou
        if (tempo_decorrido == chegada)
        {
            if (!fila)
            {
                double L = calculo_l(), log_aleatorio = log(aleatorio());

                // EDIT: calculo do atraso de transmissão
                if (percentual_calculado == (float)0.006000)
                {

                    atraso_transmissao = L / link[0];
                }
                else if (percentual_calculado == (float)0.008000)
                {

                    atraso_transmissao = L / link[1];
                }
                else if (percentual_calculado == (float)0.009500)
                {

                    atraso_transmissao = L / link[2];
                }
                else if (percentual_calculado == (float)0.009900)
                {

                    atraso_transmissao = L / link[3];
                }

                //  printf("atraso: %.20f", atraso_transmissao);

                servico = tempo_decorrido + atraso_transmissao;

                soma_tempo_servico += servico - tempo_decorrido;
            }
            fila++;
            max_fila = fila > max_fila ? fila : max_fila;

            chegada = tempo_decorrido + (-1.0 / (1.0 / intervalo_medio_chegada)) * log(aleatorio());

            // Cálculos de Little
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            e_n.no_eventos++;

            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos; // primeira iteração da 0
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_chegada.no_eventos++;
            // Fim dos cálculos de Little
        }
        // Se saiu
        else if (tempo_decorrido == servico)
        {
            // Decréscimo da fila
            fila--;

            if (fila)
            {
                double L = calculo_l(), log_aleatorio = log(aleatorio());

                // EDIT: calculo do atraso de transmissão
                if (percentual_calculado == (float)0.006000)
                {

                    atraso_transmissao = L / link[0];
                }
                else if (percentual_calculado == (float)0.008000)
                {

                    atraso_transmissao = L / link[1];
                }
                else if (percentual_calculado == (float)0.009500)
                {

                    atraso_transmissao = L / link[2];
                }
                else if (percentual_calculado == (float)0.009900)
                {

                    atraso_transmissao = L / link[3];
                }
                // printf("atraso: %.20f", atraso_transmissao);
                servico = tempo_decorrido + atraso_transmissao;

                soma_tempo_servico += servico - tempo_decorrido;
            }

            // Cálculos de Little
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            e_n.no_eventos--;

            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.tempo_anterior = tempo_decorrido;
            e_w_saida.no_eventos++;
            // Fim dos cálculos de Little
        }
        // Agora sim, coleta dos dados
        else
        {
            // Início dos cálculos necessários para os parâmetros listados na atividade
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

            e_w_saida.tempo_anterior = tempo_decorrido;
            e_n.tempo_anterior = tempo_decorrido;
            e_w_chegada.tempo_anterior = tempo_decorrido;

            double e_n_final = e_n.soma_areas / tempo_decorrido;
            double e_w_final = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            double lambda = e_w_chegada.no_eventos / tempo_decorrido;
            // Fim dos cálculos necessários para os parâmetros listados na atividade

            // Coleta se acrescenta como 100, identificada como 100 segundos
            coleta += TAXA_CHEGADA;

            // Início do armazenamento dos cálculos na struct de gráfico
            grafico->e_n_final[index] = e_n_final;
            grafico->e_w_final[index] = e_w_final;
            grafico->lambda[index] = lambda;
            grafico->tempo[index] = tempo_decorrido;
            grafico->little[index] = e_n_final - lambda * e_w_final;
            grafico->ocupacao[index] = soma_tempo_servico / maximo(tempo_decorrido, servico);
            grafico->max_fila[index] = max_fila;

            index++;
            // Fim do armazenamento dos cálculos na struct de gráfico
        }
    }
}

/*
 * @param *graficos: array de graficos em ponteiro
 * @param titulo_grafico: Título do gráfico a ser criado
 * @param eixo_y: Nome do eixo y
 * @param eixo_x: Nome do eixo x
 * @param x_tics: Tamanho do passo do eixo x
 * @param y_tics: Tamanho do passo do eixo y
 * @param nome_grafico: Nome relacionado ao tipo do gráfico: Exemplo: "E[N] = gráfico de E[N]"
 * @param orientacao_legenda: orientação de onde a legenda estará no gráfico
 * @return N/A
 * @details Cria um gráfico com os dados passados imprimindo na tela (espera-se utilizar redirecionador para arquivo txt, AKA ">")
 */
void cria_grafico(grafico *graficos, char *titulo_grafico, char *eixo_y, char *eixo_x, float x_tics, float y_tics, char *nome_grafico, char *orientacao_legenda)
{
    printf("set title '%s' \n", titulo_grafico);
    printf("set xlabel '%s' \n", eixo_x);
    printf("set ylabel  '%s' \n", eixo_y);
    printf("set xtics %.20f \n", x_tics);
    printf("set ytics %.20f \n", y_tics);
    printf("set grid \n");
    printf("set term png \n");
    printf("set terminal png size 1920,1080 font 'Arial,18' \n");
    printf("set key %s \n", orientacao_legenda);
    printf("set output '%s.png' \n", eixo_y);
    printf("plot"
           "'-' u 1:2 t '60%%' with lines lw 3.5, "
           "'-' u 1:2 t '80%%' with lines lw 3.5,"
           "'-' u 1:2 t '95%%' with lines lw 3.5,"
           "'-' u 1:2 t '99%%' with lines lw 3.5"
           "\n");

    if (strcmp(nome_grafico, "Little") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < TEMPO_SIMULACAO_EM_INTERVALO; j++)
            {
                printf("    %lF %.20lF \n", graficos[i].tempo[j], graficos[i].little[j]);
            }
            printf("e \n");
        }
    }
    else if (strcmp(nome_grafico, "E[N]") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < TEMPO_SIMULACAO_EM_INTERVALO; j++)
            {
                printf("    %lF %lF \n", graficos[i].tempo[j], graficos[i].e_n_final[j]);
            }
            printf("e \n");
        }
    }
    else if (strcmp(nome_grafico, "E[W]") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < TEMPO_SIMULACAO_EM_INTERVALO; j++)
            {
                printf("    %lF %lF \n", graficos[i].tempo[j], graficos[i].e_w_final[j]);
            }
            printf("e \n");
        }
    }
    else if (strcmp(nome_grafico, "Ocupacao") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < TEMPO_SIMULACAO_EM_INTERVALO; j++)
            {
                printf("    %lF %lF \n", graficos[i].tempo[j], graficos[i].ocupacao[j]);
            }
            printf("e \n");
        }
    }
    else if (strcmp(nome_grafico, "Max_fila") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < TEMPO_SIMULACAO_EM_INTERVALO; j++)
            {
                printf("    %lF %lF \n", graficos[i].tempo[j], graficos[i].max_fila[j]);
            }
            printf("e \n");
        }
    }
    else if (strcmp(nome_grafico, "Lambda") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < TEMPO_SIMULACAO_EM_INTERVALO; j++)
            {
                printf("    %lF %lF \n", graficos[i].tempo[j], graficos[i].lambda[j]);
            }
            printf("e \n");
        }
    }
}

/*
 * @param N/A
 * @return N/A
 * @details Função principal
 */
void main()
{
    // Utilizado para permissão de escrita de caracteres brasileiros
    setlocale(LC_ALL, "PT_BR");

    // Array de taxas já calculadas com base nas porcentagens, onde taxas[0] = 60, taxas[1] = 80%, taxas[2] = 95%, taxas[3] = 99%
    float taxas[] = {0.006, 0.008, 0.0095, 0.0099};

    // Criando instâncias da struct gráfico para cada uma das porcentagens
    grafico grafico_60, grafico_80, grafico_95, grafico_99;

    /*
     * Criando um array que possui as instâncias de gráfico das porcentagens.
     * Essa abordagem irá facilitar gerar gráficos e resultados esperados para cada porcentagem
     */
    grafico graficos[4] = {grafico_60, grafico_80, grafico_95, grafico_99};

    /*
     * Iniciamos as structs do array de gráficos e a enviamos por referência para "resolve"
     * A função "resolve" irá preencher os dados de cada gráfico
     */
    for (int i = 0; i < 4; i++)
    {
        inicia_grafico(&graficos[i]);
        resolve(taxas[i], &graficos[i]);
    }

    // Criando gráficos
    cria_grafico(graficos, "Tempo médio de fila para diferentes ocupações", "E[N]", "Tempo (s)", 2000, 10, "E[N]", "left top");
    cria_grafico(graficos, "Tempo médio de espera para diferentes ocupações", "E[W]", "Tempo (s)", 2000, 0.1, "E[W]", "left top");
    cria_grafico(graficos, "Ocupações conforme o tempo", "Ocupacao", "Tempo (s)", 2000, 0.025, "Ocupacao", "right bot");
    cria_grafico(graficos, "Erro de Little para diferentes ocupações", "Little", "Tempo (s)", 2000, 0.0000000090, "Little", "left top");
}
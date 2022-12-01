#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct little
{
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;

} little;

typedef struct grafico
{
    double e_n_final[360];
    double e_w_final[360];
    double lambda[360];
    double tempo[360];
    double little[360];
    double ocupacao[360];
    double max_fila[360];

} grafico;

double aleatorio()
{
    double u = rand() / ((double)RAND_MAX + 1);
    // limitando entre (0,1]
    u = 1.0 - u;

    return (u);
}

double minimo(double num1, double num2)
{
    if (num1 < num2)
    {
        return num1;
    }
    return num2;
}

double maximo(double num1, double num2)
{
    if (num1 > num2)
    {
        return num1;
    }
    return num2;
}

void inicia_little(little *l)
{
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

// inicializa gráfico
void inicia_grafico(grafico *g)
{
    for (int i = 0; i < 360; i++)
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

int resolve(float a, grafico *grafico)
{
    double tempo_simulacao;
    double tempo_decorrido = 0.0;

    double intervalo_medio_chegada;
    double tempo_medio_servico;

    double chegada;
    double servico;
    double coleta = 100.0;

    double soma_tempo_servico = 0.0;

    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    /*
    Little -> E[n] = lambda * E[w] -> média de quantidade de requisicoes dentro do sistema = taxa de chegada * media do tempo de espera dentro do sistema
    */

    little e_n;
    little e_w_chegada;
    little e_w_saida;

    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);

    /*
    Little -- fim
    */

    srand(0);

    // printf("Informe o tempo de simulacao (segundos): ");
    // scanf("%lF", &tempo_simulacao);
    tempo_simulacao = 36000;

    // printf("Informe o intervalo medio entre chegadas (segundos): ");
    // scanf("%lF", &intervalo_medio_chegada);
    intervalo_medio_chegada = 0.2;

    // printf("Informe o tempo medio de servico (segundos): ");
    // scanf("%lF", &tempo_medio_servico);
    tempo_medio_servico = a; // receber  4 parametros para esse valor que faz a taxa de ocupação ser 80%, 90% 95% e 99%

    // gerando o tempo de chegada da primeira requisicao.
    chegada = (-1.0 / (1.0 / intervalo_medio_chegada)) * log(aleatorio());

    int index = 0;

    while (tempo_decorrido <= tempo_simulacao)
    {

        // chegada, servico, coleta
        // eventualmente, fila == 0 (esta ocioso) --> servico pode ser desprezada
        tempo_decorrido = !fila ? minimo(chegada, coleta) : minimo(minimo(chegada, coleta), servico);

        // havendo fila tempo_decorrido deve ser o menor entre chegada, serviço e tempo de coleta
        // um caso a mais deve ser tratando dentro da cadeia de ifs (coleta)

        // chegada

        if (tempo_decorrido == chegada)
        {
            // printf("Chegada em %lF.\n", tempo_decorrido);
            if (!fila)
            {
                servico = tempo_decorrido + (-1.0 / (1.0 / tempo_medio_servico)) * log(aleatorio());
                soma_tempo_servico += servico - tempo_decorrido;
            }
            fila++;
            max_fila = fila > max_fila ? fila : max_fila;

            chegada = tempo_decorrido + (-1.0 / (1.0 / intervalo_medio_chegada)) * log(aleatorio());

            // little
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            e_n.no_eventos++;

            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos; // primeira iteração da 0
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_chegada.no_eventos++;
            // little
        }
        else if (tempo_decorrido == servico)
        { // saida
            // printf("Saida em %lF.\n", tempo_decorrido);
            fila--;

            if (fila)
            {
                servico = tempo_decorrido + (-1.0 / (1.0 / tempo_medio_servico)) * log(aleatorio());
                soma_tempo_servico += servico - tempo_decorrido;
            }

            // little
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            e_n.no_eventos--;

            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.tempo_anterior = tempo_decorrido;
            e_w_saida.no_eventos++;
            // little
        }
        else
        {
            // coleta dados
            // printf("\nTempo de coleta: %lF\n", tempo_decorrido);
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

            e_w_saida.tempo_anterior = tempo_decorrido;
            e_n.tempo_anterior = tempo_decorrido;
            e_w_chegada.tempo_anterior = tempo_decorrido;

            double e_n_final = e_n.soma_areas / tempo_decorrido;
            double e_w_final = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            double lambda = e_w_chegada.no_eventos / tempo_decorrido;

            coleta += 100;

            grafico->e_n_final[index] = e_n_final;
            grafico->e_w_final[index] = e_w_final;
            grafico->lambda[index] = lambda;
            grafico->tempo[index] = tempo_decorrido;
            grafico->little[index] = e_n_final - lambda * e_w_final;
            grafico->ocupacao[index] = soma_tempo_servico / maximo(tempo_decorrido, servico);
            grafico->max_fila[index] = max_fila;

            index++;
            // se entrar nesse caso a variavel de coleta deve ser atualizada para +100
        }
    }

    return 0;
}

// para 90% 80% 95% e 99% deve modificar o tempo de serviço, já que o tempo médio de chegada é fixa
//  grafico de ocupacao (eixo y de 0 a 1 (ocupacao)) e eixo x com tempo) o mesmo gráfico deve conter todos os valores (80%, 90%, ...)
//  nenhuma medida pode tender ao infinito

void cria_grafico(grafico graficos, char *rotulo, char *nome_grafico)
{
    // create a gnuplot graphic
    printf("set title '%s' \n", rotulo);
    printf("set xlabel '%s' \n", "Tempo");
    printf("set ylabel '%s' \n", "Ocupação");
    printf("set grid \n");
    printf("set term png \n");
    printf("set output '%s.png' \n", nome_grafico);
    printf("plot '-' with lines title '%s' \n", rotulo);

    for (int i = 0; i < 360; i++)
    {
        printf("%lF %lF \n", graficos.tempo[i], graficos.e_n_final[i]);
    }
    printf("plot '-' with lines title '%s' \n", "lakaka");
    for (int i = 0; i < 360; i++)
    {
        printf("%lF %lF \n", graficos.tempo[i], graficos.e_w_final[i]);
    }
    printf("e \n");
}

int main()
{
    // 80% = 0.16, 90% = 0.18, 95% = 0.19 e 99% = 0.198
    float taxas[] = {0.16, 0.18, 0.19, 0.198};
    grafico grafico_80, grafico_90, grafico_95, grafico_99;

    grafico graficos[4] = {grafico_80, grafico_90, grafico_95, grafico_99};

    for (int i = 0; i < 4; i++)
    {
        inicia_grafico(&graficos[i]);
        resolve(taxas[i], &graficos[i]);
    }

    for (int i = 0; i < 4; i++)
    {
        char nome[12];
        sprintf(nome, "grafico_%d", i);
        cria_grafico(graficos[i], "E[N]", nome);
    }

    system("gnuplot grafico.txt");
}

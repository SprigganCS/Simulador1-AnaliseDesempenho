#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define tamanho_simulacao 36000;
#define tamanho_amostra 100;

typedef struct little
{
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;

} little;

typedef struct resultado
{
    int index;
    double e_n_final;
    double e_w_final;
    double lambda;
    double little;
} resultado;

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

//initiate resultado array
void inicia_resultado(resultado *r)
{
    for (int i = 0; i <= 360; i++)
    {
        r[i].index = i;
        r[i].e_n_final = 0.0;
        r[i].e_w_final = 0.0;
        r[i].lambda = 0.0;
     //   r[i].little = 0.0;
    }
}


int resolve(float a)
{
    double tempo_simulacao;
    double tempo_decorrido = 0.0;

    double intervalo_medio_chegada;
    double tempo_medio_servico;

    double chegada;
    double servico;

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

    srand(7);

    tempo_simulacao = tamanho_simulacao;

    intervalo_medio_chegada = 5;

    tempo_medio_servico = a; // receber  4 parametros para esse valor que faz a taxa de ocupação ser 80%, 90% 95% e 99%

    // gerando o tempo de chegada da primeira requisicao.
    chegada = (-1.0 / (1.0 / intervalo_medio_chegada)) * log(aleatorio());

    // gerando array de 0 até 36000 com step de 100
    int arr_tempo[3600];
    for (int i = 0; i < 3600; i++)
    {
        arr_tempo[i] = i * 100;
    }

    int indice_tempo = 0;

    resultado resultados[((int)((int)tempo_simulacao) / 100) + 1];
    //printf("tamanho do array de resultados: %d", (int)((int)tempo_simulacao) / 100);
    inicia_resultado(resultados);

    double e_n_final = 0;
    double e_w_final = 0;
    double lambda = 0;

    while (tempo_decorrido <= tempo_simulacao)
    {
        tempo_decorrido = !fila ? chegada : minimo(chegada, servico);

        if (tempo_decorrido >= arr_tempo[indice_tempo])
        {
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

            e_n_final = e_n.soma_areas / tempo_decorrido;
            e_w_final = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            lambda = e_w_chegada.no_eventos / tempo_decorrido;

            resultados[indice_tempo].index = indice_tempo;
            resultados[indice_tempo].e_n_final = e_n_final;
            resultados[indice_tempo].e_w_final = e_w_final;
            resultados[indice_tempo].lambda = lambda;
            resultados[indice_tempo].little = e_n_final - (lambda * e_w_final);

            indice_tempo++;
        }

        if (tempo_decorrido == chegada)
        {
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

            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_chegada.no_eventos++;
            // little
        }
        else
        { // saida
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
    }

    printf("Erro de Little: %.20lF\n\n", e_n_final - lambda * e_w_final);

    printf("Ocupacao: %lF.\n", soma_tempo_servico / maximo(tempo_decorrido, servico));
    printf("Max fila: %ld.\n", max_fila);

    return 0;
}

// para 90% 80% 95% e 99% deve modificar o tempo de serviço, já que o tempo médio de chegada é fixa
//  grafico de ocupacao (eixo y de 0 a 1 (ocupacao)) e eixo x com tempo) o mesmo gráfico deve conter todos os valores (80%, 90%, ...)
//  nenhuma medida pode tender ao infinito
//

int main()
{
    float taxas[] = {4, 4.5, 4.75, 4.95};
    for (int i = 0; i < 4; i++)
    {
        resolve(taxas[i]);
        printf("-----------------------------------------------");
    }
}

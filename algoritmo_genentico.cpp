/*
Ideia para a parte de seleção: 

Para solução dada pelo algoritmo de construção: aplicamos '1 / custo_da_solucao' e armazenamos em um vetor.
A partir disso somamos os resultados armazenados e fazemos 'x = 1 / soma dos valores'.
Depois multiplicamos todos os valores armazenados no vetor por x, desta forma temos a probabilidade de cada solução ser escolhida de um valor 0 a 1.
Para facilitar na seleção podemos somar todos os elementos dester vetor pelo elemento da posição anteriro, neste sentido o último elemento do vetor será igual 1.

Para selecionar probabilisticamente, podemos escolher sortear um número aleatório no intervalo de 0 a 100, 
e a partir disso selecionar a solução que está nesta faixa de valores armazenados.
Se o valor sorteado está entre dois elementos do vetor, escolhe-se o elemento maior.

*/

// Função para sortear um número
#include <random>
#include <iostream>
#include "arquivo_e_estrutura_de_dados.cpp"

using namespace std;



struct Sorteados {
    double primeiro;
    double segundo;
};

// Armazena o indice das soluções selecionadas
Sorteados solucoes_selecionadas;


vector<double> vetor_de_probabilidade(vector<Solucao> solucoes){

    vector<double> vetor_auxiliar;
    double solucao_inversa;
    double soma_solucoes = 0.0;

    for (int i = 0; i < solucoes.size(); i++){

        solucao_inversa = 1 / solucoes[i].custo;
        soma_solucoes += solucao_inversa;
        vetor_auxiliar.push_back(solucao_inversa);

    }

    double multiplicador = 100 / soma_solucoes;

    vetor_auxiliar[0] *= multiplicador;

    for (int i = 1; i < vetor_auxiliar.size(); i++){

        vetor_auxiliar[i] *= multiplicador;
        vetor_auxiliar[i] += vetor_auxiliar[i-1];
    }

    return vetor_auxiliar;

}

Sorteados sorteaar_de_0_a_100()
{
    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());  // to seed mersenne twister.
                        // replace the call to rd() with a
                        // constant value to get repeatable
                        // results.
    uniform_real_distribution<> dist(0.0, 100.0);

    Sorteados resultado = {dist(gen), dist(gen)};
    return resultado;
}

int sorteaar_de_0_a_n(int n)
{
    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());  // to seed mersenne twister.
                        // replace the call to rd() with a
                        // constant value to get repeatable
                        // results.
    uniform_real_distribution<> dist(0, n);

    int resultado = dist(gen);
    return resultado;
}
void solucoes_sorteadas(vector<double> vetor_solucoes_inversas, Sorteados numeros_sorteados){

    

    int indice_primeiro = 0, indice_segundo = 0;
    int k = 1;
    int tamanho_do_vetor = vetor_solucoes_inversas.size();
    while (indice_primeiro < tamanho_do_vetor && k){

        if (vetor_solucoes_inversas[indice_primeiro] >= numeros_sorteados.primeiro){
            k = 0;
        }
        indice_primeiro ++;
    }

    indice_primeiro --;
    k = 1;

    while (indice_segundo < tamanho_do_vetor && k){

        if (vetor_solucoes_inversas[indice_segundo] >= numeros_sorteados.segundo){
            k = 0;
        }
        indice_segundo ++;
    }

    indice_segundo --;

    if (indice_primeiro == indice_segundo){
        if (indice_segundo > 0){
            indice_segundo--;
        }
        else{
            indice_segundo++;
        }
    }

    solucoes_selecionadas.primeiro = indice_primeiro;
    solucoes_selecionadas.segundo = indice_segundo;
    
}

// Cruzamento por fusao (inspirado em Beasley & Chu) com reparo guloso.
// As colunas presentes nos dois pais são herdadas direto (consenso); as presentes
// em apenas um pai são herdadas com probabilidade que favorece o pai de menor custo.
// Em seguida o filho é reparado gulosamente (cobrindo as linhas que sobraram) e tem
// as colunas redundantes eliminadas, garantindo uma solução viável e enxuta.
Solucao cruzamento(const Solucao& pai1, const Solucao& pai2){

    int melhor_coluna;
    float melhor_valor;
    float valor_atual;

    inicializar_solucao();

    // Marca quais colunas cada pai possui: 1 = só pai1, 2 = só pai2, 3 = ambos.
    vector<int> presenca(colunas, 0);
    for (int i = 0; i < pai1.colunas.size(); i++){
        presenca[pai1.colunas[i]] = presenca[pai1.colunas[i]] | 1;
    }
    for (int i = 0; i < pai2.colunas.size(); i++){
        presenca[pai2.colunas[i]] = presenca[pai2.colunas[i]] | 2;
    }

    // Probabilidade de herdar uma coluna exclusiva de cada pai (favorece o de menor custo).
    float soma_custos = pai1.custo + pai2.custo;
    float prob_pai1 = (soma_custos > 0) ? pai2.custo / soma_custos : 0.5f;
    float prob_pai2 = (soma_custos > 0) ? pai1.custo / soma_custos : 0.5f;

    uniform_real_distribution<float> sorteio(0.0f, 1.0f);

    for (int i = 0; i < colunas; i++){
        if (presenca[i] == 3){
            adicionar_coluna(i);                          // consenso dos pais: mantem sempre
        } else if (presenca[i] == 1){
            if (sorteio(gerador) < prob_pai1){
                adicionar_coluna(i);
            }
        } else if (presenca[i] == 2){
            if (sorteio(gerador) < prob_pai2){
                adicionar_coluna(i);
            }
        }
    }

    // Reparo guloso: enquanto sobrar linha descoberta, adiciona a melhor coluna disponível.
    while (linhas_descobertas > 0){
        melhor_coluna = -1;
        melhor_valor = 0;
        for (int i = 0; i < colunas; i++){
            if (ganho_coluna[i] > 0){
                valor_atual = avaliar_coluna(i, func_global);
                if (melhor_coluna == -1 || valor_atual < melhor_valor){
                    melhor_coluna = i;
                    melhor_valor = valor_atual;
                }
            }
        }
        adicionar_coluna(melhor_coluna);
    }

    // Remove colunas que ficaram redundantes após a fusão e o reparo.
    eliminar_redundancia();

    return salvar_solucao();
}

void adicionar_colunas_e_eliminar_mutacoes(vector<int> colunas_adicionadas){

    vector<int> selecionadas;
    int coluna_atual;

    for (int i = 0; i < colunas; i++){
        if (colunas_selecionadas[i]){
            selecionadas.push_back(i);
        }
    }

    sort(selecionadas.begin(), selecionadas.end(), comparar_por_peso_desc);

    int tamanho_adicional = colunas_adicionadas.size();

    for (int i = 0; i < tamanho_adicional; i++){
        selecionadas.push_back(colunas_adicionadas[i]);
        adicionar_coluna(colunas_adicionadas[i]);
    }

    for (int i = 0; i < selecionadas.size() - tamanho_adicional; i++){
        coluna_atual = selecionadas[i];
        if (coluna_redundante(coluna_atual)){
            remover_coluna(coluna_atual);
        }
    }
}


Solucao mutacao(Solucao filho, int constante){

    
    
    int tamanho_da_mutacao = sorteaar_de_0_a_n(int(filho.colunas.size()) / constante) + 1;
    int  numero_sorteado;
    vector<int> colunas_sorteadas;

    for (int i = 0; i < tamanho_da_mutacao; i++){
        numero_sorteado = sorteaar_de_0_a_n(colunas-1);
        colunas_sorteadas.push_back(numero_sorteado);
    }

    
    adicionar_colunas_e_eliminar_mutacoes(colunas_sorteadas);
    return salvar_solucao();
}

int main(){

    solucoes_algoritmo_de_construcao();
    vector<double> vetor_solucoes_inversas = vetor_de_probabilidade(populacao);
    Sorteados numeros_sorteados = sorteaar_de_0_a_100();
    /*
    for (int i = 0; i < vetor_solucoes_inversas.size(); i++){
        cout << vetor_solucoes_inversas[i] << endl;
    }
    */
    cout << endl <<"Numeros sorteados: ";
    cout << numeros_sorteados.primeiro << " " << numeros_sorteados.segundo << endl;
    solucoes_sorteadas(vetor_solucoes_inversas, numeros_sorteados);
    cout << "Primeira solução escolhida:" << int(solucoes_selecionadas.primeiro) << " . Segunda solução escolhida:" << int(solucoes_selecionadas.segundo) << endl;

    // Cruzamento dos dois pais selecionados.
    int indice_pai1 = int(solucoes_selecionadas.primeiro);
    int indice_pai2 = int(solucoes_selecionadas.segundo);
    Solucao pai1 = populacao[indice_pai1];
    Solucao pai2 = populacao[indice_pai2];
    Solucao filho = cruzamento(pai1, pai2);
    int sorteio_de_mutacao = sorteaar_de_0_a_n(10);

    if (sorteio_de_mutacao == 9){
        filho = mutacao(filho, 4);
    }
    

    cout << endl << "Cruzamento:" << endl;
    cout << "Pai 1 (indice " << indice_pai1 << "): custo = " << fixed << setprecision(2)
         << pai1.custo << "   (" << pai1.colunas.size() << " col)" << endl;
    cout << "Pai 2 (indice " << indice_pai2 << "): custo = " << pai2.custo
         << "   (" << pai2.colunas.size() << " col)" << endl;
    if (sorteio_de_mutacao == 9){
        cout << "Mutação gerada no filho!" << endl;
    }
    cout << "Filho gerado:        custo = " << filho.custo
         << "   (" << filho.colunas.size() << " col)" << endl;

    return 0;
}

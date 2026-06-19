/*
Ideia para a parte de seleção: 

Para solução dada pelo algoritmo de construção: aplicamos '1 / custo_da_solucao' e armazenamos em um vetor.
A partir disso somamos os resultados armazenados e fazemos 'x = 1 / soma dos valores'.
Depois multiplicamos todos os valores armazenados no vetor por x, desta forma temos a probabilidade de cada solução ser escolhida de um valor 0 a 1.
Para facilitar na seleção podemos somar todos os elementos dester vetor pelo elemento da posição anteriro, neste sentido o último elemento do vetor será igual 1.

Para selecionar probabilisticamente, podemos escolher sortear um número aleatório no intervalo de 0 a 1, 
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
}

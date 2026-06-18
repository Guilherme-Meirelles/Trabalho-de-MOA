/*
Ideia para a parte de seleção: 

Para solução dada pelo algoritmo de construção: aplicamos '1 / custo_da_solucao' e armazenamos em um vetor.
A partir disso somamos os resultados armazenados e fazemos 'x = 1 / soma dos valores'.
Depois multiplicamos todos os valores armazenados no vetor por x, desta forma temos a probabilidade de cada solução ser escolhida de um valor 0 a 1.
Para facilitar na seleção podemos somar todos os elementos dester vetor pelo elemento da posição anteriro, neste sentido o último elemento do vetor será igual 1.

Para selecionar probabilisticamente, podemos escolher sortear um número aleatório no intervalo de 0 a 1, 
e a partir disso selecionar a solução que está nesta faixa de valores armazenados.
Se o valor sentido está entre dois elementos do vetor, escolhe-se o elemento maior.

*/

// Função para sortear um número
#include <random>
#include <iostream>

using namespace std;

int main()
{
    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());  // to seed mersenne twister.
                        // replace the call to rd() with a
                        // constant value to get repeatable
                        // results.
    uniform_real_distribution<> dist(0.0, 1.0);
    for (int i = 0; i < 5; ++i) {
        cout << dist(gen) << " "; // print the raw output of the generator.
    }
    cout << endl;
}
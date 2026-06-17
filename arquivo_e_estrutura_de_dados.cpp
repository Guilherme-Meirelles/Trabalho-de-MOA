#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

struct Coluna{
    float peso;
    vector<int> linhas;
};

int colunas;
int linhas;
vector<Coluna> tabela_de_dados;
vector<vector<int>> lista_de_adjacencia_linhas;

vector<bool> colunas_selecionadas;
float custo_total;
vector<int> cobertura_linha;
int linhas_descobertas;
vector<int> ganho_coluna;




vector<string> fatiamento_de_string(string string_para_fatiar){
    vector<string> vetor_de_string;
    stringstream ss(string_para_fatiar);
    string auxiliar;
    while (ss >> auxiliar){
        vetor_de_string.push_back(auxiliar);
    }
    return vetor_de_string;
}


void construcao_da_tabela(string nome_do_arquivo){

    stringstream ss;
    vector<string> vetor_string;
    Coluna coluna_da_tabela;
    ifstream Arquivo(nome_do_arquivo);
    string linhas_do_arquivo;

    getline(Arquivo, linhas_do_arquivo);
    vetor_string = fatiamento_de_string(linhas_do_arquivo);
    linhas = stoi(vetor_string[1]);

    getline(Arquivo, linhas_do_arquivo);
    vetor_string = fatiamento_de_string(linhas_do_arquivo);
    colunas = stoi(vetor_string[1]);

    getline(Arquivo, linhas_do_arquivo);

    while(getline(Arquivo, linhas_do_arquivo)){
        vetor_string = fatiamento_de_string(linhas_do_arquivo);
        
        coluna_da_tabela.peso = stof(vetor_string[1]);
        coluna_da_tabela.linhas.clear();
        for (int i = 2; i < vetor_string.size(); i++){
            coluna_da_tabela.linhas.push_back(stoi(vetor_string[i]) - 1);
        }
        tabela_de_dados.push_back(coluna_da_tabela);
    }
    Arquivo.close();
}

void montar_lista_de_adjacencia(){

    int linha_unica;
    lista_de_adjacencia_linhas.resize(linhas);

    for(int i = 0; i < colunas; i++){
        for (int j = 0; j < tabela_de_dados[i].linhas.size(); j++){
            linha_unica = tabela_de_dados[i].linhas[j];
            lista_de_adjacencia_linhas[linha_unica].push_back(i);
        }
    }

}

void inicializar_solucao(){

    colunas_selecionadas.assign(colunas, false);
    custo_total = 0;
    cobertura_linha.assign(linhas, 0);
    linhas_descobertas = linhas;
    ganho_coluna.resize(colunas);

    for (int i = 0; i < colunas; i++){
        ganho_coluna[i] = tabela_de_dados[i].linhas.size();
    }
}

void adicionar_coluna(int coluna){

    int linha_atual;
    int coluna_vizinha;

    colunas_selecionadas[coluna] = true;
    custo_total = custo_total + tabela_de_dados[coluna].peso;

    for (int i = 0; i < tabela_de_dados[coluna].linhas.size(); i++){
        linha_atual = tabela_de_dados[coluna].linhas[i];

        if (cobertura_linha[linha_atual] == 0){
            linhas_descobertas = linhas_descobertas - 1;
            for (int j = 0; j < lista_de_adjacencia_linhas[linha_atual].size(); j++){
                coluna_vizinha = lista_de_adjacencia_linhas[linha_atual][j];
                ganho_coluna[coluna_vizinha] = ganho_coluna[coluna_vizinha] - 1;
            }
        }
        cobertura_linha[linha_atual] = cobertura_linha[linha_atual] + 1;
    }
}

void remover_coluna(int coluna){

    int linha_atual;
    int coluna_vizinha;

    colunas_selecionadas[coluna] = false;
    custo_total = custo_total - tabela_de_dados[coluna].peso;

    for (int i = 0; i < tabela_de_dados[coluna].linhas.size(); i++){
        linha_atual = tabela_de_dados[coluna].linhas[i];
        cobertura_linha[linha_atual] = cobertura_linha[linha_atual] - 1;

        if (cobertura_linha[linha_atual] == 0){
            linhas_descobertas = linhas_descobertas + 1;
            for (int j = 0; j < lista_de_adjacencia_linhas[linha_atual].size(); j++){
                coluna_vizinha = lista_de_adjacencia_linhas[linha_atual][j];
                ganho_coluna[coluna_vizinha] = ganho_coluna[coluna_vizinha] + 1;
            }
        }
    }
}

bool coluna_redundante(int coluna){

    int linha_atual;

    for (int i = 0; i < tabela_de_dados[coluna].linhas.size(); i++){
        linha_atual = tabela_de_dados[coluna].linhas[i];
        if (cobertura_linha[linha_atual] < 2){
            return false;
        }
    }
    return true;
}

bool comparar_por_peso_desc(int a, int b){
    return tabela_de_dados[a].peso > tabela_de_dados[b].peso;
}

void eliminar_redundancia(){

    vector<int> selecionadas_ordenadas;
    int coluna_atual;

    for (int i = 0; i < colunas; i++){
        if (colunas_selecionadas[i]){
            selecionadas_ordenadas.push_back(i);
        }
    }

    sort(selecionadas_ordenadas.begin(), selecionadas_ordenadas.end(), comparar_por_peso_desc);

    for (int i = 0; i < selecionadas_ordenadas.size(); i++){
        coluna_atual = selecionadas_ordenadas[i];
        if (coluna_redundante(coluna_atual)){
            remover_coluna(coluna_atual);
        }
    }
}

float avaliar_coluna(int coluna, int funcao){

    float c = tabela_de_dados[coluna].peso;
    int k = ganho_coluna[coluna];

    switch (funcao){
        case 1: return c;
        case 2: return c / k;
        case 3: return c / log2(k);
        case 4: return c / (k * log2(k));
        case 5: return c / (k * log(k));
        case 6: return c / (k * k);
        case 7: return sqrt(c) / (k * k);
    }
    return c / k;
}

void construcao_gulosa(int funcao){

    int melhor_coluna;
    float melhor_valor;
    float valor_atual;

    inicializar_solucao();

    while (linhas_descobertas > 0){
        melhor_coluna = -1;
        melhor_valor = 0;
        for (int i = 0; i < colunas; i++){
            if (ganho_coluna[i] > 0){
                valor_atual = avaliar_coluna(i, funcao);
                if (melhor_coluna == -1 || valor_atual < melhor_valor){
                    melhor_coluna = i;
                    melhor_valor = valor_atual;
                }
            }
        }
        adicionar_coluna(melhor_coluna);
    }
}

int contar_colunas_selecionadas(){

    int total = 0;
    for (int i = 0; i < colunas; i++){
        if (colunas_selecionadas[i]){
            total = total + 1;
        }
    }
    return total;
}

int main(){

    construcao_da_tabela("./Tabela/Teste_01.dat");
    montar_lista_de_adjacencia();

    cout << "Linhas: " << linhas << "   Colunas: " << colunas << endl;
    cout << endl << "Construcao Gulosa - comparacao das 7 funcoes" << endl << endl;

    for (int funcao = 1; funcao <= 7; funcao++){
        construcao_gulosa(funcao);
        cout << "Funcao " << funcao
             << ":   guloso = " << custo_total
             << " (" << contar_colunas_selecionadas() << " col)";
        eliminar_redundancia();
        cout << "   pos-reducao = " << custo_total
             << " (" << contar_colunas_selecionadas() << " col)"
             << "   descobertas = " << linhas_descobertas << endl;
    }

    return 0;
}

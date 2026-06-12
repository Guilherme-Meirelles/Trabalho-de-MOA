#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

struct Coluna{
    float peso;
    vector<int> linhas;
};

int colunas;
int linhas;
vector<Coluna> tabela_de_dados;
vector<vector<int>> lista_de_adjacencia_linhas;




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
            coluna_da_tabela.linhas.push_back(stoi(vetor_string[i]));
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
            lista_de_adjacencia_linhas[linha_unica-1].push_back(i+1);
        }
    }

}

int main(){
    
    construcao_da_tabela("./Tabela/Teste_01.dat");
    
    cout << linhas << endl;
    cout << colunas << endl;
    for (int i = 0; i < colunas; i++){
        cout << i+1 << ". Peso: " << tabela_de_dados[i].peso << "   Linhas: ";
        for (int j = 0; j < tabela_de_dados[i].linhas.size(); j++){
            cout << tabela_de_dados[i].linhas[j] << ", ";
        }
        cout << endl;
    }
    cout << endl << "Lista de adjacência" << endl;

    montar_lista_de_adjacencia();
    
    for (int i = 0; i < linhas; i++){
        
        cout << endl << "Linha " <<i+1 << ". ";
        for (int j = 0; j < lista_de_adjacencia_linhas[i].size(); j++){
            cout << lista_de_adjacencia_linhas[i][j] << ", ";
        }
        cout << endl;
    }
        
    
    return 0;
}

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <set>
#include <iomanip>
using namespace std;

// Uma coluna da instancia: seu custo (peso) e quais linhas ela cobre.
struct Coluna{
    float peso;
    vector<int> linhas;
};

// Uma solução armazenada de forma esparsa: so os índices das colunas escolhidas, mais o custo.
struct Solucao{
    vector<int> colunas;
    float custo;
};

int colunas;
int linhas;
vector<Coluna> tabela_de_dados;                  // dados de cada coluna
vector<vector<int>> lista_de_adjacencia_linhas;  // para cada linha, quais colunas a cobrem
vector<Solucao> populacao;                       // populacao final entregue ao genetico

// Estado de trabalho denso, atualizado de forma incremental durante a construçãoo.
vector<bool> colunas_selecionadas;
float custo_total;
vector<int> cobertura_linha;    // quantas colunas escolhidas cobrem cada linha
int linhas_descobertas;
vector<int> ganho_coluna;       // quantas linhas ainda descobertas cada coluna cobriria

// Apoio do sorteio e buffers reutilizados pela construção randomizada.
mt19937 gerador;
vector<int> candidatos;
vector<float> scores;
vector<int> lista_restrita;

// Semente fixa (resultados reproduziveis) e melhor configuraçãoo encontrada na varredura.
const unsigned int SEMENTE = 12345;
float melhor_global = -1;
int func_global = 1;
float alpha_global = 0.10f;




// Quebra uma linha de texto em palavras separadas por espacos.
vector<string> fatiamento_de_string(string string_para_fatiar){
    vector<string> vetor_de_string;
    stringstream ss(string_para_fatiar);
    string auxiliar;
    while (ss >> auxiliar){
        vetor_de_string.push_back(auxiliar);
    }
    return vetor_de_string;
}


// Lê o arquivo .dat: quantidade de linhas/colunas e, por coluna, peso e linhas cobertas.
void construcao_da_tabela(string nome_do_arquivo){

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
            coluna_da_tabela.linhas.push_back(stoi(vetor_string[i]) - 1);  // -1: índices base zero
        }
        tabela_de_dados.push_back(coluna_da_tabela);
    }
    Arquivo.close();
}

// Monta o índice inverso: para cada linha, a lista de colunas que a cobrem.
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

// Zera o estado de trabalho para começar uma nova construção do zero.
void inicializar_solucao(){

    colunas_selecionadas.assign(colunas, false);
    custo_total = 0;
    cobertura_linha.assign(linhas, 0);
    linhas_descobertas = linhas;
    ganho_coluna.resize(colunas);

    for (int i = 0; i < colunas; i++){
        ganho_coluna[i] = tabela_de_dados[i].linhas.size();  // no início, ganho = todas as linhas da coluna
    }
}

// Adiciona uma coluna a solução e atualiza cobertura, custo e ganho das colunas vizinhas.
void adicionar_coluna(int coluna){

    int linha_atual;
    int coluna_vizinha;

    colunas_selecionadas[coluna] = true;
    custo_total = custo_total + tabela_de_dados[coluna].peso;

    for (int i = 0; i < tabela_de_dados[coluna].linhas.size(); i++){
        linha_atual = tabela_de_dados[coluna].linhas[i];

        if (cobertura_linha[linha_atual] == 0){   // linha sai de descoberta para coberta
            linhas_descobertas = linhas_descobertas - 1;
            for (int j = 0; j < lista_de_adjacencia_linhas[linha_atual].size(); j++){
                coluna_vizinha = lista_de_adjacencia_linhas[linha_atual][j];
                ganho_coluna[coluna_vizinha] = ganho_coluna[coluna_vizinha] - 1;
            }
        }
        cobertura_linha[linha_atual] = cobertura_linha[linha_atual] + 1;
    }
}

// Operação inversa de adicionar_coluna: tira a coluna e desfaz as atualizações.
void remover_coluna(int coluna){

    int linha_atual;
    int coluna_vizinha;

    colunas_selecionadas[coluna] = false;
    custo_total = custo_total - tabela_de_dados[coluna].peso;

    for (int i = 0; i < tabela_de_dados[coluna].linhas.size(); i++){
        linha_atual = tabela_de_dados[coluna].linhas[i];
        cobertura_linha[linha_atual] = cobertura_linha[linha_atual] - 1;

        if (cobertura_linha[linha_atual] == 0){   // linha volta a ficar descoberta
            linhas_descobertas = linhas_descobertas + 1;
            for (int j = 0; j < lista_de_adjacencia_linhas[linha_atual].size(); j++){
                coluna_vizinha = lista_de_adjacencia_linhas[linha_atual][j];
                ganho_coluna[coluna_vizinha] = ganho_coluna[coluna_vizinha] + 1;
            }
        }
    }
}

// Diz se a coluna e redundante: todas as suas linhas ja estao cobertas por outra coluna.
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

// Critério de ordenacao: coluna mais cara primeiro.
bool comparar_por_peso_desc(int a, int b){
    return tabela_de_dados[a].peso > tabela_de_dados[b].peso;
}

// Remove colunas redundantes, das mais caras para as mais baratas, baixando o custo sem descobrir linhas.
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

// Tira um retrato do estado de trabalho atual e devolve uma Solução esparsa (denso -> esparso).
Solucao salvar_solucao(){

    Solucao solucao;
    solucao.custo = custo_total;

    for (int i = 0; i < colunas; i++){
        if (colunas_selecionadas[i]){
            solucao.colunas.push_back(i);
        }
    }
    return solucao;
}

// Reconstrói o estado de trabalho a partir de uma Solução (esparso -> denso). Ponte para os operadores do genetico.
void carregar_solucao(const Solucao& solucao){

    inicializar_solucao();

    for (int i = 0; i < solucao.colunas.size(); i++){
        adicionar_coluna(solucao.colunas[i]);
    }
}

// Score de uma coluna segundo uma das 7 funções (relação custo / cobertura). Menor = melhor.
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

// Construção gulosa determinística: a cada passo pega a melhor coluna ate cobrir todas as linhas.
void construcao_gulosa(int funcao){

    int melhor_coluna;
    float melhor_valor;
    float valor_atual;

    inicializar_solucao();

    while (linhas_descobertas > 0){
        melhor_coluna = -1;
        melhor_valor = 0;
        for (int i = 0; i < colunas; i++){
            if (ganho_coluna[i] > 0){   // só colunas que ainda cobrem algo novo
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

// Construção gulosa randomizada (GRASP): monta a lista restrita das melhores e sorteia uma. alpha controla
// o equilíbrio entre qualidade (alpha baixo, perto do guloso) e diversidade (alpha alto, mais aleatorio).
void construcao_gulosa_randomizada(int funcao, float alpha){

    float menor;
    float maior;
    bool tem_finito;
    float valor;
    float limite;
    int escolhida;

    inicializar_solucao();

    while (linhas_descobertas > 0){
        candidatos.clear();
        scores.clear();
        menor = 0;
        maior = 0;
        tem_finito = false;

        // Pontua os candidatos e guarda o melhor e o pior score finito (ignora inf das funcoes log).
        for (int i = 0; i < colunas; i++){
            if (ganho_coluna[i] > 0){
                valor = avaliar_coluna(i, funcao);
                candidatos.push_back(i);
                scores.push_back(valor);
                if (isfinite(valor)){
                    if (!tem_finito){
                        menor = valor;
                        maior = valor;
                        tem_finito = true;
                    } else {
                        if (valor < menor) menor = valor;
                        if (valor > maior) maior = valor;
                    }
                }
            }
        }

        // Monta a lista restrita: colunas com score dentro do limite relativo definido por alpha.
        lista_restrita.clear();
        if (tem_finito){
            limite = menor + alpha * (maior - menor);
            for (int i = 0; i < candidatos.size(); i++){
                if (isfinite(scores[i]) && scores[i] <= limite){
                    lista_restrita.push_back(candidatos[i]);
                }
            }
        } else {
            for (int i = 0; i < candidatos.size(); i++){   // todos com score infinito: aceita todos
                lista_restrita.push_back(candidatos[i]);
            }
        }

        // Sorteia uma coluna da lista restrita e a adiciona.
        uniform_int_distribution<int> distribuicao(0, (int)lista_restrita.size() - 1);
        escolhida = lista_restrita[distribuicao(gerador)];
        adicionar_coluna(escolhida);
    }
}

// Conta quantas colunas estao na solucao atual (usado so para relatorio).
int contar_colunas_selecionadas(){

    int total = 0;
    for (int i = 0; i < colunas; i++){
        if (colunas_selecionadas[i]){
            total = total + 1;
        }
    }
    return total;
}

// Roda várias construcoes randomizadas de uma config, mede melhor/media/diversidade e guarda a melhor global.
void experimento_randomizado(int funcao, float alpha, int repeticoes){

    set<vector<int>> distintas;
    float melhor_custo = -1;
    float soma = 0;
    Solucao solucao;

    for (int r = 0; r < repeticoes; r++){
        construcao_gulosa_randomizada(funcao, alpha);
        eliminar_redundancia();
        solucao = salvar_solucao();
        distintas.insert(solucao.colunas);   // conjunto ignora repetidas, contando so as distintas
        soma = soma + solucao.custo;
        if (melhor_custo < 0 || solucao.custo < melhor_custo){
            melhor_custo = solucao.custo;
        }
    }

    cout << "Funcao " << funcao
         << "  alpha " << fixed << setprecision(2) << alpha
         << ":   melhor = " << setprecision(2) << melhor_custo
         << "   media = " << soma / repeticoes
         << "   distintas = " << distintas.size() << "/" << repeticoes << endl;

    if (melhor_global < 0 || melhor_custo < melhor_global){
        melhor_global = melhor_custo;
        func_global = funcao;
        alpha_global = alpha;
    }
}

// Gera a populacao final: solucoes distintas com a melhor config, ate atingir o alvo ou esgotar tentativas.
void construir_populacao(int funcao, float alpha, int alvo, int max_tentativas){

    set<vector<int>> vistas;
    int tentativas = 0;
    Solucao solucao;

    populacao.clear();

    while ((int)populacao.size() < alvo && tentativas < max_tentativas){
        construcao_gulosa_randomizada(funcao, alpha);
        eliminar_redundancia();
        solucao = salvar_solucao();
        if (vistas.insert(solucao.colunas).second){   // second == true: solucao ainda nao vista
            populacao.push_back(solucao);
        }
        tentativas = tentativas + 1;
    }
}

int main(){

    // Carrega a instancia e monta as estruturas de apoio.
    construcao_da_tabela("./Tabela/Teste_01.dat");
    montar_lista_de_adjacencia();

    cout << fixed << setprecision(2);
    cout << "Linhas: " << linhas << "   Colunas: " << colunas << endl;

    // Baseline: solucao de cada uma das 7 funcoes gulosas deterministicas.
    cout << endl << "Construcao Gulosa deterministica - 7 funcoes" << endl << endl;
    for (int funcao = 1; funcao <= 7; funcao++){
        construcao_gulosa(funcao);
        float guloso = custo_total;
        eliminar_redundancia();
        cout << "Funcao " << funcao
             << ":   guloso = " << setprecision(2) << guloso
             << "   pos-reducao = " << custo_total
             << " (" << contar_colunas_selecionadas() << " col)" << endl;
    }

    // Varredura: testa cada funcao com varios alpha para descobrir a config mais promissora.
    cout << endl << "Construcao Gulosa Randomizada (GRASP) - varredura de alpha" << endl << endl;
    float alphas[] = {0.10f, 0.20f, 0.30f};
    int repeticoes = 50;
    for (int funcao = 1; funcao <= 7; funcao++){
        for (int a = 0; a < 3; a++){
            gerador.seed(SEMENTE);   // mesma semente por config para comparacao justa
            experimento_randomizado(funcao, alphas[a], repeticoes);
        }
    }

    cout << endl << "Melhor configuracao: funcao " << func_global
         << "  alpha " << fixed << setprecision(2) << alpha_global
         << "  (melhor custo " << setprecision(2) << melhor_global << ")" << endl;

    // Monta a populacao final com a melhor config encontrada.
    gerador.seed(SEMENTE);
    construir_populacao(func_global, alpha_global, 50, 1000);

    float melhor = -1;
    float soma = 0;
    for (int i = 0; i < populacao.size(); i++){
        soma = soma + populacao[i].custo;
        if (melhor < 0 || populacao[i].custo < melhor){
            melhor = populacao[i].custo;
        }
    }

    cout << endl << "Populacao final: " << populacao.size() << " solucoes distintas" << endl;
    cout << "Melhor custo = " << setprecision(2) << melhor
         << "   custo medio = " << soma / populacao.size() << endl;

    return 0;
}

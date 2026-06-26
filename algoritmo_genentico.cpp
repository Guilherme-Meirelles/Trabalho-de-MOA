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
#include <chrono>
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

Sorteados sortear_de_0_a_n_real(float n)
{

    uniform_real_distribution<> dist(0.0, 100.0);
    Sorteados resultado = {dist(gerador), dist(gerador)};
    return resultado;
}

int sortear_de_0_a_n(int n)
{
    uniform_int_distribution<> dist(0, n);
    int resultado = dist(gerador);
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

    int n_original = selecionadas.size();   // so as colunas ja existentes entram na checagem de redundancia

    int tamanho_adicional = colunas_adicionadas.size();

    for (int i = 0; i < tamanho_adicional; i++){
        if (!colunas_selecionadas[colunas_adicionadas[i]]){   // evita readicionar coluna ja presente (corromperia cobertura/custo)
            adicionar_coluna(colunas_adicionadas[i]);
        }
    }

    for (int i = 0; i < n_original; i++){
        coluna_atual = selecionadas[i];
        if (coluna_redundante(coluna_atual)){
            remover_coluna(coluna_atual);
        }
    }
}


Solucao mutacao(Solucao filho, int constante){

    
    
    int tamanho_da_mutacao = sortear_de_0_a_n(int(filho.colunas.size()) / constante) + 1;
    int  numero_sorteado;
    vector<int> colunas_sorteadas;

    for (int i = 0; i < tamanho_da_mutacao; i++){
        numero_sorteado = sortear_de_0_a_n(colunas-1);
        colunas_sorteadas.push_back(numero_sorteado);
    }

    
    adicionar_colunas_e_eliminar_mutacoes(colunas_sorteadas);
    return salvar_solucao();
}

// Quantas rodadas de ruina+reconstrucao a busca local tenta por solucao (diversificacao).
const int RODADAS_RUINA = 5;

// Quais vizinhancas a busca local usa (bitmask, para o estudo de estrategias):
// bit 0 = vizinhanca A (troca), bit 1 = vizinhanca B (ruina). 3 = ambas (padrao).
int modo_busca_local = 3;

// ----------------------------------------------------------------------------
// Busca local
// ----------------------------------------------------------------------------

// Vizinhanca A - troca (1,1) guiada por cobertura.
// Para cada coluna escolhida c, olha suas linhas exclusivas (linhas onde cobertura == 1,
// ou seja, so c as cobre). Se nao houver exclusivas, c e redundante e sai. Caso contrario,
// procura UMA coluna mais barata que cubra todas as exclusivas de c e troca c por ela.
// Aproveita o estado incremental: detectar exclusivas e O(linhas de c). Repete enquanto melhora.
void busca_local_troca(){

    vector<char> eh_exclusiva(linhas, 0);
    vector<int> escolhidas;
    vector<int> exclusivas;
    bool melhorou = true;

    while (melhorou){
        melhorou = false;

        escolhidas.clear();
        for (int i = 0; i < colunas; i++){
            if (colunas_selecionadas[i]){
                escolhidas.push_back(i);
            }
        }

        for (int idx = 0; idx < (int)escolhidas.size(); idx++){
            int c = escolhidas[idx];
            if (!colunas_selecionadas[c]) continue;   // pode ter saido numa troca anterior

            // Linhas exclusivas de c (so c as cobre).
            exclusivas.clear();
            for (int j = 0; j < (int)tabela_de_dados[c].linhas.size(); j++){
                int l = tabela_de_dados[c].linhas[j];
                if (cobertura_linha[l] == 1){
                    exclusivas.push_back(l);
                }
            }

            if (exclusivas.empty()){          // c e redundante: remover ja reduz o custo
                remover_coluna(c);
                melhorou = true;
                continue;
            }

            // Pivo: a linha exclusiva com menos colunas candidatas (reduz o trabalho da busca).
            int pivo = exclusivas[0];
            for (int e = 1; e < (int)exclusivas.size(); e++){
                if (lista_de_adjacencia_linhas[exclusivas[e]].size() < lista_de_adjacencia_linhas[pivo].size()){
                    pivo = exclusivas[e];
                }
            }

            for (int e = 0; e < (int)exclusivas.size(); e++){
                eh_exclusiva[exclusivas[e]] = 1;
            }

            // Procura a coluna mais barata (e mais barata que c) que cobre todas as exclusivas.
            int melhor_c2 = -1;
            float melhor_peso = tabela_de_dados[c].peso;
            for (int t = 0; t < (int)lista_de_adjacencia_linhas[pivo].size(); t++){
                int c2 = lista_de_adjacencia_linhas[pivo][t];
                if (c2 == c || colunas_selecionadas[c2]) continue;
                if (tabela_de_dados[c2].peso >= melhor_peso) continue;

                int cobre = 0;
                for (int j = 0; j < (int)tabela_de_dados[c2].linhas.size(); j++){
                    if (eh_exclusiva[tabela_de_dados[c2].linhas[j]]) cobre++;
                }
                if (cobre == (int)exclusivas.size()){   // cobre todas as exclusivas
                    melhor_c2 = c2;
                    melhor_peso = tabela_de_dados[c2].peso;
                }
            }

            for (int e = 0; e < (int)exclusivas.size(); e++){
                eh_exclusiva[exclusivas[e]] = 0;
            }

            if (melhor_c2 != -1){
                remover_coluna(c);
                adicionar_coluna(melhor_c2);
                melhorou = true;
            }
        }
    }
}

// Vizinhanca B - ruina e reconstrucao guiada por desperdicio.
// Desperdicio(c) = peso(c) * media( cobertura_linha - 1 ) sobre as linhas de c: prioriza
// colunas CARAS cujas linhas ja estao redundantemente cobertas. Remove as k de maior
// desperdicio, recobre gulosamente as linhas que ficaram descobertas (mesma logica do
// reparo do cruzamento) e elimina redundancia. Opera sobre o estado de trabalho atual.
void busca_local_ruina(int k){

    vector<pair<float,int>> desperdicio;   // (score, coluna)
    for (int i = 0; i < colunas; i++){
        if (!colunas_selecionadas[i]) continue;
        int n = tabela_de_dados[i].linhas.size();
        float soma = 0;
        for (int j = 0; j < n; j++){
            soma += (cobertura_linha[tabela_de_dados[i].linhas[j]] - 1);
        }
        float media = (n > 0) ? soma / n : 0.0f;
        desperdicio.push_back({ tabela_de_dados[i].peso * media, i });
    }

    sort(desperdicio.begin(), desperdicio.end(),
         [](const pair<float,int>& a, const pair<float,int>& b){ return a.first > b.first; });

    int remover = min(k, (int)desperdicio.size());
    for (int i = 0; i < remover; i++){
        remover_coluna(desperdicio[i].second);
    }

    // Reconstrucao gulosa: cobre as linhas descobertas com a melhor coluna disponivel.
    int melhor_coluna;
    float melhor_valor;
    float valor_atual;
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

    eliminar_redundancia();
}

// Busca local completa de uma solucao: intensifica com a troca (A) e depois alterna
// algumas rodadas de ruina+reconstrucao (B) com nova troca, guardando sempre a melhor.
Solucao busca_local(Solucao solucao){

    carregar_solucao(solucao);
    if (modo_busca_local & 1){
        busca_local_troca();
    }
    Solucao melhor = salvar_solucao();

    if (modo_busca_local & 2){
        for (int r = 0; r < RODADAS_RUINA; r++){
            carregar_solucao(melhor);
            int teto = max(1, (int)melhor.colunas.size() / 3);
            uniform_int_distribution<int> sorteio_k(1, teto);
            int k = sorteio_k(gerador);

            busca_local_ruina(k);
            if (modo_busca_local & 1){
                busca_local_troca();
            }

            Solucao candidata = salvar_solucao();
            if (candidata.custo < melhor.custo){
                melhor = candidata;
            }
        }
    }

    carregar_solucao(melhor);
    return melhor;
}

// ----------------------------------------------------------------------------
// Apoio ao laco geracional (steady-state)
// ----------------------------------------------------------------------------

// Indice da melhor (menor custo) solucao da populacao.
int indice_melhor_populacao(){
    int melhor = 0;
    for (int i = 1; i < (int)populacao.size(); i++){
        if (populacao[i].custo < populacao[melhor].custo){
            melhor = i;
        }
    }
    return melhor;
}

// Diz se uma solucao identica (mesmo conjunto de colunas) ja esta na populacao.
bool existe_na_populacao(const Solucao& solucao){
    for (int i = 0; i < (int)populacao.size(); i++){
        if (populacao[i].colunas == solucao.colunas){
            return true;
        }
    }
    return false;
}

// Substitui um individuo aleatorio da metade pior (custo >= mediana) pelo filho.
// Como o melhor fica abaixo da mediana, ele nunca e substituido (elitismo implicito)
// e a diversidade da populacao e preservada.
void substituir_na_populacao(const Solucao& filho){

    vector<float> custos;
    for (int i = 0; i < (int)populacao.size(); i++){
        custos.push_back(populacao[i].custo);
    }
    sort(custos.begin(), custos.end());
    float mediana = custos[custos.size() / 2];

    vector<int> piores;
    for (int i = 0; i < (int)populacao.size(); i++){
        if (populacao[i].custo >= mediana){
            piores.push_back(i);
        }
    }

    uniform_int_distribution<int> sorteio(0, (int)piores.size() - 1);
    int alvo = piores[sorteio(gerador)];
    populacao[alvo] = filho;
}

// Prepara uma instancia: le os dados e monta a populacao inicial.
//   func_fixa <= 0  -> roda a varredura (calibracao) para escolher func_global/alpha_global;
//   func_fixa  > 0  -> usa a config calibrada passada (func_fixa/alpha_fixa) e pula a varredura.
// A varredura usa sempre a SEMENTE fixa (escolha estavel entre execucoes); a montagem da
// populacao usa a 'semente' do experimento (varia entre execucoes para gerar os boxplots).
void preparar_instancia(const string& caminho, unsigned int semente, int func_fixa, float alpha_fixa, int tamanho_populacao){

    construcao_da_tabela(caminho);
    montar_lista_de_adjacencia();

    if (func_fixa <= 0){
        float alphas[] = {0.10f, 0.20f, 0.30f};
        int repeticoes = 50;
        for (int funcao = 1; funcao <= 7; funcao++){
            for (int a = 0; a < 3; a++){
                gerador.seed(SEMENTE);
                experimento_randomizado(funcao, alphas[a], repeticoes);
            }
        }
    } else {
        func_global = func_fixa;
        alpha_global = alpha_fixa;
    }

    gerador.seed(semente);
    construir_populacao(func_global, alpha_global, tamanho_populacao, 1000);
}

// Validador independente: recalcula cobertura e custo do zero (sem usar o estado
// incremental do solver), garantindo que a solucao reportada e viavel e tem o custo certo.
struct Validacao { bool viavel; int linhas_cobertas; float custo; };

Validacao validar_solucao(const Solucao& solucao){

    vector<int> cobertura(linhas, 0);
    float custo = 0;

    for (int i = 0; i < (int)solucao.colunas.size(); i++){
        int c = solucao.colunas[i];
        custo += tabela_de_dados[c].peso;
        for (int j = 0; j < (int)tabela_de_dados[c].linhas.size(); j++){
            cobertura[tabela_de_dados[c].linhas[j]]++;
        }
    }

    int cobertas = 0;
    for (int l = 0; l < linhas; l++){
        if (cobertura[l] > 0) cobertas++;
    }

    return { cobertas == linhas, cobertas, custo };
}

int main(int argc, char** argv){

    // Argumentos (todos opcionais, em ordem). Sem argumentos, comporta-se como antes.
    //   1 caminho da instancia   2 tempo limite (ms)   3 semente
    //   4 funcao gulosa fixa (<=0 = calibrar via varredura)   5 alpha fixo
    //   6 tamanho da populacao   7 modo de busca local (1=A, 2=B, 3=A+B)
    string       caminho           = (argc > 1) ? argv[1]              : "./Tabela/Teste_01.dat";
    const long   TEMPO_LIMITE_MS   = (argc > 2) ? atol(argv[2])        : 10000;
    unsigned int semente           = (argc > 3) ? (unsigned)atol(argv[3]) : SEMENTE;
    int          func_fixa         = (argc > 4) ? atoi(argv[4])        : 0;       // 0 = varredura (calibracao)
    float        alpha_fixa        = (argc > 5) ? (float)atof(argv[5]) : 0.10f;
    int          tamanho_populacao = (argc > 6) ? atoi(argv[6])        : 50;
    modo_busca_local               = (argc > 7) ? atoi(argv[7])        : 3;

    // Parametros do laco geracional.
    const int   MAX_ITER        = 50000;   // teto de filhos gerados (rede de seguranca)
    const int   MAX_ESTAGNACAO  = 5000;    // para apos N iteracoes sem melhorar o incumbente

    auto inicio = chrono::steady_clock::now();

    // Construcao da populacao inicial (define tambem func_global / alpha_global).
    preparar_instancia(caminho, semente, func_fixa, alpha_fixa, tamanho_populacao);

    // Memetico: aplica busca local em cada individuo da populacao inicial.
    Solucao melhor = populacao[indice_melhor_populacao()];
    float custo_inicial = melhor.custo;
    for (int i = 0; i < (int)populacao.size(); i++){
        populacao[i] = busca_local(populacao[i]);
        if (populacao[i].custo < melhor.custo){
            melhor = populacao[i];
        }
    }

    // Laco evolutivo steady-state: 1 filho por iteracao, substituindo a metade pior.
    int iteracoes = 0;
    int sem_melhora = 0;

    while (iteracoes < MAX_ITER
           && sem_melhora < MAX_ESTAGNACAO
           && chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - inicio).count() < TEMPO_LIMITE_MS){

        // Selecao por roleta (favorece menor custo) - funcoes ja existentes.
        vector<double> roleta = vetor_de_probabilidade(populacao);
        Sorteados numeros = sortear_de_0_a_n_real(100.0);
        solucoes_sorteadas(roleta, numeros);
        Solucao pai1 = populacao[int(solucoes_selecionadas.primeiro)];
        Solucao pai2 = populacao[int(solucoes_selecionadas.segundo)];

        // Cruzamento -> mutacao (ocasional) -> busca local.
        Solucao filho = cruzamento(pai1, pai2);
        if (sortear_de_0_a_n(9) == 9){
            filho = mutacao(filho, 3);
        }
        filho = busca_local(filho);

        // Insercao: so entra se nao for duplicata; substitui um da metade pior.
        if (!existe_na_populacao(filho)){
            substituir_na_populacao(filho);
        }

        if (filho.custo < melhor.custo){
            melhor = filho;
            sem_melhora = 0;
        } else {
            sem_melhora = sem_melhora + 1;
        }
        iteracoes = iteracoes + 1;
    }

    long tempo_ms = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - inicio).count();

    Validacao v = validar_solucao(melhor);

    cout << fixed << setprecision(2);
    cout << endl << "==== Algoritmo Genetico + Busca Local ====" << endl;
    cout << "Instancia: " << caminho << endl;
    cout << "Linhas: " << linhas << "   Colunas: " << colunas << endl;
    cout << "Construcao escolhida: funcao " << func_global
         << "   alpha " << alpha_global << endl;
    cout << "Melhor custo inicial (pos-construcao): " << custo_inicial << endl;
    cout << "Melhor custo final:                    " << melhor.custo
         << "   (" << melhor.colunas.size() << " col)" << endl;
    cout << "Iteracoes: " << iteracoes
         << "   Tempo: " << tempo_ms << " ms"
         << "   (limite " << TEMPO_LIMITE_MS << " ms)" << endl;
    cout << "Validacao: " << (v.viavel ? "VIAVEL" : "INVIAVEL")
         << "   linhas cobertas = " << v.linhas_cobertas << "/" << linhas
         << "   custo recalculado = " << v.custo << endl;

    // Linha compacta para coleta automatica (scripts de experimento).
    // Campos: instancia;custo;n_col;iteracoes;tempo_ms;limite_ms;viavel;func;alpha;semente;modo_bl;pop
    cout << "RESULTADO;" << caminho << ";" << melhor.custo << ";" << melhor.colunas.size()
         << ";" << iteracoes << ";" << tempo_ms << ";" << TEMPO_LIMITE_MS
         << ";" << (v.viavel ? 1 : 0) << ";" << func_global << ";" << alpha_global
         << ";" << semente << ";" << modo_busca_local << ";" << tamanho_populacao << endl;

    return 0;
}

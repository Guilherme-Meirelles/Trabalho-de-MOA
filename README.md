# Trabalho de MOA — Problema de Cobertura de Conjuntos (SCP)

Trabalho da disciplina de Modelagem e Otimização Algorítmica. O objetivo é resolver o
**Problema de Cobertura de Conjuntos** (*Set Covering Problem* — SCP) usando um
**Algoritmo Genético** combinado com **busca local** (abordagem memética).

Dado um conjunto de linhas que precisam ser cobertas e um conjunto de colunas (cada uma
com um custo e cobrindo algumas linhas), o objetivo é escolher um subconjunto de colunas
que cubra **todas** as linhas com o **menor custo total** possível.

---

## 1. Descrição do problema

- Entrada: `m` linhas, `n` colunas. Cada coluna `j` tem um custo `c_j` e cobre um subconjunto
  de linhas.
- Solução viável: um conjunto de colunas tal que toda linha seja coberta por pelo menos uma
  coluna escolhida.
- Objetivo: minimizar a soma dos custos das colunas escolhidas.

## 2. Instâncias e formato dos arquivos

As instâncias ficam na pasta `Tabela/` no formato `.dat`. Há nove instâncias com dois
formatos de cabeçalho ligeiramente diferentes, ambos lidos pelo mesmo parser:

```
LINHAS   50            (ou "Linhas   50")
COLUNAS  300           (ou "Colunas  500" / "COLUNA 5522")
DADOS                  (ou "Densidade  9.51 %")
   1   61.41   6  14  46        <- coluna 1: custo 61.41, cobre as linhas 6, 14, 46
   2   68.22   5  21  39 ...     <- coluna 2: custo 68.22, cobre as linhas listadas
   ...
```

O parser lê três linhas de cabeçalho (a 3ª é ignorada) e, para cada linha de dados,
interpreta o 2º campo como custo e os demais como índices de linhas cobertas (convertidos
para base 0). A integridade foi verificada: nas nove instâncias o número de colunas lidas
bate com o cabeçalho e os índices de linha estão sempre no intervalo válido.

| Instância | Linhas | Colunas | Melhor conhecido |
|---|---|---|---|
| Teste_01 | 50  | 300  | 557,44 |
| Teste_02 | 50  | 500  | 537,89 |
| Teste_03 | 50  | 700  | 517,58 |
| Teste_04 | 100 | 500  | 1162,8 |
| Teste_05 | 100 | 700  | 1020,12 |
| Wren_01  | 200 | 539  | 7856 |
| Wren_02  | 222 | 5522 | 13908 |
| Wren_03  | 219 | 4990 | 13780 |
| Wren_04  | 798 | 4569 | 58161 |

## 3. Estruturas de dados

Os dados ficam em **duas listas de adjacência complementares**:

- `tabela_de_dados[c]` — para cada **coluna**, o custo e a lista de linhas que ela cobre;
- `lista_de_adjacencia_linhas[l]` — para cada **linha**, a lista de colunas que a cobrem.

A solução em construção é acompanhada por um **estado denso atualizado de forma incremental**
(em `adicionar_coluna`/`remover_coluna`, em tempo proporcional ao nº de linhas da coluna,
sem recálculo global):

- `colunas_selecionadas` e `custo_total` — quais colunas estão escolhidas e o custo atual;
- `cobertura_linha[l]` — quantas colunas escolhidas cobrem a linha `l`;
- `linhas_descobertas` — quantas linhas ainda não estão cobertas;
- `ganho_coluna[c]` — quantas linhas ainda descobertas a coluna `c` cobriria.

Essas estruturas tornam a construção, a eliminação de redundância e a busca local rápidas.
A solução final é guardada de forma **esparsa** (`Solucao`: só os índices das colunas
escolhidas + o custo); `salvar_solucao`/`carregar_solucao` fazem a ponte denso↔esparso.

## 4. Construção da solução inicial

### 4.1. Sete funções gulosas
A cada passo o guloso escolhe a coluna com o melhor (menor) score, até cobrir todas as
linhas. Foram implementadas sete funções (relação custo × cobertura), em `avaliar_coluna`:

| Função | Score | | Função | Score |
|---|---|---|---|---|
| 1 | `c`             | | 5 | `c / (k·ln k)` |
| 2 | `c / k`         | | 6 | `c / k²` |
| 3 | `c / log₂ k`    | | 7 | `√c / k²` |
| 4 | `c / (k·log₂ k)`| | | |

onde `c` = custo e `k` = `ganho_coluna` (linhas novas cobertas).

### 4.2. Construção gulosa randomizada (GRASP)
`construcao_gulosa_randomizada(funcao, alpha)` monta a cada passo uma **lista restrita** de
candidatos com score dentro de `menor + alpha·(maior − menor)` e sorteia um. `alpha` controla
o equilíbrio entre qualidade (baixo, perto do guloso) e diversidade (alto, mais aleatório).

### 4.3. Eliminação de redundância
`eliminar_redundancia` remove colunas redundantes (cujas linhas já estão todas cobertas por
outras), das mais caras para as mais baratas, reduzindo o custo sem descobrir linhas.

### 4.4. Calibração e população inicial
A varredura `funcao 1..7 × alpha {0,10; 0,20; 0,30}` (50 repetições cada, com semente fixa)
escolhe automaticamente a melhor combinação `func_global`/`alpha_global` por instância.
`construir_populacao` gera então até 50 soluções **distintas** com essa configuração.
Como a varredura é determinística, a configuração vencedora é estável e pode ser passada
diretamente por argumento (ver seção 9), pulando a varredura — ganho grande de tempo nas
instâncias maiores.

## 5. Algoritmo Genético (steady-state)

O laço evolutivo é **steady-state**: a cada iteração gera **um** filho, aplica busca local e
o insere na população. É a abordagem que melhor funciona para o SCP (Beasley & Chu).

- **Seleção** (`vetor_de_probabilidade` + `solucoes_sorteadas`): roleta proporcional a
  `1/custo` — soluções mais baratas têm mais chance. Sorteia dois pais.
- **Cruzamento** (`cruzamento`): fusão estilo Beasley & Chu — colunas presentes nos dois pais
  são herdadas direto (consenso); as exclusivas de um pai são herdadas com probabilidade que
  favorece o pai de menor custo. O filho é reparado gulosamente (cobrindo o que sobrou) e tem
  a redundância eliminada, garantindo viabilidade.
- **Mutação** (`mutacao`): com ~10% de chance, adiciona algumas colunas aleatórias e re-elimina
  redundância, perturbando a solução.
- **Substituição** (`substituir_na_populacao`): o filho substitui um indivíduo **aleatório da
  metade pior** (custo ≥ mediana) e é **rejeitado se for duplicata**. Como o melhor fica abaixo
  da mediana, ele nunca é substituído (elitismo implícito) e a diversidade é preservada.

### Critérios de parada (combinados)
- **Estagnação** (principal): para após `MAX_ESTAGNACAO` iterações sem melhorar o incumbente;
- **Máximo de iterações** (`MAX_ITER`): rede de segurança;
- **Tempo limite**: torna justa a comparação entre instâncias e os boxplots de GAP.

## 6. Busca local (memética) — vizinhanças

A busca local é aplicada a cada filho (e à população inicial). Combina duas vizinhanças,
selecionáveis por `modo_busca_local` (1 = A, 2 = B, 3 = A+B):

### Vizinhança A — troca (1,1) guiada por cobertura (`busca_local_troca`)
Para cada coluna escolhida `c`, identifica suas **linhas exclusivas** (onde `cobertura_linha == 1`,
ou seja, só `c` as cobre). Se não houver exclusivas, `c` é redundante e sai. Caso contrário,
procura **uma coluna mais barata** que cubra todas as exclusivas de `c` e faz a troca. A
detecção das exclusivas é O(linhas de `c`) graças ao estado incremental. Repete até estabilizar.

### Vizinhança B — ruína e reconstrução guiada por desperdício (`busca_local_ruina`)
**Elemento de originalidade do trabalho.** Define um score de desperdício:

```
desperdicio(c) = peso(c) × média_sobre_linhas_de_c( cobertura_linha − 1 )
```

isto é, prioriza colunas **caras** cujas linhas já estão **redundantemente cobertas**. Remove
as `k` colunas de maior desperdício, recobre gulosamente as linhas que ficaram descobertas e
elimina redundância. Diferente do LNS aleatório clássico, o operador de destruição é
**direcionado** pela informação de sobre-cobertura que a estrutura de dados já mantém.

`busca_local` intensifica com A, depois alterna `RODADAS_RUINA` rodadas de B + A, guardando
sempre a melhor solução encontrada.

## 7. Validação independente

`validar_solucao` recalcula cobertura e custo **do zero** (sem usar o estado incremental do
solver), confirmando que cada solução reportada é viável (cobre todas as linhas) e tem o custo
correto. Na campanha de experimentos, **234 execuções foram validadas, 0 inviáveis**.

> Observação: durante o desenvolvimento, esse validador detectou um bug na mutação — ela
> readicionava uma coluna já presente, corrompendo a contagem de cobertura e gerando soluções
> inviáveis (custo aparentemente abaixo do ótimo). O bug foi corrigido com um guard mínimo.

## 8. Reprodutibilidade

A construção usa semente fixa e os sorteadores (seleção/mutação/busca local) usam um gerador
`mt19937` **semeado** (`gerador`). Assim, fixada a semente, a execução é determinística. A
variação entre execuções (para os boxplots) vem da **semente passada por argumento**.

## 9. Parâmetros por linha de comando

O executável aceita argumentos posicionais (todos opcionais; sem argumentos comporta-se como
a configuração padrão):

```
scp_run.exe <instancia> <tempo_ms> <semente> <func> <alpha> <pop> <modo_bl>
```

| Posição | Significado | Padrão |
|---|---|---|
| 1 | caminho da instância | `./Tabela/Teste_01.dat` |
| 2 | tempo limite (ms) | 10000 |
| 3 | semente | 12345 |
| 4 | função gulosa fixa (`0` = roda a varredura/calibração) | 0 |
| 5 | alpha fixo | 0,10 |
| 6 | tamanho da população | 50 |
| 7 | modo de busca local (1=A, 2=B, 3=A+B) | 3 |

Cada execução imprime um relatório legível e uma linha compacta `RESULTADO;...` para coleta
automática pelos scripts.

## 10. Experimentos e gráficos

Pasta `Resultados/`:

- `run_experimentos.ps1` — campanha completa, gera `experimentos.csv`:
  - **E1** — boxplots de GAP: 10 sementes × 9 instâncias, config calibrada, busca local A+B;
  - **E2** — estudo de vizinhanças: modos A, B e A+B nas instâncias rápidas;
  - **E3** — estudo de tamanho de população (20/50/100).
- `plot_boxplots.py` — lê o CSV e gera `graficos/boxplot_gap.png`, `graficos/vizinhancas.png`
  e `resumo_experimentos.md` (tabelas de melhor/médio/desvio/GAP/tempo).
- `resultados.md` — log completo: configuração, baseline, sensibilidade ao tempo e achados.

### Principais resultados (10 sementes por instância)
- Atingem ou superam o melhor conhecido: **Teste_03, Teste_04** (consistentemente abaixo),
  **Teste_01, Wren_01** (ótimo na melhor semente), **Teste_05, Wren_04** (melhor semente abaixo
  da referência).
- Instâncias difíceis: **Wren_02** (melhor +2,77%) e **Wren_03** (melhor +5,19%), que empacam
  em ótimo local.
- **Estudo de vizinhanças:** a vizinhança **B (ruína-reconstrução) é o principal motor de
  melhoria**; A (troca) sozinha é a mais fraca; A+B nem sempre supera B isolada.
- **População:** maior tende a ajudar (Wren_01: GAP médio 3,64% → 2,43% de pop 20 → 100).

## 11. Como compilar e executar

Compilador g++ (MSYS2 / UCRT64), C++17:

```
g++ -std=c++17 -O2 algoritmo_genentico.cpp -o scp_run.exe
./scp_run.exe ./Tabela/Teste_01.dat
```

Reproduzir toda a campanha de experimentos e gráficos:

```
powershell -ExecutionPolicy Bypass -File Resultados\run_experimentos.ps1
python Resultados\plot_boxplots.py
```

> Observação: o arquivo principal é `algoritmo_genentico.cpp`, que inclui
> `arquivo_e_estrutura_de_dados.cpp` (leitura, estruturas e construção).

## 12. Estrutura do projeto

```
arquivo_e_estrutura_de_dados.cpp  leitura, estruturas, 7 gulosas, GRASP, redundância
algoritmo_genentico.cpp           seleção, cruzamento, mutação, busca local, laço, main
Tabela/                           instâncias .dat
Resultados/
  run_experimentos.ps1            campanha de experimentos
  plot_boxplots.py                geração de boxplots e tabelas
  experimentos.csv                dados crus da campanha
  resumo_experimentos.md          tabelas-resumo
  resultados.md                   log detalhado e achados
  graficos/                       boxplot_gap.png, vizinhancas.png
```

## 13. Próximos passos

- Melhorar a **diversificação** nas instâncias Wren_02/Wren_03 (restart ao estagnar ou
  população maior), que ainda empacam em ótimo local.
- Avaliar tornar a busca local **B sozinha** o padrão em algumas instâncias, dado o resultado
  do estudo de vizinhanças.

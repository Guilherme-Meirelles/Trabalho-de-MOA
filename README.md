# Trabalho de MOA — Problema de Cobertura de Conjuntos (SCP)

Trabalho da disciplina de Modelagem e Otimização Algorítmica. O objetivo do trabalho
é resolver o Problema de Cobertura de Conjuntos (Set Covering Problem) usando
Algoritmos Genéticos combinados com busca local.

O problema consiste em: dado um conjunto de linhas que precisam ser cobertas e um
conjunto de colunas (cada uma com um custo e cobrindo algumas linhas), escolher um
subconjunto de colunas que cubra todas as linhas com o menor custo total possível.

Este repositório está em desenvolvimento. Até o momento foram implementadas a leitura
dos dados, as estruturas de dados e o algoritmo de construção das soluções.

## O que já foi feito

### Leitura dos dados
O programa lê as instâncias no formato dos arquivos `.dat` da pasta `Tabela`. O arquivo
informa a quantidade de linhas, a quantidade de colunas e, em seguida, para cada coluna,
o seu peso (custo) e a lista de linhas que ela cobre.

### Estruturas de dados
Os dados ficam guardados em duas listas de adjacência que se complementam:

- para cada **coluna**, o seu peso e quais linhas ela cobre;
- para cada **linha**, quais colunas a cobrem.

Além disso, a solução é representada e acompanhada por estruturas auxiliares que são
atualizadas de forma incremental sempre que uma coluna entra ou sai da solução:

- quais colunas estão escolhidas e o custo total atual;
- quantas colunas escolhidas cobrem cada linha (cobertura);
- quantas linhas ainda estão descobertas;
- quantas linhas ainda descobertas cada coluna cobre (ganho).

Essas estruturas deixam a construção e a futura busca local rápidas, pois evitam
recalcular tudo do zero a cada passo.

### Construção gulosa (7 funções)
A solução inicial é construída por um algoritmo guloso: a cada passo ele escolhe a
coluna com a melhor relação entre custo e cobertura e a adiciona à solução, até que
todas as linhas estejam cobertas. Foram implementadas sete funções gulosas diferentes
(variações de custo dividido pela quantidade de linhas cobertas), o que permite comparar
qual estratégia gera as melhores soluções iniciais.

### Eliminação de redundância
Depois da construção, o programa remove colunas redundantes, ou seja, colunas cujas
linhas já estão todas cobertas por outras colunas. A remoção é feita das colunas mais
caras para as mais baratas, reduzindo o custo da solução sem deixar nenhuma linha
descoberta.

### Seleção do algoritmo genético 
A partir das soluções obtidas deve se selecionar quais soluções passarão para os estágios de cruzamento e mutação do algoritmo genético, essa escolha deve ser feita probabilisticamente com a soluções de menor custo tendo mais chance de serem escolhidas. Foi implementado um algoritmo que escolhe probabilisticamente sempre duas soluções obtidas.

## Como compilar e rodar
O projeto usa C++ (compilador g++ do MSYS2 / UCRT64). Para compilar e executar:

```
g++ -std=c++17 arquivo_e_estrutura_de_dados.cpp -o arquivo_e_estrutura_de_dados.exe
./arquivo_e_estrutura_de_dados.exe
```

No VS Code também é possível compilar pela tarefa de build (Ctrl+Shift+B).



## Próximos passos
- Algoritmo Genético (população inicial, seleção, cruzamento e mutação);
- Busca local para refinar as soluções;
- Experimentos com todas as instâncias e análise dos resultados.

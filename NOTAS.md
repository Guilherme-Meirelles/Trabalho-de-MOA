# Notas para o parceiro — o que mudou

Resumo direto das últimas mudanças, para você se situar rápido. Detalhes completos no
`README.md` e em `Resultados/resultados.md`.

## ⚠️ Mudanças que encostaram no seu código (leia primeiro)

1. **Correção de bug na `mutacao`** (`algoritmo_genentico.cpp`, em
   `adicionar_colunas_e_eliminar_mutacoes`). A mutação podia sortear uma coluna **já
   presente** na solução e chamar `adicionar_coluna` de novo, contando dobrado a cobertura
   e o custo. Isso fazia a eliminação de redundância remover uma coluna realmente necessária
   e gerar **soluções inviáveis** (com custo aparentemente abaixo do ótimo). O fix é mínimo:
   um guard `if (!colunas_selecionadas[...])` antes de adicionar, e a checagem de redundância
   só sobre as colunas originais. A lógica da sua mutação foi preservada.

2. **Integração da sua revisão `e63c542`** (sorteadores reprodutíveis). Seus
   `sortear_de_0_a_n` / `sortear_de_0_a_n_real` com o `gerador` semeado foram adotados e as
   chamadas atualizadas. Resultado: execução **reprodutível** (fixada a semente, o resultado
   se repete), o que era necessário para os experimentos. O `saida.out` (binário) foi tirado
   do versionamento (entrou no `.gitignore`).

Fora esses dois pontos, **o resto do seu código (construção, cruzamento, seleção, estruturas)
não foi alterado** — só foram adicionadas funções novas.

## O que foi adicionado

- **Busca local** (`busca_local` e auxiliares):
  - Vizinhança **A** — troca (1,1) guiada por cobertura;
  - Vizinhança **B** — ruína e reconstrução guiada por desperdício (elemento de originalidade).
- **Laço genético steady-state** no `main` (gera 1 filho por iteração, busca local, substitui
  a metade pior, rejeita duplicatas). Parada por estagnação + máx. iterações + tempo limite.
- **Validador independente** (`validar_solucao`): recalcula cobertura/custo do zero e confirma
  viabilidade. Na campanha: 234 execuções, 0 inviáveis.
- **Parâmetros por linha de comando** (instância, tempo, semente, func/alpha calibrados,
  população, modo de busca local). Sem argumentos, o comportamento é o padrão.
- **Experimentos**: `Resultados/run_experimentos.ps1` (campanha) e
  `Resultados/plot_boxplots.py` (boxplots + tabelas), com os resultados já gerados.

## Mapa dos commits

| Commit | O que fez |
|---|---|
| `Adiciona busca local, laco genetico steady-state e validacao de solucoes` | Busca local (A e B), laço steady-state, validador, fix da mutação, instâncias e log |
| `Mescla revisao do parceiro (sorteadores reprodutiveis)` | Integra sua `e63c542`; remove `saida.out` |
| `Adiciona parametrizacao por argumentos, scripts de experimento e resultados` | CLI, scripts E1/E2/E3, boxplots e tabelas |
| `Acentua textos dos graficos e detalha o README` | Acentos nos gráficos e README completo |

## Como rodar

```
g++ -std=c++17 -O2 algoritmo_genentico.cpp -o scp_run.exe
./scp_run.exe ./Tabela/Teste_01.dat

# campanha completa de experimentos:
powershell -ExecutionPolicy Bypass -File Resultados\run_experimentos.ps1
python Resultados\plot_boxplots.py
```

## Principais resultados

- Atingem/superam o melhor conhecido: Teste_03, Teste_04 (sempre abaixo), Teste_01, Wren_01
  (ótimo na melhor semente), Teste_05, Wren_04.
- Ainda difíceis: Wren_02 e Wren_03 (empacam em ótimo local) — próximo alvo de melhoria.
- Estudo de vizinhanças: a vizinhança **B (ruína-reconstrução) é o principal motor de
  melhoria**; A sozinha é a mais fraca.

Ver `Resultados/graficos/` (boxplots) e `Resultados/resumo_experimentos.md` (tabelas).

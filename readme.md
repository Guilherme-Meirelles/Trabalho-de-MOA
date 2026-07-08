# Execução do arquivo

Compilação:
- g++ algoritmo_genetico.cpp -o algoritmo_genetico.exe

Execução:
- .\algoritmo_genetico.exe 

Execução definido parâmetros, exemplo:

- .\algoritmo_genetico.exe ./Tabela/Teste_02.dat 20000 4 5 0.2f 60 3 

Definição em ordem cada parâmetreo e o valor dele caso não seja inserido no comando de execução:

1. Caminho do arquivo da instância de teste utilizada : ./Tabela/Teste_01.dat
2. Tempo limite de execução do algoritmo: 10000 (ms)
3. Semente da função geradora de números aleatórios: 12345
4. Função gulosa utilizada: 0 (algoritmo verifica qual é a melhor função gulosa)
5. 𝝰(fator de aleatoriedade): 0.1
6. tamanho da população de soluções: 50
7. modo(técnica) de busca local: 3


## Execução de diversos testes

Para executar diversos testes a fim de gerar tabelas e gráficos, basta executar em ordem estas linhas de comando:

- g++ -std=c++17 -O2 algoritmo_genetico.cpp -o scp_run.exe
- powershell -ExecutionPolicy Bypass -File Resultados\run_experimentos.ps1
- python Resultados\plot_boxplots.py

A execução completa da segunda linha de comando é demorada, com o tempo variando de máquina para máquina. Pode-se levar até 1 hora.


Simulador de Processador

Visão Geral

Este projeto implementa um simulador de processador simples com suporte a um conjunto limitado de instruções. O simulador carrega instruções de um arquivo de entrada (entrada.txt), executa cada instrução sequencialmente e gera arquivos de saída contendo o estado final da unidade de controle, registradores e memória RAM.

Estrutura do Simulador

Constantes

MEM_SIZE: Tamanho da memória RAM (32 posições).

NUM_REGS: Número de registradores (4).

Variáveis Globais

memoria[MEM_SIZE]: Simula a memória RAM.

registradores[NUM_REGS]: Simula os registradores R0 a R3.

PC: Contador de programa.

IR: Registrador de instrução (instrução atual).

halted: Flag para encerrar a execução.

historicoUC: Armazena o histórico de execução da Unidade de Controle.

programaGlobal: Armazena as instruções do programa carregado.

Funcionalidades

Leitura de Programa

O programa é lido a partir de entrada.txt, ignorando linhas em branco e comentários (linhas iniciadas por #). As instruções são armazenadas no vetor programaGlobal.

Execução de Instruções

A função executarInstrucao() interpreta e executa uma linha de instrução por vez. Suporta as seguintes instruções:

LOAD Rx V: Carrega valor V (imediato ou da memória) no registrador Rx.

STORE A Rx: Armazena o valor do registrador Rx na posição de memória A.

MOVE Rx Ry: Copia o valor de Ry para Rx.

ADD Rx Ry Rz: Soma os valores de Ry e Rz, armazena o resultado em Rx.

SUB Rx Ry Rz: Subtrai Rz de Ry, armazena o resultado em Rx.

AND Rx Ry Rz: Faz operação lógica AND entre Ry e Rz, resultado em Rx.

OR Rx Ry Rz: Faz operação lógica OR entre Ry e Rz, resultado em Rx.

BRANCH L: Salta incondicionalmente para a linha L.

BEZERO L: Salta para a linha L se o valor de R0 for zero.

HALT: Encerra a execução do programa.

Saídas Geradas

Após a execução, são gerados os seguintes arquivos:

unidade_controle.txt: Histórico de instruções executadas com seus respectivos endereços.

banco_registradores.txt: Conteúdo final dos registradores R0 a R3.

memoria_ram.txt: Conteúdo final da memória de posição 0 a 31.

Funções Auxiliares

split(linha): Divide uma linha de instrução em tokens, ignorando comentários e espaços.

getRegNum(reg): Retorna o índice de um registrador (R0 a R3).

getMemAddr(addr): Converte uma string de endereço para inteiro, se for válida.

debugState(): Mostra o estado atual do processador (usada para debug).

gerarSaidas(): Gera os arquivos de saída após a execução do programa.

Execução

Escreva o programa em entrada.txt.

Compile com:

g++ -o proc_sim proc_sim.cpp

Execute:

./proc_sim

Exemplo de entrada.txt

LOAD R0 10
LOAD R1 20
ADD R2 R0 R1
STORE 5 R2
HALT

Esse exemplo soma 10 + 20, armazena o resultado em R2 e grava na memória posição 5.


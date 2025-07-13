#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

// Constantes
const int MEM_SIZE = 32;      // Tamanho da memória simulada
const int NUM_REGS = 4;       // Número de registradores (R0 a R3)

// Estado do processador
int memoria[MEM_SIZE] = {0};
int registradores[NUM_REGS] = {0}; // Registradores R0, R1, R2, R3
int PC = 0;                  // Contador de programa
string IR;                   // Registrador de instrução
bool halted = false;         // Flag para indicar se o programa terminou

// Função para dividir a linha da instrução em palavras (tokens)
vector<string> split(const string& linha)
{
  stringstream ss(linha);
  string palavra;
  vector<string> resultado;

  // Remove comentários da linha (qualquer coisa após '#')
  string linhaLimpa = linha.substr(0, linha.find('#'));

  ss.str(linhaLimpa);
  while (ss >> palavra)
  {
    // Converte para letras maiúsculas para padronização
    transform(palavra.begin(), palavra.end(), palavra.begin(), ::toupper);
    resultado.push_back(palavra);
  }

  return resultado;
}

// Extrai o número do registrador (ex: "R2" -> 2)
int getRegNum(const string& reg)
{
  if (reg.size() < 2 || reg[0] != 'R')
  {
    return -1;
  }
  return reg[1] - '0';
}

// Extrai o endereço de memória (ex: "I3" -> 3, "10" -> 10)
int getMemAddr(const string& addr)
{
  if (addr.empty())
  {
    return -1;
  }

  if (addr[0] == 'I' || addr[0] == 'B')
  {
    return stoi(addr.substr(1));
  }

  return stoi(addr);
}

// Executa uma única instrução
void executarInstrucao(const vector<string>& instrucao)
{
  if (instrucao.empty() || halted)
  {
    return;
  }

  string opcode = instrucao[0];

  if (opcode == "LOAD" && instrucao.size() >= 3)
  {
    int reg = getRegNum(instrucao[1]);
    int addr = getMemAddr(instrucao[2]);
    if (reg >= 0 && reg < NUM_REGS && addr >= 0 && addr < MEM_SIZE)
    {
      registradores[reg] = memoria[addr];
    }
  }
  else if (opcode == "STORE" && instrucao.size() >= 3)
  {
    int addr = getMemAddr(instrucao[1]);
    int reg = getRegNum(instrucao[2]);
    if (reg >= 0 && reg < NUM_REGS && addr >= 0 && addr < MEM_SIZE)
    {
      memoria[addr] = registradores[reg];
    }
  }
  else if (opcode == "MOVE" && instrucao.size() >= 3)
  {
    int reg1 = getRegNum(instrucao[1]);
    int reg2 = getRegNum(instrucao[2]);
    if (reg1 >= 0 && reg1 < NUM_REGS && reg2 >= 0 && reg2 < NUM_REGS)
    {
      registradores[reg1] = registradores[reg2];
    }
  }
  else if (opcode == "ADD" && instrucao.size() >= 4)
  {
    int reg1 = getRegNum(instrucao[1]);
    int reg2 = getRegNum(instrucao[2]);
    int reg3 = getRegNum(instrucao[3]);
    if (reg1 >= 0 && reg1 < NUM_REGS &&
        reg2 >= 0 && reg2 < NUM_REGS &&
        reg3 >= 0 && reg3 < NUM_REGS)
    {
      registradores[reg1] = registradores[reg2] + registradores[reg3];
    }
  }
  else if (opcode == "SUB" && instrucao.size() >= 4)
  {
    int reg1 = getRegNum(instrucao[1]);
    int reg2 = getRegNum(instrucao[2]);
    int reg3 = getRegNum(instrucao[3]);
    if (reg1 >= 0 && reg1 < NUM_REGS &&
        reg2 >= 0 && reg2 < NUM_REGS &&
        reg3 >= 0 && reg3 < NUM_REGS)
    {
      registradores[reg1] = registradores[reg2] - registradores[reg3];
    }
  }
  else if (opcode == "AND" && instrucao.size() >= 4)
  {
    int reg1 = getRegNum(instrucao[1]);
    int reg2 = getRegNum(instrucao[2]);
    int reg3 = getRegNum(instrucao[3]);
    if (reg1 >= 0 && reg1 < NUM_REGS &&
        reg2 >= 0 && reg2 < NUM_REGS &&
        reg3 >= 0 && reg3 < NUM_REGS)
    {
      registradores[reg1] = registradores[reg2] & registradores[reg3];
    }
  }
  else if (opcode == "OR" && instrucao.size() >= 4)
  {
    int reg1 = getRegNum(instrucao[1]);
    int reg2 = getRegNum(instrucao[2]);
    int reg3 = getRegNum(instrucao[3]);
    if (reg1 >= 0 && reg1 < NUM_REGS &&
        reg2 >= 0 && reg2 < NUM_REGS &&
        reg3 >= 0 && reg3 < NUM_REGS)
    {
      registradores[reg1] = registradores[reg2] | registradores[reg3];
    }
  }
  else if (opcode == "BRANCH" && instrucao.size() >= 2)
  {
    int addr = getMemAddr(instrucao[1]);
    if (addr >= 0 && addr < MEM_SIZE)
    {
      PC = addr - 1;
    }
  }
  else if (opcode == "BEZERO" && instrucao.size() >= 2)
  {
    if (registradores[0] == 0)
    {
      int addr = getMemAddr(instrucao[1]);
      if (addr >= 0 && addr < MEM_SIZE)
      {
        PC = addr - 1;
      }
    }
  }
  else if (opcode == "BNEG" && instrucao.size() >= 2)
  {
    if (registradores[0] < 0)
    {
      int addr = getMemAddr(instrucao[1]);
      if (addr >= 0 && addr < MEM_SIZE)
      {
        PC = addr - 1;
      }
    }
  }
  else if (opcode == "NOP")
  {
    // Sem operação
  }
  else if (opcode == "HALT")
  {
    halted = true;
    return;
  }
  else
  {
    cerr << "Instrução inválida na linha " << PC << ": " << opcode << endl;
  }

  PC++; // Avança o contador de programa
}

// Gera arquivos de saída após execução do programa
void gerarSaidas(const vector<string>& historicoUC)
{
  ofstream ucFile("unidade_controle.txt");
  for (const auto& estado : historicoUC)
  {
    ucFile << estado << endl;
  }
  ucFile.close();

  ofstream regFile("banco_registradores.txt");
  for (int i = 0; i < NUM_REGS; i++)
  {
    regFile << registradores[i] << endl;
  }
  regFile.close();

  ofstream memFile("memoria_ram.txt");
  for (int i = 0; i < MEM_SIZE; i++)
  {
    memFile << memoria[i] << endl;
  }
  memFile.close();
}

// Função principal
int main()
{
  // Inicializa memória com dados fixos (ex: idade e valor limite)
  memoria[0] = 20;   // I0 = idade
  memoria[18] = 18;  // I18 = limite de idade
  memoria[1] = 0;    // I1 = resultado final (ex: aprovado/reprovado)

  ifstream entrada("entrada.txt");
  if (!entrada.is_open())
  {
    cerr << "Erro ao abrir arquivo entrada.txt" << endl;
    return 1;
  }

  vector<string> programa;
  string linha;

  // Lê programa do arquivo até 32 instruções
  while (getline(entrada, linha) && programa.size() < MEM_SIZE)
  {
    auto instrucao = split(linha);
    if (!instrucao.empty())
    {
      programa.push_back(linha);
    }
  }
  entrada.close();

  vector<string> historicoUC;

  // Ciclo de execução de instruções
  while (PC < programa.size() && !halted)
  {
    if (PC >= programa.size())
    {
      break;
    }

    auto instrucao = split(programa[PC]);
    IR = programa[PC];

    // Guarda o estado atual para gerar relatório da unidade de controle
    historicoUC.push_back(to_string(PC) + " " + IR);

    executarInstrucao(instrucao);
  }

  // Geração dos arquivos de saída
  gerarSaidas(historicoUC);

  cout << "Simulação concluída. Arquivos gerados:" << endl;
  cout << "- unidade_controle.txt" << endl;
  cout << "- banco_registradores.txt" << endl;
  cout << "- memoria_ram.txt" << endl;

  return 0;
}

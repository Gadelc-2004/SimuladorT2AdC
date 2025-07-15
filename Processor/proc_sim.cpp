#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

const int MEM_SIZE = 32;
const int NUM_REGS = 4;

int memoria[MEM_SIZE] = {0};
int registradores[NUM_REGS] = {0};
int PC = 0;
string IR;
bool halted = false;
vector<string> historicoUC;
vector<string> programaGlobal; // Declarado globalmente para acesso em executarInstrucao

bool isNumber(const string& s) {
    if (s.empty()) return false;
    size_t pos = 0;
    
    if (s[0] == '+' || s[0] == '-') {
        pos = 1;
        if (s.length() == 1) return false;
    }
    
    return s.find_first_not_of("0123456789", pos) == string::npos;
}

vector<string> split(const string& linha) {
    stringstream ss(linha);
    string palavra;
    vector<string> resultado;

    string linhaLimpa = linha.substr(0, linha.find('#'));
    ss.str(linhaLimpa);
    
    while (ss >> palavra) {
        transform(palavra.begin(), palavra.end(), palavra.begin(), ::toupper);
        resultado.push_back(palavra);
    }

    return resultado;
}

int getRegNum(const string& reg) {
    if (reg.size() < 2 || reg[0] != 'R') {
        return -1;
    }
    return reg[1] - '0';
}

int getMemAddr(const string& addr) {
    if (addr.empty()) {
        return -1;
    }

    if (addr[0] == 'I' || addr[0] == 'B') {
        return stoi(addr.substr(1));
    }

    return stoi(addr);
}

void executarInstrucao(const vector<string>& instrucao) {
    if (instrucao.empty() || halted) {
        return;
    }

    string opcode = instrucao[0];

    if (opcode == "LOAD" && instrucao.size() >= 3) {
        int reg = getRegNum(instrucao[1]);
        int addr = getMemAddr(instrucao[2]);
        if (reg >= 0 && reg < NUM_REGS && addr >= 0 && addr < MEM_SIZE) {
            registradores[reg] = memoria[addr];
        }
    }
    else if (opcode == "STORE" && instrucao.size() >= 3) {
        int addr = getMemAddr(instrucao[1]);
        int reg = getRegNum(instrucao[2]);
        if (reg >= 0 && reg < NUM_REGS && addr >= 0 && addr < MEM_SIZE) {
            memoria[addr] = registradores[reg];
        }
    }
    else if (opcode == "MOVE" && instrucao.size() >= 3) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        if (reg1 >= 0 && reg1 < NUM_REGS && reg2 >= 0 && reg2 < NUM_REGS) {
            registradores[reg1] = registradores[reg2];
        }
    }
    else if (opcode == "ADD" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 >= 0 && reg1 < NUM_REGS &&
            reg2 >= 0 && reg2 < NUM_REGS &&
            reg3 >= 0 && reg3 < NUM_REGS) {
            registradores[reg1] = registradores[reg2] + registradores[reg3];
        }
    }
    else if (opcode == "SUB" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 >= 0 && reg1 < NUM_REGS &&
            reg2 >= 0 && reg2 < NUM_REGS &&
            reg3 >= 0 && reg3 < NUM_REGS) {
            registradores[reg1] = registradores[reg2] - registradores[reg3];
        }
    }
    else if (opcode == "AND" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 >= 0 && reg1 < NUM_REGS &&
            reg2 >= 0 && reg2 < NUM_REGS &&
            reg3 >= 0 && reg3 < NUM_REGS) {
            registradores[reg1] = registradores[reg2] & registradores[reg3];
        }
    }
    else if (opcode == "OR" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 >= 0 && reg1 < NUM_REGS &&
            reg2 >= 0 && reg2 < NUM_REGS &&
            reg3 >= 0 && reg3 < NUM_REGS) {
            registradores[reg1] = registradores[reg2] | registradores[reg3];
        }
    }
    else if (opcode == "BRANCH" && instrucao.size() >= 2) {
        int addr = getMemAddr(instrucao[1]);
        // Verificação crítica: endereço deve ser válido no programa
        if (addr >= 0 && addr < programaGlobal.size()) {
            PC = addr;
            return;  // Não incrementa PC
        }
    }
    else if (opcode == "BEZERO" && instrucao.size() >= 2) {
        if (registradores[0] == 0) {
            int addr = getMemAddr(instrucao[1]);
            // Verificação crítica: endereço deve ser válido no programa
            if (addr >= 0 && addr < programaGlobal.size()) {
                PC = addr;
                return;  // Não incrementa PC
            }
        }
    }
    else if (opcode == "BNEG" && instrucao.size() >= 2) {
        if (registradores[0] < 0) {
            int addr = getMemAddr(instrucao[1]);
            // Verificação crítica: endereço deve ser válido no programa
            if (addr >= 0 && addr < programaGlobal.size()) {
                PC = addr;
                return;  // Não incrementa PC
            }
        }
    }
    else if (opcode == "NOP") {
        // Sem operação
    }
    else if (opcode == "HALT") {
        halted = true;
        return;
    }
    else {
        cerr << "Instrução inválida na linha " << PC << ": " << opcode << endl;
    }

    PC++;
}

void gerarSaidas() {
    // unidade_controle.txt: histórico completo de PC e IR
    ofstream ucFile("unidade_controle.txt");
    for (const auto& linha : historicoUC) {
        ucFile << linha << endl;
    }
    ucFile.close();

    // banco_registradores.txt: Valores finais de R0 a R3
    ofstream regFile("banco_registradores.txt");
    for (int i = 0; i < NUM_REGS; i++) {
        regFile << registradores[i] << endl;
    }
    regFile.close();

    // memoria_ram.txt: Valores finais de M[0] a M[31]
    ofstream memFile("memoria_ram.txt");
    for (int i = 0; i < MEM_SIZE; i++) {
        memFile << memoria[i] << endl;
    }
    memFile.close();
}

int main() {
    fill_n(memoria, MEM_SIZE, 0);

    ifstream entrada("entrada.txt");
    if (!entrada.is_open()) {
        cerr << "Erro ao abrir arquivo entrada.txt" << endl;
        return 1;
    }

    vector<string> programa;
    string linha;
    int mem_pos = 0;
    while (getline(entrada, linha)) {
        // Remover espaços no início e fim
        size_t start = linha.find_first_not_of(" \t\r\n");
        if (start == string::npos) {
            continue; // Linha vazia
        }
        size_t end = linha.find_last_not_of(" \t\r\n");
        linha = linha.substr(start, end - start + 1);
        
        if (linha.empty() || linha[0] == '#') {
            continue;
        }

        if (isNumber(linha)) {
            if (mem_pos < MEM_SIZE) {
                memoria[mem_pos] = stoi(linha);
                mem_pos++;
            } else {
                cerr << "Memória cheia, ignorando valor: " << linha << endl;
            }
        }
        else {
            if (programa.size() < MEM_SIZE) {
                programa.push_back(linha);
            }
            else {
                cerr << "Limite de instruções atingido, ignorando: " << linha << endl;
            }
        }
    }

    entrada.close();
    programaGlobal = programa; // Armazena globalmente para acesso em executarInstrucao

    // Executar até HALT ou PC fora do programa
    while (PC < programa.size() && PC >= 0 && !halted) {
        auto instrucao = split(programa[PC]);
        IR = programa[PC];
        historicoUC.push_back(to_string(PC) + " " + IR);
        executarInstrucao(instrucao);
        
        // Verificação adicional de segurança
        if (PC < 0 || PC >= programa.size()) {
            break;
        }
    }

    gerarSaidas();

    cout << "Simulação concluída. Arquivos gerados:" << endl;
    cout << "- unidade_controle.txt" << endl;
    cout << "- banco_registradores.txt" << endl;
    cout << "- memoria_ram.txt" << endl;

    return 0;
}

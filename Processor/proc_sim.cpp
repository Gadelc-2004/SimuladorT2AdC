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
vector<string> programaGlobal;

// Função auxiliar para debug
void debugState() {
    cout << "PC: " << PC << " | Regs: ";
    for (int i = 0; i < NUM_REGS; i++) {
        cout << "R" << i << "=" << registradores[i] << " ";
    }
    cout << "| Mem[5-7]: " << memoria[5] << " " << memoria[6] << " " << memoria[7] << endl;
}

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
    int num = reg[1] - '0';
    return (num >= 0 && num < NUM_REGS) ? num : -1;
}

int getMemAddr(const string& addr) {
    if (addr.empty()) {
        return -1;
    }
    return isNumber(addr) ? stoi(addr) : -1;
}

void executarInstrucao(const vector<string>& instrucao) {
    if (instrucao.empty() || halted) {
        return;
    }

    string opcode = instrucao[0];
    cout << "Executando [" << PC << "]: " << opcode << " ";
    for (size_t i = 1; i < instrucao.size(); i++) {
        cout << instrucao[i] << " ";
    }
    cout << endl;

    if (opcode == "LOAD" && instrucao.size() >= 3) {
        int reg = getRegNum(instrucao[1]);
        if (reg == -1) {
            cerr << "Registrador inválido: " << instrucao[1] << endl;
            PC++;
            return;
        }
        
        if (isNumber(instrucao[2])) {
            int valor = stoi(instrucao[2]);
            registradores[reg] = valor;
            cout << "  Carregando valor imediato " << valor << " em R" << reg << endl;
        } else {
            int addr = getMemAddr(instrucao[2]);
            if (addr != -1 && addr < MEM_SIZE) {
                registradores[reg] = memoria[addr];
                cout << "  Carregando valor da memória[" << addr << "] = " << memoria[addr] << " em R" << reg << endl;
            } else {
                cerr << "Endereço de memória inválido: " << instrucao[2] << endl;
            }
        }
        PC++;
    }
    else if (opcode == "STORE" && instrucao.size() >= 3) {
        int addr = getMemAddr(instrucao[1]);
        int reg = getRegNum(instrucao[2]);
        if (reg != -1 && addr != -1 && addr < MEM_SIZE) {
            memoria[addr] = registradores[reg];
            cout << "  Armazenando R" << reg << " = " << registradores[reg] << " na memória[" << addr << "]" << endl;
        } else {
            cerr << "Parâmetros inválidos para STORE" << endl;
        }
        PC++;
    }
    else if (opcode == "MOVE" && instrucao.size() >= 3) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        if (reg1 != -1 && reg2 != -1) {
            registradores[reg1] = registradores[reg2];
            cout << "  Movendo R" << reg2 << " = " << registradores[reg2] << " para R" << reg1 << endl;
        }
        PC++;
    }
    else if (opcode == "ADD" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 != -1 && reg2 != -1 && reg3 != -1) {
            registradores[reg1] = registradores[reg2] + registradores[reg3];
            cout << "  ADD: R" << reg1 << " = R" << reg2 << "(" << registradores[reg2] 
                 << ") + R" << reg3 << "(" << registradores[reg3] << ") = " << registradores[reg1] << endl;
        }
        PC++;
    }
    else if (opcode == "SUB" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 != -1 && reg2 != -1 && reg3 != -1) {
            registradores[reg1] = registradores[reg2] - registradores[reg3];
            cout << "  SUB: R" << reg1 << " = R" << reg2 << "(" << registradores[reg2] 
                 << ") - R" << reg3 << "(" << registradores[reg3] << ") = " << registradores[reg1] << endl;
        }
        PC++;
    }
    else if (opcode == "AND" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 != -1 && reg2 != -1 && reg3 != -1) {
            registradores[reg1] = registradores[reg2] & registradores[reg3];
            cout << "  AND: R" << reg1 << " = R" << reg2 << "(" << registradores[reg2] 
                 << ") & R" << reg3 << "(" << registradores[reg3] << ") = " << registradores[reg1] << endl;
        }
        PC++;
    }
    else if (opcode == "OR" && instrucao.size() >= 4) {
        int reg1 = getRegNum(instrucao[1]);
        int reg2 = getRegNum(instrucao[2]);
        int reg3 = getRegNum(instrucao[3]);
        if (reg1 != -1 && reg2 != -1 && reg3 != -1) {
            registradores[reg1] = registradores[reg2] | registradores[reg3];
            cout << "  OR: R" << reg1 << " = R" << reg2 << "(" << registradores[reg2] 
                 << ") | R" << reg3 << "(" << registradores[reg3] << ") = " << registradores[reg1] << endl;
        }
        PC++;
    }
    else if (opcode == "BRANCH" && instrucao.size() >= 2) {
        int addr = stoi(instrucao[1]);
        if (addr >= 0 && addr < (int)programaGlobal.size()) {
            cout << "  Branch incondicional para " << addr << endl;
            PC = addr;
        } else {
            cerr << "Endereço de branch inválido: " << addr << endl;
            PC++;
        }
    }
    else if (opcode == "BEZERO" && instrucao.size() >= 2) {
        int addr = stoi(instrucao[1]);
        if (registradores[0] == 0) {
            cout << "  BEZERO: R0 == 0, pulando para " << addr << endl;
            if (addr >= 0 && addr < (int)programaGlobal.size()) {
                PC = addr;
            } else {
                cerr << "Endereço de branch inválido: " << addr << endl;
                PC++;
            }
        } else {
            cout << "  BEZERO: R0 != 0, continuando" << endl;
            PC++;
        }
    }
    else if (opcode == "HALT") {
        cout << "  HALT encontrado, terminando execução" << endl;
        halted = true;
        return;
    }
    else {
        cerr << "Instrução inválida: " << opcode << endl;
        PC++;
    }

    debugState();
}

void gerarSaidas() {
    ofstream ucFile("unidade_controle.txt");
    for (const auto& linha : historicoUC) {
        ucFile << linha << endl;
    }
    ucFile.close();

    ofstream regFile("banco_registradores.txt");
    for (int i = 0; i < NUM_REGS; i++) {
        regFile << "R" << i << ": " << registradores[i] << endl;
    }
    regFile.close();

    ofstream memFile("memoria_ram.txt");
    for (int i = 0; i < MEM_SIZE; i++) {
        memFile << "[" << i << "]: " << memoria[i] << endl;
    }
    memFile.close();
}

int main() {
    fill_n(memoria, MEM_SIZE, 0);
    fill_n(registradores, NUM_REGS, 0);

    ifstream entrada("entrada.txt");
    if (!entrada.is_open()) {
        cerr << "Erro ao abrir arquivo entrada.txt" << endl;
        return 1;
    }

    vector<string> programa;
    string linha;
    while (getline(entrada, linha)) {
        size_t start = linha.find_first_not_of(" \t\r\n");
        if (start == string::npos) continue;
        size_t end = linha.find_last_not_of(" \t\r\n");
        linha = linha.substr(start, end - start + 1);
        
        if (linha.empty() || linha[0] == '#') continue;

        programa.push_back(linha);
    }

    entrada.close();
    programaGlobal = programa;

    cout << "Programa carregado:" << endl;
    for (size_t i = 0; i < programa.size(); i++) {
        cout << i << ": " << programa[i] << endl;
    }
    cout << "Iniciando execução..." << endl << endl;

    while (PC < programa.size() && !halted) {
        auto instrucao = split(programa[PC]);
        IR = programa[PC];
        historicoUC.push_back(to_string(PC) + " " + IR);
        executarInstrucao(instrucao);
    }

    gerarSaidas();

    cout << endl << "Simulação concluída." << endl;
    return 0;
}

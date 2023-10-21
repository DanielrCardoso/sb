#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <map>

bool isInstruction(const std::string& token, const std::set<std::string>& instructionSet) {
    return instructionSet.find(token) != instructionSet.end();
}

int getTamanhoInstrucao(const std::string& instrucao){
    if(instrucao == "COPY"){
        return 3;
    }else if(instrucao == "STOP"){
        return 1;
    }else{
        return 2;
    }
}

int getInstructionValue(const std::string& instruction, const std::map<std::string, int>& instructionMap) {
    auto it = instructionMap.find(instruction);
    if (it != instructionMap.end()) {
        return it->second;
    } else {
        return -1;
    }
}


int main() {
    std::set<std::string> instructionSet = {"CONST","SPACE","LOAD", "STORE", "ADD", "SUB", "MUL", "DIV", "INPUT", "OUTPUT", "JMPP", "JMPZ", "STOP"};
    std::map<std::string, int> instructionMap;
    int code;

    instructionMap["ADD"] = 1;
    instructionMap["SUB"] = 2;
    instructionMap["MUL"] = 3;
    instructionMap["DIV"] = 4;
    instructionMap["JMP"] = 5;
    instructionMap["JMPN"] = 6;
    instructionMap["JMPP"] = 7;
    instructionMap["JMPZ"] = 8;
    instructionMap["COPY"] = 9;
    instructionMap["LOAD"] = 10;
    instructionMap["STORE"] = 11;
    instructionMap["INPUT"] = 12;
    instructionMap["OUTPUT"] = 13;
    instructionMap["STOP"] = 14;

    std::map<std::string,int> tabelaDeSimbolos;

    std::ifstream arquivo_asm("bin.asm");
    if (!arquivo_asm.is_open()) {
        std::cerr << "Erro ao abrir o arquivo ASM." << std::endl;
        return 1;
    }

    std::vector<std::string> data_section_lines;
    std::vector<std::string> text_section_lines;

    bool in_data_section = false;
    bool in_text_section = false;

    std::string linha;
    while (std::getline(arquivo_asm, linha)) {
        if (linha.find("SECAO DATA") != std::string::npos) {
            in_data_section = true;
            in_text_section = false;
            continue;
        } else if (linha.find("SECAO TEXT") != std::string::npos) {
            in_data_section = false;
            in_text_section = true;
            continue;
        }

        if (in_data_section) {
            data_section_lines.push_back(linha);
        } else if (in_text_section) {
            text_section_lines.push_back(linha);
        }
    }

    arquivo_asm.close();

    std::vector<std::string> tokens_data;
    std::cout << "Linhas da SECAO DATA:" << std::endl;
    for (const std::string& line : data_section_lines) {
        std::cout << line << std::endl;

        std::istringstream iss(line);
        std::string token;

        iss >> token;

        tokens_data.push_back(token);
    }

    std::cout << "-----TOKENS-----" << std::endl;
    int lc = 0;
    for (const std::string& line : tokens_data) {
        if(!isInstruction(line,instructionSet)){
            std::cout << line << std::endl;
            tabelaDeSimbolos[line]=lc;
            lc++;
        }
    }

    for (const auto& par : tabelaDeSimbolos) {
        std::cout << "Chave: " << par.first << ", Valor: " << par.second << std::endl;
    }


    std::vector<std::string> tokens_text;
    std::cout << "\nLinhas da SECAO TEXT:" << std::endl;
    for (const std::string& line : text_section_lines) {
        std::cout << line << std::endl;

        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            if(token.find(":") != std::string::npos){ //eh label
                std::string label = token;
                iss>>token;
                lc+=getTamanhoInstrucao(token);
                tabelaDeSimbolos[label]=lc;
                break;
            }else{
                lc+=getTamanhoInstrucao(token);
                break;
            }
        }
    }


    std::cout<<"----tabela de simbolos----"<<std::endl;
    for (const auto& par : tabelaDeSimbolos) {
        std::cout << "Chave: " << par.first << ", Valor: " << par.second << std::endl;
    }

    // std::cout << "-----TOKENS-----" << std::endl;
    // for (const std::string& token : tokens_text) {
    //     if(isInstruction(token,instructionSet)){
    //         lc += getTamanhoInstrucao(token)
    //         std::cout << token << std::endl;
    //     }else{
    //         lc += getTamanhoInstrucao(token)
    //     }
    // }



    std::string strObj = "";
    int op1 =0;
    int op2 =0;


    std::cout<<"----cod obj----"<<std::endl;

    for (const std::string& line : text_section_lines) {
        // std::cout << line << std::endl;

        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            if(token.find(":") != std::string::npos){ //eh label
                iss>>token;
            }
            code = getInstructionValue(token,instructionMap);
            if(code == 9){
                iss>>token;
                op1=tabelaDeSimbolos[token+":"];

                iss>>token;
                op2=tabelaDeSimbolos[token+":"];
                strObj = std::to_string(code) + " " + std::to_string(op1) + " " + std::to_string(op2);
            }else if(code == 14){
                strObj = std::to_string(code) + " ";
            }else{
                iss>>token;
                op1=tabelaDeSimbolos[token+":"];

                strObj = std::to_string(code) + " " + std::to_string(op1) + " ";
            }
            break;
        }

        std::cout<<strObj<<std::endl;
    }

    return 0;
}


//le linha 
// verifica se é secao text
// enquanto não for
// armazenar linha em um vetor de strings para secao data

//quando achar secao text armazenar string em um vetor para secao text

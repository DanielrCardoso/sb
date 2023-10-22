#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <cctype>

#include "InstructionsUtils.h"
#include "StringFilesUtils.h"

bool isLetterOrUnderscore(char c) {
    return isalpha(c) || c == '_';
}

bool validaRegraLabel(std::string token,std::set<std::string> instructionSet){

    if (token.back() == ':') token.pop_back();

    if (token.empty()) return false;
    if (!isLetterOrUnderscore(token[0])) return false;
    if (isInstruction(token,instructionSet)) return false;

    // Verifica os caracteres restantes.
    for (char c : token) {
        if (!isLetterOrUnderscore(c) && !isdigit(c)) {
            return false;
        }
    }

    return true;
}

int main() {
    std::set<std::string> instructionSet = {"CONST","SPACE","LOAD", "STORE", "ADD", "SUB", "MUL", "DIV", "INPUT", "OUTPUT", "JMPP", "JMPZ", "STOP","COPY"};
    std::set<std::string> directiveSet = {"CONST","SPACE"};
    std::map<std::string, int> instructionMap;
    std::map<int, int> tabelaDeDados;
    int code;
    int lc = 0;

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

    std::vector<std::string> assemblyLines = readFile("bin.asm");

    // Vetores para armazenar linhas da seção DATA e da seção TEXT.
    std::vector<std::string> data_section_lines;
    std::vector<std::string> text_section_lines;

    bool in_data_section = false;
    bool in_text_section = false;

    std::string linha;
    // Leitura do arquivo de entrada e separação das linhas em seções.
    for (const std::string& linha : assemblyLines) {
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

    std::vector<std::string> tokens_data;
    std::cout << "Linhas da SECAO DATA:" << std::endl;
    int idx = 0;
    for (const std::string& line : data_section_lines) {
        std::cout << line << std::endl;

        std::istringstream iss(line);
        std::string token;

        iss >> token;
      
        tokens_data.push_back(token);

        iss>>token;

        std::cout<<"identificador const/space " << token << std::endl;
        
        if(token.compare("CONST") == 0){
            iss>>token;
            std::cout<<"valor const " << token << std::endl;
            tabelaDeDados[idx] = std::stoi(token);
            idx +=1;
        }
        else if(token.compare("SPACE") == 0){
            std::vector<std::string> teste = splitString(line," ,");

            std::cout<<"teste "<< teste.size() <<std::endl;

            if(teste.size()>2){
                for(int i=1;i<std::stoi(teste[2]);i++){
                    tabelaDeDados[idx]=0;
                    idx++;
                }
            }

            tabelaDeDados[idx]=0;
            idx +=1;
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
                tabelaDeSimbolos[label]=lc;
                lc+=getTamanhoInstrucao(token);
                break;
            }else{
                lc+=getTamanhoInstrucao(token);
                break;
            }
        }
    }

    int enderecoDados = lc;
    std::cout << "-----TOKENS-----" << std::endl;
    for (const std::string& token : tokens_data) {
        if(!isInstruction(token,instructionSet)){
            if(validaRegraLabel(token,instructionSet)){
                std::cout << token << std::endl;
                tabelaDeSimbolos[token]=lc;
                lc++;
            }else{
                std::cout << "ERRO LEXICO: erro na declaracao da label: '" << token << "'"<<std::endl;
                exit(0);
            }
        }
    }

    std::cout<<"----tabela de simbolos----"<<std::endl;
    for (const auto& par : tabelaDeSimbolos) {
        std::cout << "Chave: " << par.first << ", Valor: " << par.second << std::endl;
    }

    std::string strObj = "";
    int op1 =0;
    int op2 = 0;
    std::vector<std::string> operandosCopy;


    std::cout<<"----cod obj----"<<std::endl;

    for (const std::string& line : text_section_lines) {

        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> labelComposto;

        while (iss >> token) {
            if(token.find(":") != std::string::npos){ //eh label
                if(validaRegraLabel(token,instructionSet)){
                    iss>>token;
                }
                else{
                    std::cout << "ERRO LEXICO: erro na declaracao da label: '" << token << "'"<<std::endl;
                    exit(0);
                }
            }
            code = getInstructionValue(token,instructionMap);
            if(code == 9){
                iss>>token;
                operandosCopy = splitString(token," ,");

                labelComposto = splitString(operandosCopy[0],"+");
                op1=tabelaDeSimbolos[labelComposto[0]+":"];
                if(labelComposto.size()>1){
                    op1+= std::stoi(labelComposto[1]);
                }

                labelComposto = splitString(operandosCopy[1],"+");
                op2=tabelaDeSimbolos[labelComposto[0]+":"];
                if(labelComposto.size()>1){
                    op2+= std::stoi(labelComposto[1]);
                }

                strObj += std::to_string(code) + " " + std::to_string(op1) + " " + std::to_string(op2) + " ";
            }else if(code == 14){
                strObj += std::to_string(code) + " ";
            }else{
                iss>>token;

                labelComposto = splitString(token,"+");

                op1=tabelaDeSimbolos[labelComposto[0]+":"];

                if(labelComposto.size()>1){
                    op1+= std::stoi(labelComposto[1]);
                }

                strObj += std::to_string(code) + " " + std::to_string(op1) + " ";
            }
            break;
        }

        // std::cout<<strObj<<std::endl;
    }

    std::cout<<"----tabela de dados----"<<std::endl;
    for (const auto& par : tabelaDeDados) {
        strObj += std::to_string(par.second) + " ";
        std::cout << "Chave: " << par.first + enderecoDados << ", Valor: " << par.second << std::endl;
    }

    std::cout<<strObj<<std::endl;

    return 0;
}


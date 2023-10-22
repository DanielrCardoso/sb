#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <cctype>
#include <regex>

#include "InstructionsUtils.h"
#include "StringFilesUtils.h"

bool isLetterOrUnderscore(char c) {
    return isalpha(c) || c == '_';
}

std::regex espacoRegex("\\s");

bool analisadorLexico(std::string token,std::set<std::string> instructionSet){

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


bool analisadorLabels(const std::string& token, const std::set<std::string>& instructionSet) {
    if (token.empty()) return false;
    
    size_t plusPos = token.find("+");
    
    if (plusPos != std::string::npos) {
        // Se encontramos um "+", dividimos a string em duas partes
        std::string part1 = token.substr(0, plusPos);
        std::string part2 = token.substr(plusPos + 1);

        if (part1.empty() || part2.empty()) return false;
        
        if (!isLetterOrUnderscore(part1[0]) || isInstruction(part1, instructionSet)) return false;
        if (!isdigit(part2[0]) && !isInstruction(part2, instructionSet)) return false;
    } else {
        // Se não houver um "+", verificamos a string inteira da mesma maneira
        if (!isLetterOrUnderscore(token[0]) || isInstruction(token, instructionSet)) return false;
    }

    return true;
}

bool contemApenasNumeros(const std::string& str) {
    for (char caractere : str) {
        if (!std::isdigit(caractere)) {
            return false;
        }
    }
    return true;
}


bool contemApenasNumerosComSinal(const std::string& str) {
    size_t inicio = 0;
    if (str[0] == '-' || str[0] == '+') {
        inicio = 1;
    }
    for (size_t i = inicio; i < str.length(); i++) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }

    return true;
}

bool analisadorSintaticoSecData(std::string line,std::set<std::string> instructionSet,std::set<std::string> directiveSet){
    std::istringstream iss(line);
    std::string token;
    std::string parametro;

    iss >> token;
    int quantidadeArgs = splitString(line," ").size();
    quantidadeArgs--;
    bool isConst = false;

    if(!isInstruction(token,instructionSet)){ // eh simbolo
        if(token.find(":") != std::string::npos){ //eh label
            if(!analisadorLexico(token,instructionSet)){
                std::cout << "ERRO LEXICO: erro na declaracao da label: '" << token << "'"<<std::endl;
                return false;
            }
            iss >> token;
        }else{
            std::cout << "ERRO SINTATICO: erro no token: '" << token << "'"<<std::endl;
            return false;
        }
    }else{
        std::cout << "ERRO SINTATICO: Nao e possivel utilizar : '" << token << "' como declaracao de variavel"<<std::endl;
        return false;
    }

    if(!isDirective(token, directiveSet)){
        std::cout << "ERRO SINTATICO: A instrucao : '" << token << "' nao e valida para a secao DATA."<<std::endl;
        return false;
    }

    // std::cout<<"fora dos args: "<< token << std::endl;

    if(token == "CONST"){
        isConst = true;
        quantidadeArgs--;

        // std::cout<< line << " " << quantidadeArgs << std::endl;
        if(quantidadeArgs != 1){
            std::cout << "ERRO SINTATICO: A direiva CONST exige UM argumento." <<std::endl;
            return false;
        }

    }else if(token == "SPACE"){
        quantidadeArgs--;

        // std::cout<< line << " " << quantidadeArgs << std::endl;

        if(quantidadeArgs >1){
            std::cout << "ERRO SINTATICO: A direiva SPACE aceita no maximo UM argumento." <<std::endl;
            return false;
        }
    }

    iss >> token;

    if(!isConst && quantidadeArgs>0 && !contemApenasNumeros(token)){
        std::cout << "ERRO SINTATICO: argumento invalido." <<std::endl;
        return false;
    }else if(isConst && quantidadeArgs>0 && !contemApenasNumerosComSinal(token)){
        std::cout << "ERRO SINTATICO: argumento invalido." <<std::endl;
        return false;
    }


    return true;
}

bool analisadorSintaticoSecText(std::string line,std::set<std::string> instructionSet){
    std::istringstream iss(line);
    std::string token;
    std::string parametro;

    iss >> token;

    int quantidadeArgs = splitString(line,", ").size();
    if(!isInstruction(token,instructionSet)){ // eh simbolo
        quantidadeArgs--;
        if(token.find(":") != std::string::npos){ //eh label
            if(!analisadorLexico(token,instructionSet)){
                std::cout << "ERRO LEXICO: erro na declaracao da label: '" << token << "'"<<std::endl;
                return false;
            }
            iss >> token;
        }else{
            std::cout << "ERRO SINTATICO: erro no token: '" << token << "'"<<std::endl;
            return false;
        }
    }

    if(!isInstruction(token,instructionSet)){
        std::cout << "ERRO SINTATICO: A instrucao: '" << token << "' nao esta definida na tabela de diretivas da linguagem."<<std::endl;
        return false;
    }

    if(token != "STOP"){

        iss >> parametro;

        // std::cout<< "OP "<< token << " param "<< parametro << std::endl;

        if(token != "COPY"){

            if(quantidadeArgs != 2){
                std::cout << "ERRO SINTATICO: A instrucao deve ter UM argumento."<<std::endl;
                return false;
            }
            // return true;
            if(!analisadorLabels(parametro,instructionSet)){
                std::cout << "ERRO SINTATICO: Os parametros da funcao nao seguem as regras de declaracao."<<std::endl;
                return false;           
            }

        }
        else{
            std::vector<std::string> operandos = splitString(parametro,",");
            if(operandos.size()!=2){
                    std::cout << "ERRO SINTATICO: A instrucao COPY deve receber dois parametros."<<std::endl;
                    return false;
            }
            for(auto x : operandos){
                    if(std::regex_search(x, espacoRegex)){
                        std::cout << "ERRO SINTATICO: Os parametros da funcao COPY nao devem conter espacos."<<std::endl;
                        return false;
                    }

                    if(!analisadorLabels(x,instructionSet)){
                        std::cout << "ERRO SINTATICO: Os parametros da funcao COPY nao seguem as regras de declaracao."<<std::endl;
                        return false;
                    }

            }
        }

    }else{
        if(quantidadeArgs > 1){
            std::cout << "ERRO SINTATICO: STOP não deve ter argumentos."<<std::endl;
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
    int numerolinha = 0;
    bool comprometido = false;
    // Leitura do arquivo de entrada e separação das linhas em seções.
    for (const std::string& linha : assemblyLines) {
        numerolinha ++;
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
            if(!analisadorSintaticoSecData(linha,instructionSet,directiveSet)){
                std::cout << "ERRO na linha "<< numerolinha << ": " << linha << std::endl << std::endl;
                comprometido = true;
            }
            data_section_lines.push_back(linha);
 
        } else if (in_text_section) {
            if(!analisadorSintaticoSecText(linha,instructionSet)){
                std::cout << "ERRO na linha "<< numerolinha << ": " << linha << std::endl << std::endl;
                comprometido = true;
            }
            text_section_lines.push_back(linha);
        }

    }

    if(comprometido){
        exit(0);
    }

    std::vector<std::string> tokens_data;
    // std::cout << "Linhas da SECAO DATA:" << std::endl;
    int idx = 0;
    for (const std::string& line : data_section_lines) {
        // std::cout << line << std::endl;

        std::istringstream iss(line);
        std::string token;

        iss >> token;
      
        tokens_data.push_back(token);

        iss>>token;

        // std::cout<<"identificador const/space " << token << std::endl;
        
        if(token.compare("CONST") == 0){
            iss>>token;
            // std::cout<<"valor const " << token << std::endl;
            tabelaDeDados[idx] = std::stoi(token);
            idx +=1;
        }
        else if(token.compare("SPACE") == 0){
            std::vector<std::string> teste = splitString(line," ,");

            // std::cout<<"teste "<< teste.size() <<std::endl;

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

    // for (const auto& par : tabelaDeSimbolos) {
    //     std::cout << "Chave: " << par.first << ", Valor: " << par.second << std::endl;
    // }

    std::vector<std::string> tokens_text;
    int countDataLines = 1;
    // std::cout << "\nLinhas da SECAO TEXT:" << std::endl;
    for (const std::string& line : text_section_lines) {
        countDataLines++;
        // std::cout << line << std::endl;

        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            if(token.find(":") != std::string::npos){ //eh label
                std::string label = token;
                if(tabelaDeSimbolos.find(label) != tabelaDeSimbolos.end()){
                                        std::cout<<"ERRO na linha " << countDataLines<< " : "  << line << std::endl;

                    std::cout<<"ERRO SEMANTICO: Declaracao de label duplicada: "<<label << std::endl; 
                    exit(0);
                }
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

    int idxData = 1;
    int enderecoDados = lc;
    // std::cout << "-----TOKENS-----" << std::endl;
    for (const std::string& token : tokens_data) {
        idxData++;
        if(!isInstruction(token,instructionSet)){
            // std::cout << token << std::endl;
            if(tabelaDeSimbolos.find(token) != tabelaDeSimbolos.end()){
                std::cout<<"ERRO na linha " << idxData << std::endl;
                std::cout<<"ERRO SEMANTICO: Declaracao de label duplicada: "<<token << std::endl; 
                exit(0);
            }
            tabelaDeSimbolos[token]=lc;
            lc++;
        }
    }

    // std::cout<<"----tabela de simbolos----"<<std::endl;
    // for (const auto& par : tabelaDeSimbolos) {
    //     std::cout << "Chave: " << par.first << ", Valor: " << par.second << std::endl;
    // }

    std::string strObj = "";
    int op1 =0;
    int op2 = 0;
    std::vector<std::string> operandosCopy;


    // std::cout<<"----cod obj----"<<std::endl;

    int countTextLines = 1;
    for (const std::string& line : text_section_lines) {
        countTextLines++;

        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> labelComposto;

        while (iss >> token) {
            if(token.find(":") != std::string::npos){ //eh label
                iss>>token;
            }
            code = getInstructionValue(token,instructionMap);
            if(code == 9){
                std::cout<<"entrou no copy"<<std::endl;

                iss>>token;
                operandosCopy = splitString(token," ,");

                labelComposto = splitString(operandosCopy[0],"+");

                // std::cout<<labelComposto[1]<<std::endl;


                if(!(tabelaDeSimbolos.find(labelComposto[0]+':') != tabelaDeSimbolos.end())){
                                    std::cout<<labelComposto[0]<<std::endl;

                    std::cout<<"ERRO na linha " << countTextLines<< " : "  << line << std::endl;
                    std::cout<<"ERRO SEMANTICO: Declaracao de label duplicada: "<<labelComposto[0] << std::endl; 
                    exit(0);
                }

                op1=tabelaDeSimbolos[labelComposto[0]+":"];
                if(labelComposto.size()>1){
                    op1+= std::stoi(labelComposto[1]);
                }

                labelComposto = splitString(operandosCopy[1],"+");

                if(!(tabelaDeSimbolos.find(labelComposto[0]+':') != tabelaDeSimbolos.end())){
                                    std::cout<<labelComposto[0]<<std::endl;
                    std::cout<<"ERRO na linha " << countTextLines<< " : "  << line << std::endl;

                    std::cout<<"ERRO SEMANTICO: Declaracao de label duplicada: "<<labelComposto[0] << std::endl; 
                    exit(0);
                }


                op2=tabelaDeSimbolos[labelComposto[0]+":"];
                if(labelComposto.size()>1){
                    op2+= std::stoi(labelComposto[1]);
                }

                strObj += std::to_string(code) + " " + std::to_string(op1) + " " + std::to_string(op2) + " ";
            }else if(code == 14){
                                std::cout<<"entrou no stop"<<std::endl;

                strObj += std::to_string(code) + " ";
            }else{
                                std::cout<<"entrou no geral " << code <<std::endl;

                iss>>token;

                labelComposto = splitString(token,"+");

                if(!(tabelaDeSimbolos.find(labelComposto[0]+':') != tabelaDeSimbolos.end())){
                                        std::cout<<"ERRO na linha " << countTextLines<< " : "  << line << std::endl;

                    std::cout<<"ERRO SEMANTICO: Label não declarada: "<<labelComposto[0] << std::endl; 
                    exit(0);
                }

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


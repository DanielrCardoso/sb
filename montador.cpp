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

std::tuple<std::vector<std::string>, std::vector<std::string>, bool> makeDataAndText(std::vector<std::string> assemblyLines,std::set<std::string> instructionSet,std::set<std::string> directiveSet){
    std::string linha;
    int numerolinha = 0;
    bool in_data_section = false;
    bool in_text_section = false;
    bool comprometido = false;

    // Vetores para armazenar linhas da seção DATA e da seção TEXT.
    std::vector<std::string> data_section_lines;
    std::vector<std::string> text_section_lines;


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


    return std::make_tuple(data_section_lines,text_section_lines,comprometido);

}

std::tuple<std::map<int, int>, std::vector<std::string>> criaTabelaDeDados(std::vector<std::string> data_section_lines){
    std::vector<std::string> tokens_data;
    std::map<int, int> tabelaDeDados;
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

    return std::make_tuple(tabelaDeDados,tokens_data);
}

std::tuple<std::map<std::string,int>,int,bool> criaTabelaSimbolosSecText(std::vector<std::string> text_section_lines){
    std::map<std::string,int> tabelaDeSimbolos;
    int lc = 0;
    bool comprometido = false;

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
                    comprometido=true;
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

    return std::make_tuple(tabelaDeSimbolos,lc,comprometido);
}

std::tuple<std::map<std::string,int>,int,bool> atualizaTabelaDeSimbolosSecData(std::vector<std::string> tokens_data,std::map<std::string,int> tabelaDeSimbolos,int lc,std::set<std::string> instructionSet){
    int idxData = 1;
    bool comprometido = false;
    // std::cout << "-----TOKENS-----" << std::endl;
    for (const std::string& token : tokens_data) {
        idxData++;
        if(!isInstruction(token,instructionSet)){
            // std::cout << token << std::endl;
            if(tabelaDeSimbolos.find(token) != tabelaDeSimbolos.end()){
                std::cout<<"ERRO na linha " << idxData << std::endl;
                std::cout<<"ERRO SEMANTICO: Declaracao de label duplicada: "<<token << std::endl; 
                comprometido=true;
            }
            tabelaDeSimbolos[token]=lc;
            lc++;
        }
    }
    return std::make_tuple(tabelaDeSimbolos,lc,comprometido);
}

std::string geraCodigoObjeto(std::vector<std::string> text_section_lines,std::map<std::string,int> tabelaDeSimbolos,std::map<int, int> tabelaDeDados){
    std::map<std::string, int> instructionMap;
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
    
    bool comprometido = false;
    std::string strObj = "";
    int op1 =0;
    int op2 = 0;
    int code;
    std::vector<std::string> operandosCopy;

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
                iss>>token;
                operandosCopy = splitString(token," ,");

                labelComposto = splitString(operandosCopy[0],"+");

                if(!(tabelaDeSimbolos.find(labelComposto[0]+':') != tabelaDeSimbolos.end())){
                    std::cout<<labelComposto[0]<<std::endl;

                    std::cout<<"ERRO na linha " << countTextLines<< " : "  << line << std::endl;
                    std::cout<<"ERRO SEMANTICO: Declaracao de label duplicada: "<<labelComposto[0] << std::endl; 
                    comprometido=true;
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
                    comprometido=true;
                }


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

                if(!(tabelaDeSimbolos.find(labelComposto[0]+':') != tabelaDeSimbolos.end())){
                    std::cout<<"ERRO na linha " << countTextLines<< " : "  << line << std::endl;
                    std::cout<<"ERRO SEMANTICO: Label não declarada: "<<labelComposto[0] << std::endl; 
                    comprometido=true;
                }

                op1=tabelaDeSimbolos[labelComposto[0]+":"];

                if(labelComposto.size()>1){
                    op1+= std::stoi(labelComposto[1]);
                }

                strObj += std::to_string(code) + " " + std::to_string(op1) + " ";
            }
            break;
        }
    }

    for (const auto& par : tabelaDeDados){
        strObj += std::to_string(par.second) + " ";
    }

    return strObj;

}

void finaliza(bool comprometido){
    if(comprometido){
        exit(0);   
    }
}
int main() {
    std::set<std::string> instructionSet = {"CONST","SPACE","LOAD", "STORE", "ADD", "SUB", "MUL", "DIV", "INPUT", "OUTPUT", "JMPP", "JMPZ", "STOP","COPY"};
    std::set<std::string> directiveSet = {"CONST","SPACE"};
    
    //Le o arquivo indicado e armazena cada linha em um vetor de strings.
    std::vector<std::string> assemblyLines = readFile("bin.asm");

    //Constroi e armazena as secoes DATA e TEXT.
    std::tuple<std::vector<std::string>, std::vector<std::string>, bool> resultado = makeDataAndText(assemblyLines,instructionSet,directiveSet);
    std::vector<std::string> data_section_lines = std::get<0>(resultado);
    std::vector<std::string> text_section_lines = std::get<1>(resultado);
    bool comprometido = std::get<2>(resultado); //caso exista um erro impede a criacao do arquivo objeto.


    //Constroi a tabela de dados e uma estrutura auxiilar com os tokens da secap DATA.
    std::tuple<std::map<int, int>, std::vector<std::string>> dataResult = criaTabelaDeDados(data_section_lines);
    std::map<int, int> tabelaDeDados = std::get<0>(dataResult);
    std::vector<std::string> tokens_data = std::get<1>(dataResult);

    //Constroi a tabela de simbolos com os dados da secao TEXT, alem de criar a variavel Line Count(LC)
    std::tuple<std::map<std::string,int>,int,bool> tableSimbResul = criaTabelaSimbolosSecText(text_section_lines);
    std::map<std::string,int> tabelaDeSimbolos = std::get<0>(tableSimbResul);
    int lc = std::get<1>(tableSimbResul);
    comprometido =std::get<2>(tableSimbResul);

    int enderecoDados = lc; // salva o valor de LC para usar como contador de linhas de DATA(sera utilizado para informar a linha do erro)
    
    //atualiza a tabela de simbolos com as informacoes da secao DATA.
    std::tuple<std::map<std::string,int>,int,bool> tableSimbResulData = atualizaTabelaDeSimbolosSecData(tokens_data,tabelaDeSimbolos,lc,instructionSet);
    tabelaDeSimbolos = std::get<0>(tableSimbResulData);
    lc = std::get<1>(tableSimbResulData);
    comprometido =std::get<2>(tableSimbResulData);

    finaliza(comprometido);

    std::string strObj = geraCodigoObjeto(text_section_lines,tabelaDeSimbolos,tabelaDeDados);

    std::cout<<strObj<<std::endl;

    return 0;
}


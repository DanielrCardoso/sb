#include "StringFilesUtils.h"
#include <fstream>
#include <iostream>


std::vector<std::string> splitString(const std::string& input, const std::string& delimiters) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    
    while (pos < input.length()) {
        size_t found = input.find_first_of(delimiters, pos);
        if (found == std::string::npos) {
            found = input.length();
        }
        tokens.push_back(input.substr(pos, found - pos));
        pos = found + 1;
    }
    
    return tokens;
}

std::vector<std::string> readFile(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream arquivo_asm(filename);

    if (!arquivo_asm.is_open()) {
        std::cerr << "Erro ao abrir o arquivo '" << filename << "'" << std::endl;
        return {};
    }

    std::string linha;
    while (std::getline(arquivo_asm, linha)) {
        lines.push_back(linha);
    }
    arquivo_asm.close();

    return lines;
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

bool isLetterOrUnderscore(char c) {
    return isalpha(c) || c == '_';
}


// bool isLetterOrUnderscore(char c) {
//     return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
// }

// bool isDigit(char c) {
//     return (c >= '0' && c <= '9');
// }

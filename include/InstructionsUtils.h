#ifndef INSTRUCTIONSUTILS_H
#define INSTRUCTIONSUTILS_H

#include <string>
#include <set>
#include <map>

bool isInstruction(const std::string& token, const std::set<std::string>& instructionSet);
bool isDirective(const std::string& token, const std::set<std::string>& directiveSet);
int getTamanhoInstrucao(const std::string& instrucao);
int getInstructionValue(const std::string& instruction, const std::map<std::string, int>& instructionMap);
bool contemApenasNumeros(const std::string& str);
bool contemApenasNumerosComSinal(const std::string& str);
bool isLetterOrUnderscore(char c);



#endif

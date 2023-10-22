#include "InstructionsUtils.h"

bool isInstruction(const std::string& token, const std::set<std::string>& instructionSet) {
    return instructionSet.find(token) != instructionSet.end();
}

bool isDirective(const std::string& token, const std::set<std::string>& directiveSet) {
    return directiveSet.find(token) != directiveSet.end();
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

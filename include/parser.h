#pragma once
#include <iostream>
#include <fstream>
#include <string>

#include "./circuit.h"

class Parser{
public:
    Parser(const std::string& fn);
    ~Parser();

    // bool flag: 是否解析成功
    bool parse(Circuit& circuit);

private:
    bool parseModels(Circuit& circuit);
    bool parseElements(Circuit& circuit);
    bool isDotModelLine(const std::string& line) const;

    std::ifstream file;
    std::string filename;
};

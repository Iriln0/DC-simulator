#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "../utils/string_utils.hpp"
#include "../utils/value_parser.hpp"

class Parser{
public:
    Parser(const std::string& fn): file(fn), filename(fn) {}
    ~Parser(){file.close();}

    void parse(){
        if(!file.is_open()){
            std::cerr << "Can not open file <" << filename << ">!" << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line);           // 第一行默认为注释
        while(std::getline(file, line)){
            line = trim(line);              // 去掉首尾空白
            if(line.empty()) continue;      // 忽略空行
            if(line.at(0)=='*') continue;   // 忽略注释行

            lines.push_back(line);
        }
    }

    const std::vector<std::string> getLines() const {
        return lines;
    }

private:
    std::ifstream file;
    std::string filename;
    std::vector<std::string> lines;
};

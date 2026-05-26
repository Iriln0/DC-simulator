#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include "../utils/string_utils.hpp"
#include "../utils/value_parser.hpp"

#include "../include/circuit.hpp"
#include "../elements/element.hpp"
#include "../elements/resistor.hpp"
#include "../elements/capacitor.hpp"
#include "../elements/inductor.hpp"
#include "../elements/voltageSource.hpp"
#include "../elements/currentSource.hpp"
#include "../elements/diode.hpp"
#include "../elements/bjt.hpp"
#include "../elements/mosfet.hpp"

class Parser{
public:
    Parser(const std::string& fn): file(fn), filename(fn) {}
    ~Parser(){file.close();}

    // bool flag: 是否解析成功
    bool parse(Circuit& circuit){
        if(!file.is_open()){
            std::cerr << "Can not open file <" << filename << ">!" << std::endl;
            return false;
        }

        std::string line;
        std::getline(file, line);           // 第一行默认为注释
        while(std::getline(file, line)){
            line = trim(line);              // 去掉首尾空白
            if(line.empty()) continue;      // 忽略空行
            if(line.at(0)=='*') continue;   // 忽略注释行

            std::stringstream iss(line);

            std::string element_name;
            iss >> element_name;

            if(element_name.at(0) == '.'){
                if(element_name == ".OP"){
                    circuit.addCommand(Command{CommandType::Op});
                }
                else if(element_name == ".PRINT"){
                    circuit.addCommand(Command{CommandType::Print});
                }
                else{
                    std::cerr << "Unknown command: " << line << std::endl;
                    return false;
                }
            }
            else if(element_name.at(0) == 'R'){
                std::string node1, node2, str_value;
                iss >> node1 >> node2 >> str_value;
                double value = parse_value(str_value);
                if(value == -1){
                    std::cerr << "Wrong value: " << str_value << std::endl;
                    return false;
                }
                circuit.addElement(std::make_unique<Resistor>(element_name, node1, node2, value));
            }
            else if(element_name.at(0) == 'L'){
                std::string node1, node2, str_value;
                iss >> node1 >> node2 >> str_value;
                double value = parse_value(str_value);
                if(value == -1){
                    std::cerr << "Wrong value: " << str_value << std::endl;
                    return false;
                }
                circuit.addElement(std::make_unique<Inductor>(element_name, node1, node2, value));
            }
            else if(element_name.at(0) == 'C'){
                std::string node1, node2, str_value;
                iss >> node1 >> node2 >> str_value;
                double value = parse_value(str_value);
                if(value == -1){
                    std::cerr << "Wrong value: " << str_value << std::endl;
                    return false;
                }
                circuit.addElement(std::make_unique<Capacitor>(element_name, node1, node2, value));
            }
            else if(element_name.at(0) == 'V'){
                std::string node1, node2, str_value;
                iss >> node1 >> node2 >> str_value;
                double value = parse_value(str_value);
                if(value == -1){
                    std::cerr << "Wrong value: " << str_value << std::endl;
                    return false;
                }
                circuit.addElement(std::make_unique<VoltageSource>(element_name, node1, node2, value));
            }
            else if(element_name.at(0) == 'I'){
                std::string node1, node2, str_value;
                iss >> node1 >> node2 >> str_value;
                double value = parse_value(str_value);
                if(value == -1){
                    std::cerr << "Wrong value: " << str_value << std::endl;
                    return false;
                }
                circuit.addElement(std::make_unique<CurrentSource>(element_name, node1, node2, value));
            }
            else if(element_name.at(0) == 'D'){
                std::string anode, cathode, model;
                iss >> anode >> cathode;
                if (iss >> model) {
                    circuit.addElement(
                        std::make_unique<Diode>(element_name, anode, cathode, model));
                } else {
                    circuit.addElement(
                        std::make_unique<Diode>(element_name, anode, cathode));
                }
            }
            else if(element_name.at(0) == 'Q'){
                std::string collector, base, emitter, tok4, tok5;
                iss >> collector >> base >> emitter >> tok4;
                if (iss >> tok5) {
                    circuit.addElement(std::make_unique<BJT>(
                        element_name, collector, base, emitter, tok5, tok4));
                } else {
                    circuit.addElement(std::make_unique<BJT>(
                        element_name, collector, base, emitter, tok4));
                }
            }
            else if(element_name.at(0) == 'M'){
                std::string drain, gate, source, tok4, tok5;
                iss >> drain >> gate >> source >> tok4;
                if (iss >> tok5) {
                    circuit.addElement(std::make_unique<MOSFET>(
                        element_name, drain, gate, source, tok4, tok5));
                } else {
                    circuit.addElement(std::make_unique<MOSFET>(
                        element_name, drain, gate, source, source, tok4));
                }
            }
            else{
                std::cerr << "Unknown element: " << line << std::endl;
                return false;
            }
        }

        return true;
    }

private:
    std::ifstream file;
    std::string filename;
};

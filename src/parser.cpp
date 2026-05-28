#include "../include/parser.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../utils/string_utils.hpp"
#include "../utils/value_parser.hpp"

#include "../elements/element.h"
#include "../elements/resistor.h"
#include "../elements/capacitor.h"
#include "../elements/inductor.h"
#include "../elements/voltageSource.h"
#include "../elements/currentSource.h"
#include "../elements/diode.h"
#include "../elements/bjt.h"
#include "../elements/mosfet.h"

Parser::Parser(const std::string& fn): file(fn), filename(fn) {}

Parser::~Parser(){
    file.close();
}

bool Parser::isDotModelLine(const std::string& line) const {
    std::stringstream iss(line);
    std::string token;
    if (!(iss >> token)) {
        return false;
    }
    return equals_ignore_case(token, ".model");
}

bool Parser::parseModels(Circuit& circuit) {
    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line.at(0) == '*') {
            continue;
        }
        if (isDotModelLine(line) && !circuit.models().parseLine(line)) {
            return false;
        }
    }

    return true;
}

bool Parser::parseElements(Circuit& circuit) {
    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line.at(0) == '*') {
            continue;
        }
        if (isDotModelLine(line)) {
            continue;
        }

        std::stringstream iss(line);

        std::string element_name;
        iss >> element_name;

        if (element_name.at(0) == '.') {
            if (element_name == ".OP") {
                circuit.addCommand(Command{CommandType::Op});
            } else if (element_name == ".PRINT") {
                circuit.addCommand(Command{CommandType::Print});
            } else {
                std::cerr << "Unknown command: " << line << std::endl;
                return false;
            }
        } else if (element_name.at(0) == 'R') {
            std::string node1, node2, str_value;
            iss >> node1 >> node2 >> str_value;
            double value = parse_value(str_value);
            if (value == -1) {
                std::cerr << "Wrong value: " << str_value << std::endl;
                return false;
            }
            circuit.addElement(std::make_unique<Resistor>(element_name, node1, node2, value));
        } else if (element_name.at(0) == 'L') {
            std::string node1, node2, str_value;
            iss >> node1 >> node2 >> str_value;
            double value = parse_value(str_value);
            if (value == -1) {
                std::cerr << "Wrong value: " << str_value << std::endl;
                return false;
            }
            circuit.addElement(std::make_unique<Inductor>(element_name, node1, node2, value));
        } else if (element_name.at(0) == 'C') {
            std::string node1, node2, str_value;
            iss >> node1 >> node2 >> str_value;
            double value = parse_value(str_value);
            if (value == -1) {
                std::cerr << "Wrong value: " << str_value << std::endl;
                return false;
            }
            circuit.addElement(std::make_unique<Capacitor>(element_name, node1, node2, value));
        } else if (element_name.at(0) == 'V') {
            std::string node1, node2, str_value;
            iss >> node1 >> node2 >> str_value;
            double value = parse_value(str_value);
            if (value == -1) {
                std::cerr << "Wrong value: " << str_value << std::endl;
                return false;
            }
            circuit.addElement(std::make_unique<VoltageSource>(element_name, node1, node2, value));
        } else if (element_name.at(0) == 'I') {
            std::string node1, node2, str_value;
            iss >> node1 >> node2 >> str_value;
            double value = parse_value(str_value);
            if (value == -1) {
                std::cerr << "Wrong value: " << str_value << std::endl;
                return false;
            }
            circuit.addElement(std::make_unique<CurrentSource>(element_name, node1, node2, value));
        } else if (element_name.at(0) == 'D') {
            std::string anode, cathode, model;
            iss >> anode >> cathode;
            if (iss >> model) {
                circuit.addElement(
                    std::make_unique<Diode>(element_name, anode, cathode, model));
            } else {
                circuit.addElement(
                    std::make_unique<Diode>(element_name, anode, cathode));
            }
        } else if (element_name.at(0) == 'Q') {
            std::string collector, base, emitter, tok4, tok5;
            iss >> collector >> base >> emitter >> tok4;
            if (iss >> tok5) {
                circuit.addElement(std::make_unique<BJT>(
                    element_name, collector, base, emitter, tok5, tok4));
            } else {
                circuit.addElement(std::make_unique<BJT>(
                    element_name, collector, base, emitter, tok4));
            }
        } else if (element_name.at(0) == 'M') {
            std::string drain, gate, source, tok4, tok5;
            iss >> drain >> gate >> source >> tok4;
            if (iss >> tok5) {
                circuit.addElement(std::make_unique<MOSFET>(
                    element_name, drain, gate, source, tok4, tok5));
            } else {
                circuit.addElement(std::make_unique<MOSFET>(
                    element_name, drain, gate, source, source, tok4));
            }
        } else {
            std::cerr << "Unknown element: " << line << std::endl;
            return false;
        }
    }

    return true;
}

bool Parser::parse(Circuit& circuit) {
    if (!file.is_open()) {
        std::cerr << "Can not open file <" << filename << ">!" << std::endl;
        return false;
    }

    if (!parseModels(circuit)) {
        return false;
    }

    file.clear();
    file.seekg(0);
    if (!file) {
        std::cerr << "Failed to rewind netlist file" << std::endl;
        return false;
    }

    if (!parseElements(circuit)) {
        return false;
    }

    try {
        circuit.validateModels();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return false;
    }

    return true;
}

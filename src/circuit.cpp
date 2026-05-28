#include "circuit.h"

#include <utility>

const char* commandTypeName(CommandType type){
    switch(type){
        case CommandType::Op:   return "Op";
        case CommandType::Print: return "Print";
        default: return "Unknown";
    }
    return "Unknown";
}

bool Circuit::empty() const {
    return elements.empty();
}

void Circuit::addCommand(Command command) {
    commands.push_back(std::move(command));
}

void Circuit::addElement(std::unique_ptr<Element> element) {
    elements.push_back(std::move(element));
}

size_t Circuit::elementCount() const {
    return elements.size();
}

const std::vector<std::unique_ptr<Element>>& Circuit::getElements() const {
    return elements;
}

const std::vector<Command> Circuit::getCommands() const {
    return commands;
}

void Circuit::print(std::ostream& os) const{
    os << "====================== Circuit ======================" << std::endl;
    os << "Elements: " << elements.size() << "" << std::endl;
    for(const auto& element: elements){
        // 元件名 元件类型
        os << " " << element->getName() << "    "
           << elementTypeName(element->getType()) << "    ";

        // 元件节点信息
        os << "nodes: ";
        std::vector<std::string> nodes = element->getNodes();
        for(auto& node: nodes){
            os << node << " ";
        }

        os << " ";

        switch(element->getType()){
            case ElementType::Resistor:
            case ElementType::Capacitor:
            case ElementType::Inductor:
            case ElementType::VoltageSource:
            case ElementType::CurrentSource:
                os << "value: " << element->getValue() << std::endl;
                break;
            case ElementType::BJT:
            case ElementType::Diode:
            case ElementType::MOSFET:
                os << "model: " << element->getModel() << std::endl;
                break;
            default:
                std::cerr << "Wrong type!" << std::endl;
                break;
        }
    }
    os << "-----------------------------------------------------" << std::endl;
    os << "Commands: " << commands.size() << std::endl;
    for(auto& command: commands){
        os << commandTypeName(command.type) << std::endl;
    }

    os << "=====================================================" << std::endl;
}
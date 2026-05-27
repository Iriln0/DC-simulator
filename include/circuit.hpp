#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include "../elements/element.hpp"

enum class CommandType {
    Op,
    Print
    /**
     * @todo
     * other command types to be added
     */
};

struct Command{
    CommandType type;
    /**
     * @todo
     * Something else according to the needs
     */
};

inline const char* commandTypeName(CommandType type){
    switch(type){
        case CommandType::Op:   return "Op";
        case CommandType::Print: return "Print";
        default: return "Unknown";
    }
    return "Unknown";
}

class Circuit{
public:
    Circuit() = default;
    ~Circuit() = default;

    bool empty() const {
        return elements.empty();
    }

    void addCommand(Command command) {
        commands.push_back(std::move(command));
    }   

    void addElement(std::unique_ptr<Element> element) {
        elements.push_back(std::move(element));
    }

    size_t elementCount() const {
        return elements.size();
    }

    const std::vector<std::unique_ptr<Element>>& getElements() const {
        return elements;
    }

    // 打印电路详细信息
    void print(std::ostream& os=std::cout) const{
        os << "==================== Circuit ====================" << std::endl;
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
                default:
                    break;
            }
            os << std::endl;
        }
        os << "Commands: " << commands.size() << std::endl;
        for(auto& command: commands){
            os << commandTypeName(command.type) << std::endl;
        }

        os << "=================================================" << std::endl;
    }

private:
    std::vector<Command> commands;
    std::vector<std::unique_ptr<Element>> elements;
};

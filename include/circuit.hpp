#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
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

    void print(std::ostream& os=std::cout) const{
        /**
         * @todo 根据指令进行相应的输出
         */
    }

private:
    std::vector<Command> commands;
    std::vector<std::unique_ptr<Element>> elements;
};

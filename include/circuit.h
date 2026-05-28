#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include "../elements/element.h"
#include "./model.h"

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

const char* commandTypeName(CommandType type);

class Circuit{
public:
    Circuit() = default;
    ~Circuit() = default;

    bool empty() const;

    void addCommand(Command command);

    void addElement(std::unique_ptr<Element> element);

    size_t elementCount() const;

    const std::vector<std::unique_ptr<Element>>& getElements() const;

    const std::vector<Command> getCommands() const;

    ModelLibrary& models();
    const ModelLibrary& models() const;

    bool validateModels() const;

    // 打印电路详细信息
    void print(std::ostream& os=std::cout) const;

private:
    std::vector<Command> commands;
    std::vector<std::unique_ptr<Element>> elements;
    ModelLibrary _models;
};

#pragma once

#include <vector>
#include <memory>
#include "Command.h"

class CommandManager
{
public:
    static void ExecuteCommand(std::unique_ptr<Command> command);

    static void Undo();
    static void Redo();

private:
    static std::vector<std::unique_ptr<Command>> s_UndoStack;
    static std::vector<std::unique_ptr<Command>> s_RedoStack;
};
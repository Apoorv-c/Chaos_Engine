#include "Commands/CommandManager.h"

std::vector<std::unique_ptr<Command>> CommandManager::s_UndoStack;
std::vector<std::unique_ptr<Command>> CommandManager::s_RedoStack;

void CommandManager::ExecuteCommand(std::unique_ptr<Command> command)
{
    command->Execute();

    s_UndoStack.push_back(std::move(command));

    s_RedoStack.clear();
}

void CommandManager::Undo()
{
    if (s_UndoStack.empty())
        return;

    auto command = std::move(s_UndoStack.back());

    s_UndoStack.pop_back();

    command->Undo();

    s_RedoStack.push_back(std::move(command));
}

void CommandManager::Redo()
{
    if (s_RedoStack.empty())
        return;

    auto command = std::move(s_RedoStack.back());

    s_RedoStack.pop_back();

    command->Execute();

    s_UndoStack.push_back(std::move(command));
}
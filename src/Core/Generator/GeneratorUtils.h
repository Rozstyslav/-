#pragma once
#include "stdafx.h"
#include "Utils/singleton.hpp"
#include "Core/Generator/IGeneratorItem.h"

class GeneratorUtils : public singleton<GeneratorUtils>
{
public:
    void RegisterOperation(const std::string& type, size_t priority)
    {
        m_operations[type] = priority;
    }

    void RegisterOperand(const std::string& type)
    {
        m_operands.insert(type);
    }

    void RegisterEquationEnd(const std::string& type)
    {
        m_equationEnd.insert(type);
    }

    void RegisterLBraket(const std::string& type)
    {
        m_lBraketType = type;
    }

    void RegisterRBraket(const std::string& type)
    {
        m_rBraketType = type;
    }

    std::list<std::shared_ptr<IGeneratorItem>> ConvertToPostfixForm(
        std::list<std::shared_ptr<IGeneratorItem>>::iterator& it,
        const std::list<std::shared_ptr<IGeneratorItem>>::iterator& end) const
    {
        std::list<std::shared_ptr<IGeneratorItem>> postfixForm;
        std::list<std::shared_ptr<IGeneratorItem>> stack;

        while (it != end)
        {
            auto item = *it;
            auto itemType = item->type();

            if (IsOperand(item))
            {
                postfixForm.push_back(item);
            }
            else if (IsOperation(item))
            {
                while (!stack.empty() && !Prioritet(item, stack.back()) && stack.back()->type() != m_lBraketType)
                {
                    postfixForm.push_back(stack.back());
                    stack.pop_back();
                }
                stack.push_back(item);
            }
            else if (itemType == m_lBraketType)
            {
                stack.push_back(item);
                postfixForm.push_back(item);
            }
            else if (itemType == m_rBraketType)
            {
                while (stack.back()->type() != m_lBraketType)
                {
                    postfixForm.push_back(stack.back());
                    stack.pop_back();
                }
                stack.pop_back();
                postfixForm.push_back(item);
            }
            
            if (IsNextEndOfEquation(it, end))
            {
                break;
            }

            ++it;
        }

        while (!stack.empty())
        {
            postfixForm.push_back(stack.back());
            stack.pop_back();
        }

        return postfixForm;
    }

    static void PrintResultToStack(std::ostream& out, const GeneratorDetails::GeneratorArgs& args)
    {
        out << "\tmov [esp + " << args.posArg0 << "], " << args.regPrefix << "ax\n";
        out << "\tpop ecx\n";
        out << "\tpop " << args.regPrefix << "ax\n";
        out << "\tpush ecx\n";
    }

    static bool IsNextTokenIs(const std::list<std::shared_ptr<IGeneratorItem>>::iterator& it,
        const std::list<std::shared_ptr<IGeneratorItem>>::iterator& end,
        const std::string& type)
    {
        auto res = false;
        if (it != end && std::next(it) != end && (*std::next(it))->type() == type)
            res = true;

        return res;
    }

private:
    inline bool IsOperand(const std::shared_ptr<IGeneratorItem>& item) const
    {
        return m_operands.contains(item->type());
    }

    inline bool IsOperation(const std::shared_ptr<IGeneratorItem>& item) const
    {
        return m_operations.contains(item->type());
    }

    bool Prioritet(const std::shared_ptr<IGeneratorItem>& left, const std::shared_ptr<IGeneratorItem>& right) const
    {
        size_t leftPriority = 0;
        size_t rightPriority = 0;

        if (IsOperation(left))
            leftPriority = m_operations.at(left->type());

        if (IsOperation(right))
            rightPriority = m_operations.at(right->type());

        return leftPriority > rightPriority;
    }

    bool IsNextEndOfEquation(const std::list<std::shared_ptr<IGeneratorItem>>::iterator& it,
        const std::list<std::shared_ptr<IGeneratorItem>>::iterator& end) const
    {
        auto res = true;

        if (it != end && std::next(it) != end)
        {
            auto next = *std::next(it);
            res = m_equationEnd.contains(next->type()) || IsNextTokenOnNextLine(it, end);
        }

        return res;
    }

    static bool IsNextTokenOnNextLine(const std::list<std::shared_ptr<IGeneratorItem>>::iterator& it,
        const std::list<std::shared_ptr<IGeneratorItem>>::iterator& end)
    {
        auto res = false;
        if (it != end && std::next(it) != end && ((*it)->line() + 1) == (*std::next(it))->line())
            res = true;

        return res;
    }

private:
    std::map<std::string, size_t> m_operations;
    std::set<std::string> m_operands;
    std::set<std::string> m_equationEnd;
    std::string m_lBraketType;
    std::string m_rBraketType;
};
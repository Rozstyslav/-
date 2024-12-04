#pragma once
#include "stdafx.h"
#include "Core/Tokens/TokenBase.hpp"
#include "Core/Backus/BackusRuleBase.h"
#include "Core/Generator/GeneratorItemBase.h"
#include "Rules/AssignmentRule/Assignment.h"
#include "Tokens/Common/EndOfFile.h"

class Identifier : public TokenBase<Identifier>, public BackusRuleBase<Identifier>, public GeneratorItemBase<Identifier>
{
    BASE_ITEM

public:
    Identifier() { setLexeme(""); };
    virtual ~Identifier() = default;

    std::shared_ptr<IToken> tryCreateToken(std::string& lexeme) const override
    {
        if (lexeme.size() > (m_mask.size() + m_prefix.size()))
            return nullptr;

        bool res = true;    
        if (!lexeme.starts_with(m_prefix))
        {
            return nullptr;
        }

        std::string_view ident{ lexeme.begin() + m_prefix.size(), lexeme.end() };
        for (size_t i = 0; i < ident.size(); i++)
        {
            if ((isupper(ident[i]) != isupper(m_mask[i])) && !isdigit(ident[i]))
            {
                res &= false;
                break;
            }
        }

        std::shared_ptr<IToken> token = nullptr;
        if (res)
        {
            token = clone();
            token->setValue(lexeme);
            lexeme.clear();
        }

        return token;
    };

    void genCode(std::ostream& out, GeneratorDetails& details,
        std::list<std::shared_ptr<IGeneratorItem>>::iterator& it,
        const std::list<std::shared_ptr<IGeneratorItem>>::iterator& end) const final
    {
        if (!GeneratorUtils::IsNextTokenIs(it, end, Assignment::Type()))
        {
            if ((*std::prev(end))->type() == EndOfFile::Type())
                details.registerNumberData(customData());
            else
                out << "\tpush " << customData() << std::endl;
        }
    };

private:
    const std::string m_prefix = "";
    const std::string m_mask = "xxxxxxxxxxxxxxxx";
};
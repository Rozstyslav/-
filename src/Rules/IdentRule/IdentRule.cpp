#include "stdafx.h"
#include "IdentRule.h"

#include "Rules/IdentRule/Identifier.h"
#include "Rules/IdentRule/Undefined.h"

SimpleToken(ProgramName, "");

BackusRulePtr MakeIdentRule(std::shared_ptr<Controller> controller)
{
    using enum ItemType;

    controller->regItem<Identifier>(TokenAndRule, -1);

    GeneratorUtils::Instance()->RegisterOperand(Identifier::Type());

    auto context = controller->context();

    auto identRule = controller->addRule(context->IdentRuleName(), {
        BackusRuleItem({ Identifier::Type()}, OnlyOne)
        });

    identRule->setPostHandler([context](BackusRuleList::iterator&,
        BackusRuleList::iterator& it,
        BackusRuleList::iterator& end)
    {
        static bool isFirstIdentChecked = !context->IsFirstProgName();
        auto isVarBlockChecked = context->IsVarBlockChecked();
        auto& identTable = context->IdentTable();

        auto identIt = std::prev(it, 1);
        if (isVarBlockChecked)
        {
            if (!identTable.contains((*identIt)->value()))
            {
                auto undef = std::make_shared<Undefined>();
                undef->setValue((*identIt)->value());
                undef->setLine((*identIt)->line());
                undef->setCustomData((*identIt)->customData());
                *identIt = undef;
            }
        }
        else
        {
             identTable.insert((*identIt)->value());
        }

        (*identIt)->setCustomData((*identIt)->value() + "_");
    });

    return identRule;
}
#pragma once
#include "stdafx.h"
#include "Core/Tokens/TokenBase.hpp"
#include "Core/Backus/BackusRuleBase.h"
#include "Core/Generator/GeneratorItemBase.h"

class VarType : public TokenBase<VarType>, public BackusRuleBase<VarType>, public GeneratorItemBase<VarType>
{
    BASE_ITEM

public:
    VarType() { setLexeme("longint"); };
    virtual ~VarType() = default;
};
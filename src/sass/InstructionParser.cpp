/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "InstructionParser.h"
#include <cctype>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include "Instruction.h"
#include "UTF8.h"
#include <stdexcept>
#include <iostream>
#include "Token.h"
#include "Fixup.h"
extern const unsigned BitMasks[32];

int InstructionParser::processorMode;

bool InstructionParser::ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos)
{
    if (inputTokens[tokenPos]->type == InputToken::NUMBER)
    {
        AsmExprNode* val = (AsmExprNode*)inputTokens[tokenPos]->val;
        bool isConst = val->GetType() != AsmExprNode::LABEL;
        if (isConst || !needConstant)
        {
            if (isConst)
            {
                if (val->GetType() != AsmExprNode::IVAL)
                    return false;
                if (sign)
                {
                    if (bits == 8 && (val->ival & ~BitMasks[bits - 2]) != 0)
                        if ((val->ival & ~BitMasks[bits - 2]) != (~BitMasks[bits - 2]))
                            if ((val->ival & ~BitMasks[bits - 2]) != 0)
                                return false;
                }
                else
                {
                    if (bits == 8 && (val->ival & ~BitMasks[bits - 1]) != 0)
                        if ((val->ival & ~BitMasks[bits - 2]) != (~BitMasks[bits - 2]))
                            return false;
                }
            }
            numeric = new Numeric;
            numeric->node = val;
            numeric->pos = 0;
            numeric->relOfs = relOfs;
            numeric->size = bits;
            numeric->used = false;
            return true;
        }
    }
    return false;
}
bool InstructionParser::SetNumber(int tokenPos, int oldVal, int newVal)
{
    bool rv = false;
    if (tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::NUMBER)
    {
        AsmExprNode* val = (AsmExprNode*)inputTokens[tokenPos]->val;
        bool isConst = val->GetType() != AsmExprNode::LABEL;
        if (isConst)
        {
            if (val->GetType() == AsmExprNode::IVAL)
            {
                if (val->ival == oldVal)
                {
                    numeric = new Numeric(new AsmExprNode(newVal));
                    rv = true;
                }
            }
        }
    }
    return rv;
}


bool InstructionParser::MatchesOpcode(std::string opcode)
{
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);

    if (opcodeTable.end() != opcodeTable.find(opcode) || prefixTable.end() != prefixTable.find(opcode))
        return true;
    return false;
}
std::unordered_map<std::string, int>::iterator InstructionParser::GetOpcode(const std::string& opcode)
{
    auto it = opcodeTable.find(opcode);
    return it;
}
Instruction* InstructionParser::Parse(std::string& line)
{
    std::string op;
    inputTokens.clear();

    for (auto c : CleanupValues)
        delete c;
    for (auto o : operands)
        delete o;
    CleanupValues.clear();
    operands.clear();
    prefixes.clear();
    numeric = nullptr;
    while (true)
    {
        int npos = line.find_first_of(" \t\v\r\n");
        if (npos != std::string::npos)
        {
            op = line.substr(0, npos);
            line = line.substr(npos);
            npos = line.find_first_not_of(" \t\v\r\n");
            if (npos == std::string::npos)
            {
                line = "";
            }
            else
            {
                line.replace(0, npos, "");
            }
        }
        else
        {
            op = line;
            line = "";
        }
        auto it = prefixTable.find(op);
        if (it != prefixTable.end())
        {
            prefixes.push_back(it->second);
        }
        else
        {
            break;
        }
    }
    std::transform(op.begin(), op.end(), op.begin(), ::tolower);

    if (op == "")
    {
        auto rv = DispatchOpcode(-1);
        if (rv == AERR_NONE)
        {
            unsigned char buf[64];
            return new Instruction(buf, 0);
        }
    }
    else
    {
        auto it = GetOpcode(op);
        if (it != opcodeTable.end())
        {
            bits.Reset();
            int opcode = it->second;
            if (!Tokenize(line))
            {
                throw new std::runtime_error("Unknown token sequence");
            }
            eol = false;
            auto rv = DispatchOpcode(opcode);
            Instruction* s = nullptr;
            switch (rv)
            {
                case AERR_NONE: {
                    unsigned char buf[32];
                    bits.GetBytes(buf, 32);
#ifdef XXXXX
                    std::cout << std::hex << bits.GetBits() << " ";
                    for (int i = 0; i < bits.GetBits() >> 3; i++)
                        std::cout << std::hex << (int)buf[i] << " ";
                    std::cout << std::endl;
#endif
                    if (!eol)
                        throw new std::runtime_error("Extra characters at end of line");
                    s = new Instruction(buf, (bits.GetBits() + 7) / 8);
                    //			std::cout << bits.GetBits() << std::endl;
                    for (auto& operand : operands)
                    {
                        if (operand->used && operand->size)
                        {
                            if (((AsmExprNode*)operand->node)->GetType() != AsmExprNode::IVAL &&
                                ((AsmExprNode*)operand->node)->GetType() != AsmExprNode::FVAL)
                            {
                                if (s->Lost() && operand->pos)
                                    operand->pos -= 8;
                                int n = operand->relOfs;
                                if (n < 0)
                                    n = -n;
                                Fixup* f = new Fixup((AsmExprNode*)operand->node, (operand->size + 7) / 8, operand->relOfs != 0, n,
                                                     operand->relOfs > 0);
                                f->SetInsOffs((operand->pos + 7) / 8);
                                s->Add(f);
                            }
                        }
                    }
                }
                break;
                case AERR_SYNTAX:
                    throw new std::runtime_error("Syntax error while parsing instruction");
                case AERR_OPERAND:
                    throw new std::runtime_error("Unknown operand");
                case AERR_BADCOMBINATIONOFOPERANDS:
                    throw new std::runtime_error("Bad combination of operands");
                case AERR_UNKNOWNOPCODE:
                    throw new std::runtime_error("Unrecognized opcode");
                case AERR_INVALIDINSTRUCTIONUSE:
                    throw new std::runtime_error("Invalid use of instruction");
                default:
                    throw new std::runtime_error("unknown error");
            }
            return s;
        }
    }
    throw new std::runtime_error("Unrecognized opcode");
}
bool InstructionParser::Tokenize(std::string& line)
{
    while (line.size())
        NextToken(line);
    return true;
}
void InstructionParser::NextToken(std::string& line)
{
    bool negate = false;
    int npos = line.find_first_not_of(" \t\r\n\v");
    if (npos == std::string::npos)
        line = "";
    else if (npos != 0)
        line = line.substr(npos);
    if (line.size())
    {
        int n = 0;
        if (isalpha(line[0]) || line[0] == '_')
        {
            while (n < line.size() && (isalnum(line[n]) || line[n] == '_')) n++;
            std::string id = line.substr(0, n);
            line.erase(0, n);
            auto it = tokenTable.find(id);
            if (it != tokenTable.end())
            {
                 if (it->second >= 1000)
                 {
                     inputTokens.push_back(new InputToken);
                     auto next = inputTokens.back();
                     next->type = InputToken::REGISTER;
                     next->val = new AsmExprNode(it->second-1000);
                 }
                 else
                 {
                     inputTokens.push_back(new InputToken);
                     auto next = inputTokens.back();
                     next->type = InputToken::TOKEN;
                     next->val = new AsmExprNode(it->second);
                 }
            }
            else
            {
                 inputTokens.push_back(new InputToken);
                 auto next = inputTokens.back();
                 next->type = InputToken::NUMBER;
                 next->val = new AsmExprNode(id);
            }
        }
        else if (isdigit(line[0]))
        {
            inputTokens.push_back(new InputToken);
            while (n < line.size() && isdigit(line[n])) n++;
            std::string id = line.substr(0, n);
            line.erase(0, n);
             
            auto next = inputTokens.back();
            next->type = InputToken::NUMBER;
            next->val = new AsmExprNode(atoi(id.c_str()));
        }
        else if (line[0] == ';') // comment
        {
            line.erase(0, line.size());
        }
        else if (ispunct(line[0]))
        {
            std::string token;
            token += line[0];
            line.erase(0, 1);
            auto it = tokenTable.find(token);
            if (it != tokenTable.end())
            {
                inputTokens.push_back(new InputToken);
                auto next = inputTokens.back();
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(it->second);
            }
            else {
                throw new std::runtime_error(std::string("Unexpected token: ") + std::string(token));
            }
        }
        else
        {
                throw new std::runtime_error(std::string("Unexpected character: ") + line[0]);
        }
    }
}

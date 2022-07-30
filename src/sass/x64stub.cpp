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

#include "Instruction.h"
#include "InstructionParser.h"
#include "x64Operand.h"
#include "x64Parser.h"
#include "x64Instructions.h"
#include "Fixup.h"
#include <fstream>
#include <iostream>


InstructionParser* InstructionParser::GetInstance() { return static_cast<InstructionParser*>(new x64Parser()); }

int x64Parser::DoMath(char op, int left, int right)
{
    switch (op)
    {
        case '!':
            return -left;
        case '~':
            return ~left;
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '>':
            return left >> right;
        case '<':
            return left << right;
        case '&':
            return left & right;
        case '|':
            return left | right;
        case '^':
            return left ^ right;
        default:
            return left;
    }
}
void x64Parser::Setup(Section* sect)
{
    Setprocessorbits(processorMode);
}

bool x64Parser::ParseSection(AsmFile* fil, Section* sect)
{
    return false;
}
bool x64Parser::ParseDirective(AsmFile* fil, Section* sect)
{
   return false;
}
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

#ifndef InstructionParser_h
#define InstructionParser_h

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <stdlib.h>
#include <climits>
#include <memory>
#include <set>
#include "AdlStructures.h"
#include "AsmExpr.h"

class Instruction;
class Section;
class AsmFile;

namespace Optimizer
{
class SimpleExpression;
}
struct ocode;  // compiler support
struct expr;   // compiler support
struct amode;  // compiler support

class InstructionParser
{
  public:
    InstructionParser() : numeric(nullptr) { }
    static InstructionParser* GetInstance();

    bool MatchesOpcode(std::string opcode);
    std::unordered_map<std::string, int>::iterator GetOpcode(const std::string& opcode);

    Instruction* Parse(std::string& line);
    virtual bool IsBigEndian() = 0;
    virtual void Setup(Section* sect) = 0;
    bool SetNumber(int tokenPos, int oldVal, int newVal);
    static bool SetProcessorMode(int mode)
    {
        if (mode == 16 || mode == 32 || mode == 64)
        {
            processorMode = mode;
            return true;
        }
        return false;
    }

  protected:
    bool ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos);
    bool Tokenize(int& op);
    bool IsNumber();
    bool eol;
    enum
    {
        TOKEN_BASE = 0,
        REGISTER_BASE = 1000
    };
    virtual asmError DispatchOpcode(int opcode) = 0;
    void NextToken(std::string& line);
    enum
    {
        TK_ID = INT_MIN,
        TK_REG = INT_MIN + 1,
        TK_NUMERIC = INT_MIN + 2,
    };
    Numeric* numeric;
    std::unordered_map<std::string, int> tokenTable;
    std::unordered_map<std::string, int> opcodeTable;
    std::unordered_map<std::string, int> prefixTable;
    std::list<Numeric*> operands;
    std::list<Coding*> CleanupValues;
    std::list<int> prefixes;
    std::vector<InputToken*> inputTokens;
    BitStream bits;
    static int processorMode;

    // c compiler support

  public:
    std::string FormatInstruction(ocode* ins);
    asmError GetInstruction(ocode* ins, Instruction*& newIns, std::list<Numeric*>& operands);

  protected:
    bool Tokenize(std::string& line);
    void SetRegToken(int reg, int sz);
    void SetNumberToken(int val);
    bool SetNumberToken(struct Optimizer::SimpleExpression* offset, int& n);
    void SetExpressionToken(struct Optimizer::SimpleExpression* offset);
    void SetSize(int sz);
    void SetBracketSequence(bool open, int sz, int seg);
    void SetOperandTokens(amode* operand);
    void SetTokens(ocode* ins);
};
#endif

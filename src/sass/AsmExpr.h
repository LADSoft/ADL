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

#ifndef AsmExpr_h
#define AsmExpr_h

#include <string>
#include <memory>
#include "AdlStructures.h"

#include "Token.h"
class ppDefine;
class Section;

typedef long long PPINT;

class AsmExprNode : public AdlExprNode
{
  public:
    enum Type
    {
        IVAL,
        FVAL,
        LABEL,
        PC,
        REG
    };
    AsmExprNode(Type xType) :
        AdlExprNode(0), type(xType)
    {
    }
    AsmExprNode(PPINT Ival) :
        AdlExprNode(Ival), type(IVAL)
    {
    }
    AsmExprNode(PPINT regval, bool reg) :
        AdlExprNode(regval), type(REG) { }
    AsmExprNode(double& Fval) : AdlExprNode(0), type(FVAL), fval(Fval) {}
    AsmExprNode(std::string lbl) : AdlExprNode(0), type(LABEL), label(lbl) {}
    AsmExprNode(const AsmExprNode& old) : AdlExprNode(old)
    {
        fval = old.fval;
        label = old.label;
        type = old.type;
    }
    ~AsmExprNode() {}
    double fval;
    std::string label;
    Type GetType() { return type; }
    void SetType(Type tType) { type = tType; }

  protected:
    bool IsAbsoluteInternal(int& level);

  private:
    Type type;
    Section* sect;
};
#endif

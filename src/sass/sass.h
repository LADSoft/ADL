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
#ifndef SASS_H
#define SASS_H
#include "CmdSwitch.h"
#include "Instruction.h"
#include "InstructionParser.h"
#include <list>
#include <map>

class Sass
{
public:
      Sass()
      {
      }
      int Run(int argc, char **argv);
protected:
	void ParseFile(std::istream& input, InstructionParser& ip);
	void WriteFile(std::ostream& output);
      std::string GetLabel(std::string& line);
      Instruction* GetData(std::string &line, int selector);
      Instruction* GetInstruction(std::string & line, InstructionParser& ip);
      void ResolveFixups(void);
      void DisplayBytes(std::ostream& output, Instruction* ins);
private:
      std::list<Instruction*> instructions;
      std::map<std::string, Label*> labels;
      int lineNo = 1;
      static CmdSwitchParser SwitchParser;
      static CmdSwitchFile File;
      static CmdSwitchInt ProcessorMode;
	static const char* usageText;

};
#endif
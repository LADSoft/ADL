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
#include "Sass.h"
#include "Utils.h"
#include "fixup.h"
#include "x64Operand.h"
#include "x64Parser.h"
#include <iostream>
#include <iomanip>
#include "Utils.h"
#include <vector>

CmdSwitchParser Sass::SwitchParser;
CmdSwitchFile Sass::File(SwitchParser, '@');
CmdSwitchInt Sass::ProcessorMode(SwitchParser, 's', 32, 0, 100, {"processor-mode"});
const char* Sass::usageText =
    "[options] [stdin && stdout]"
    "\n"
    "  @filename                          Use response file\n"
    "  /s:xxx, --processor-mode           Set processor mode (16,32,64)\n"
    "\n"
    "Time: " __TIME__ "  Date: " __DATE__;


int main(int argc, char **argv)
{
     Sass main;
     return main.Run(argc, argv);
}

std::string Sass::GetLabel(std::string& line)
{
    int n = 0;
    while (n < line.size() && isspace(line[n])) n++;
    if (n < line.size())
    {
        if (isalpha(line[n]) || line[n] == '_')
        {
            int m = n;
            while (n < line.size() && (isalnum(line[n]) || line[n] == '_'))
                n++;
            int q  = n;
            while (q < line.size() && isspace(line[q])) q++;
            if (q >= line.size() || line[q] != ':')
                return std::string();
            std::string id = line.substr(m, n);
            line.erase(0, q+ 1);
            return id;            
        }
    }
    return std::string();
}
Instruction* Sass::GetData(std::string &line, int selector)
{
    std::vector<unsigned char> bytes;
    while (line.size())
    {;
        int n=0;
        while (n < line.size() && isspace(line[n])) n++;
        if (n < line.size())
        {
            if (isdigit(line[n]))
            {
                int m = n;
                while (n < line.size() && isdigit(line[n])) n++;
                unsigned val = atoi(line.substr(m, n).c_str());
                line.erase(0, n);
                switch (selector)
                {
                    case 'b':
                        bytes.push_back(val);
                        break;
                    case 'd':
                        bytes.push_back(val);
                        bytes.push_back( val >> 8); 
                        bytes.push_back( val >> 16); 
                        bytes.push_back( val >> 24); 
                        break;
                    case 'w':
                        bytes.push_back(val);
                        bytes.push_back( val >> 8); 
                        break;
                }
                if (line.size())
                {
                    int n = line.find_first_of(',');
                    if (n != std::string::npos)
                        line = line.substr(n+1);
                }
            }
            else
            {
                throw new std::runtime_error("invalid number in data entry");
            }
        }
    }
    return new Instruction(&*bytes.begin(), bytes.size(), true);
}
Instruction* Sass::GetInstruction(std::string & line, InstructionParser& ip)
{
    Instruction* rv = nullptr;
    int n = 0;
    while (n < line.size() && isspace(line[n])) n++;
    if (n < (line.size() - 2) && line[n] == 'd')
    {
        switch(line[n+1])
        {
            case 'b':
            case 'w':
            case 'd':
                {
                    char x = line[n+1];
                    line.erase(0, n+2);
                    rv = GetData(line, x);
                }
                break;
            default:
                line.erase(0, n);
                rv = ip.Parse(line);
                break;
        } 
    }
    else
    {
        line.erase(0, n);
        rv = ip.Parse(line);
    }
    return rv;
}
void Sass::ParseFile(std::istream& input, InstructionParser& ip)
{

    while (!input.eof())
    {
        char buf[1000];
        buf[0] = 0;
        input.getline(buf, 1000);
        if (buf[0])
        {
            std::string line(buf);
            auto lbl = GetLabel(line);
            if (lbl.size())
            {
                auto v = new Label(lbl,0,0);
                if (labels.find(lbl) != labels.end())
                     throw new std::runtime_error(std::string("Duplicate label ") + lbl);
                labels[lbl] = v;
                auto ins = new Instruction(v);
                ins->SetText(std::string(buf));
                instructions.push_back(ins);
            }
            auto ins = GetInstruction(line, ip);
            ins->SetText(std::string(buf));
            instructions.push_back(ins);
            lineNo++;
        }
    }
}
void Sass::ResolveFixups()
{
    int pos = 0;
    for (auto i : instructions)
    {
        i->SetOffset(pos);
        if (i->GetType() == Instruction::LABEL)
        {
            i->GetLabel()->SetOffset(pos);
        }
        pos += i->GetSize();
    }
    for (auto i : instructions)
    {
        for (auto&& f : *i->GetFixups())
        {
            std::string lbl = f->GetExpr()->label;
            auto lblinst = labels[lbl];
            if (lblinst == nullptr)
                throw new std::runtime_error(std::string("unknown label ") + lbl);
            unsigned addr = lblinst->GetOffset()->ival;
            if (f->IsRel())
            {
                bool toErr = false;
                // this next is processor dependent
                addr -= i->GetOffset() + i->GetSize();
                switch(f->GetSize())
                {
                    case 1:
                        toErr = (int)addr < -128 || (int)addr > 127;
                        break;
                    case 2:
                        toErr = (int)addr < -32768 || (int)addr > 32767;
                        break;
                }
                if (toErr)
                {
                    throw new std::runtime_error(std::string("relative branch out of range"));
                }
            }
            unsigned char *b = i->GetBytes();
            int n = f->GetInsOffs();
            switch(f->GetSize())
            {
                case 1:
                    b[n] = addr;
                    break;
                case 2:
                    b[n] = addr;
                    b[n+1] = addr >> 8;
                    break;
                case 4:
                    b[n] = addr;
                    b[n+1] = addr >> 8;
                    b[n+2] = addr >> 16;
                    b[n+3] = addr >> 24;
                    break;
            }
        }
    }
}
void Sass::DisplayBytes(std::ostream& output, Instruction* ins)
{
    if (ins->GetSize() == 0)
    {
        output << "               " << ins->GetText() << std::endl;
    }
    for (int j = 0; j < ins->GetSize(); j+= 5)
    {
        int k;
        for (k = j; k < j + 5; k++)
        {
            if (k < ins->GetSize())
            {
                output << std::setfill('0') << std::setw(2) << std::setbase(16);
                output << (int)ins->GetBytes()[k] << " ";
            }
            else
            {
                output << "   ";
            }
        }
        if (j == 0)
            output << ins->GetText();
        output << std::endl;
        if (k < ins->GetSize())
        {
            output << "          ";
        }
    }

}
void Sass::WriteFile(std::ostream& output)
{
    for (auto i : instructions)
    {
        switch (i->GetType())
        {
             case Instruction::CODE:
             case Instruction::DATA:
                 output << std::setbase(16)<< std::setfill('0') << std::setw(8) << i->GetOffset() << std::setfill(' ') << std::setw(0) << ": " ;
                 DisplayBytes(output, i);
                 output << std::setfill(' ') << std::setw(0) << std::setbase(10);
                 break;
             case Instruction::LABEL:
                 break;
             default:
                  throw new std::runtime_error("Internal error: unsupported instruction type");
        }
    }
}
int Sass::Run(int argc, char **argv)
{
    int rv = 0;
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    if (!SwitchParser.Parse(&argc, argv) ||  argc != 1)
    {
        Utils::usage(argv[0], usageText);
    }
    auto ip = InstructionParser::GetInstance();
    if (!InstructionParser::SetProcessorMode(ProcessorMode.GetValue()))
        Utils::fatal("Invalid processor mode");
    ip->Setup(nullptr);
    try
    {
        ParseFile(std::cin, *ip);
        ResolveFixups();
        WriteFile(std::cout);
    }
    catch (std::exception *e)
    {
        std::cout << "Error(" << lineNo << "): " << e->what() << std::endl;
        delete e;
    }   

    return rv;
}

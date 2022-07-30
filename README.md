ADL
=======

ADL Architecture design language compiler

Project Build Status:
[![Build status](https://ci.appveyor.com/api/projects/status/mg9ystqer6n6uqng?svg=true)](https://ci.appveyor.com/project/LADSoft/orc)

ADL is a design language that can be used to specify the bit pattern associated with generating object code from assembly language source.   In the future it may also be used to generate backends for the orange c compiler.   It is part of the [Orange C](https://www.github.com/LADSoft/OrangeC) toolchain.   

An accompanying program SASS shows how to use the generated code as the core for a simple assembler

Source Code for ADL is released under the GNU General Public License version 3.

The [Appveyor CI](https://ci.appveyor.com/project/LADSoft/ADL) project for this repository builds a setup file after each checkin.   It uses `omake` to do this.   The project can be built either by running omake, or by using the VS2019 solution which is included.


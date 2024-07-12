A simple implementation of the game "Snake" in pure C and WinApi

Written for the purpose of learning the language and API

May be useful for beginners for educational purposes

Customizable game level sizes and window scaling

Increasing speed, two types of apples

Can be compiled in the MSYS2 environment using GCC or Clang, requires mingw-w64-libgdiplus installed.

![alt text](https://github.com/ist76/lonelysnake/blob/main/Screenshot.PNG?raw=true)

# Installation

To simply launch the application, just download the single snake.exe file from the “bin” folder. No additional libraries are required, the program should run on any fairly modern 64-bit version of Windows (including, probably, Win7).
If you have the mingw-w64 environment and mingw-w64-x86_64-toolchain installed, assembly is not difficult: just run 
'''make'''
in a local copy of the repository or build the project manually something like this:
```gcc main.c logic.c winproc.c -Wall -lgdi32 -march=corei7 -Os -s -static -municode -mwindows -o snake.exe```

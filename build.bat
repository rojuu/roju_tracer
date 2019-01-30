@echo off
SETLOCAL

set SDL_INC=%SDL2%\include
set SDL_LIB=%SDL2%\lib\x64

set CommonCompilerFlags=/Zi /Od /EHsc /nologo /FC /I%SDL_INC% /std:c++14
set CommonLinkerFlags=/DEBUG /LIBPATH:%SDL_LIB% SDL2.lib SDL2main.lib

if not exist bin (
    mkdir bin
)
pushd bin
if not exist SDL2.dll (
    robocopy %SDL_LIB% . *.dll
)
cl %CommonCompilerFlags% ..\src\*.cpp /link /subsystem:console %CommonLinkerFlags% /out:roju_tracer.exe
popd
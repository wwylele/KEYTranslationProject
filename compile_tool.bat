@echo off
WHERE >nul 2>nul gcc && GOTO usegcc || GOTO usecl

:usegcc
ECHO compiling using gcc
cd ./brfnt/brfnt_mod/
g++ -O2 -std=c++11 main.cpp Brfnt.cpp -lgdi32 -o brfnt_mod
cd ../../
cd ./gfa/
gcc -O2 bpe.c -o bpe
cd ../
GOTO end

:usecl
WHERE >nul 2>nul cl || GOTO failed
ECHO compiling using msvc
cd ./brfnt/brfnt_mod/
cl -nologo -O2 main.cpp Brfnt.cpp /Febrfnt_mod /EHsc /link gdi32.lib user32.lib
cd ../../
cd ./gfa/
cl -nologo -O2 bpe.c /Febpe
cd ../
GOTO end

:failed
ECHO neither gcc nor msvc can be found. Please install one of them and set up in environment variable.
:end
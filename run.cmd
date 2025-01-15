del main.exe
g++ -std=c++17 -pthread -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include ^
-IC:\msys64\mingw32\include\wx-3.2 ^
-IC:\msys64\mingw32\lib\wx\include\msw-unicode-3.2 ^
-o main.exe ^
src\structures.cpp ^
src\print.cpp ^
src\costs.cpp ^
src\path_finding.cpp ^
src\heuristics.cpp ^
src\experiments.cpp ^
src\main.cpp ^
src\rectshape.cpp ^
-lsfml-graphics ^
-lsfml-window ^
-lsfml-system ^
-lsfml-network ^
-ljsoncpp ^
-lopengl32
@REM g++ -std=c++17 -Wall -Wextra -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include -o main.exe ^
@REM src\structures.cpp ^
@REM src\print.cpp ^
@REM src\costs.cpp ^
@REM src\main.cpp ^
@REM src\path_finding.cpp ^
@REM src\heuristics.cpp
.\main.exe
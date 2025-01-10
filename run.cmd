del main.exe
@REM g++ -std=c++17 -pthread -Wall -Wextra -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include -o main.exe ^
@REM src\structures.cpp ^
@REM src\print.cpp ^
@REM src\costs.cpp ^
@REM src\main.cpp ^
@REM src\rectshape.cpp ^
@REM src\path_finding.cpp ^
@REM src\heuristics.cpp ^
@REM -lsfml-graphics ^
@REM -lsfml-window ^
@REM -lsfml-system ^
@REM -lsfml-network
g++ -std=c++17 -Wall -Wextra -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include -o main.exe ^
src\structures.cpp ^
src\print.cpp ^
src\costs.cpp ^
src\main.cpp ^
src\path_finding.cpp ^
src\heuristics.cpp
.\main.exe
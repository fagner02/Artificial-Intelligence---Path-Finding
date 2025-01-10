del main.exe
g++ -std=c++17 -mwindows -pthread -Wall -Wextra -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include -o main.exe ^
src\structures.cpp src\print.cpp src\costs.cpp src\main.cpp src\rectshape.cpp src\path_finding.cpp ^
-lsfml-graphics ^
-lsfml-window ^
-lsfml-system ^
-lsfml-network
.\main.exe
del main.exe
g++ -std=c++17 -mwindows -static -static-libgcc -static-libstdc++ -pthread -Wl,-Bstatic -DSFML_STATIC -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include ^
src\main.cpp ^
src\structures.cpp ^
src\print.cpp ^
src\costs.cpp ^
src\path_finding.cpp ^
src\heuristics.cpp ^
src\experiments.cpp ^
src\rectshape.cpp ^
src\point.cpp ^
src\goal_test.cpp ^
src\node.cpp ^
-lsfml-graphics-s ^
-lsfml-window-s ^
-lsfml-system-s ^
-lwinmm ^
-lopengl32 ^
-lgdi32 ^
-lfreetype ^
-lpng16 ^
-lz ^
-ljpeg ^
-lbz2 ^
-lpthread ^
-lharfbuzz ^
-lgraphite2 ^
-lstdc++fs ^
-lbrotlidec ^
-lbrotlienc ^
-lbrotlicommon ^
-lusp10 ^
-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 ^
-lrpcdce4 -lrpcrt4 ^
-o main.exe
@REM g++ -std=c++17 -Wall -Wextra -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include -o main.exe ^
@REM src\structures.cpp ^
@REM src\print.cpp ^
@REM src\costs.cpp ^
@REM src\main.cpp ^
@REM src\path_finding.cpp ^
@REM src\heuristics.cpp
.\main.exe
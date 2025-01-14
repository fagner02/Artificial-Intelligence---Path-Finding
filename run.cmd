del main.exe
g++ -std=c++17 -pthread -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include ^
-IC:\msys64\mingw32\include\wx-3.2 ^
-IC:\msys64\mingw32\lib\wx\include\msw-unicode-3.2 ^
-o main.exe ^
src\structures.cpp ^
src\print.cpp ^
src\costs.cpp ^
src\main.cpp ^
src\rectshape.cpp ^
src\path_finding.cpp ^
src\heuristics.cpp ^
-lsfml-graphics ^
-lsfml-window ^
-lsfml-system ^
-lsfml-network ^
-lwx_baseu-3.2 ^
-lwx_baseu_xml-3.2 ^
-lwx_baseu_net-3.2 ^
-lwx_mswu_core-3.2 ^
-lwx_mswu_adv-3.2 ^
-lwx_mswu_html-3.2 ^
-lwx_mswu_qa-3.2 ^
-lwx_mswu_xrc-3.2 ^
-lwx_mswu_aui-3.2 ^
-lwx_mswu_propgrid-3.2 ^
-lwx_mswu_ribbon-3.2 ^
-lwx_mswu_stc-3.2 ^
-lwx_mswu_gl-3.2 ^
-lwx_mswu_media-3.2 ^
-lwx_mswu_webview-3.2 ^
-lwx_mswu_richtext-3.2
@REM g++ -std=c++17 -Wall -Wextra -LC:\msys64\mingw32\lib -I.\include -IC:\msys64\mingw32\include -o main.exe ^
@REM src\structures.cpp ^
@REM src\print.cpp ^
@REM src\costs.cpp ^
@REM src\main.cpp ^
@REM src\path_finding.cpp ^
@REM src\heuristics.cpp
.\main.exe
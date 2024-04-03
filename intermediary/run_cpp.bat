cd cpp
g++ -o main main.cpp variables.cpp -std=c++17 -O2
cd ..
.\SemiFinalJudge.exe -m maps\map1.txt -l ERR -s 12 .\cpp\main.exe
.\SemiFinalJudge.exe -m maps\map2.txt -l ERR -s 12 .\cpp\main.exe
.\SemiFinalJudge.exe -m maps\map3.txt -l ERR -s 12 .\cpp\main.exe
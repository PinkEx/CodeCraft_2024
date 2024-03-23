cd cpp
g++ main.cpp -o main.exe -std=c++17 -O3
cd ..
PreliminaryJudge.exe -m maps\map11.txt -s 10 -l ERR "cpp\main.exe"
PreliminaryJudge.exe -m maps\map12.txt -s 10 -l ERR "cpp\main.exe"
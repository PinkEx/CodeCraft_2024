cd cpp
g++ -o main main.cpp variables.cpp -std=c++17 -O2
cd ..
del logs\log1.txt
.\SemiFinalJudge.exe -m maps\map1.txt -l ERR -s 233 .\cpp\main.exe >> logs/log1.txt 2>&1
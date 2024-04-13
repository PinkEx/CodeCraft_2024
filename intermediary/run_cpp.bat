cd cpp
cd old
g++ -o main main.cpp variables.cpp -std=c++17 -O2
cd ..
cd ..
.\SemiFinalJudge.exe -m maps\map1.txt -l ERR .\cpp\old\main.exe
.\SemiFinalJudge.exe -m maps\map2.txt -l ERR .\cpp\old\main.exe
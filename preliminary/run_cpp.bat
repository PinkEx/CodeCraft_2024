@echo off
cd test
g++ main-11.cpp -o main-11.exe -std=c++17 -O3
cd ..
for /L %%k in (11, 1, 12) do (
    type nul > logs\log_%%k.txt
    for /L %%i in (1, 1, 50) do (
        echo %%i >> logs/log_%%k.txt
        PreliminaryJudge.exe -m maps\map%%k.txt -s %%i -l ERR "test\main-11.exe" >> logs/log_%%k.txt 2>&1
    )
    findstr /v "Err" logs\log_%%k.txt > logs\temp_log.txt
    del logs\log_%%k.txt
    ren logs\temp_log.txt log_%%k.txt
)
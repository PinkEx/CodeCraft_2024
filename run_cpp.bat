@echo off

for /L %%k in (0, 1, 8) do (
    type nul > logs\log_%%k.txt
    for /L %%i in (1, 1, 50) do (
        echo %%i >> logs/log_%%k.txt
        PreliminaryJudge.exe -m maps\map%%k.txt -s %%i -l ERR "cpp\main.exe" >> logs/log_%%k.txt 2>&1
    )
    findstr /v "Err" logs\log_%%k.txt > logs\temp_log.txt
    del logs\log_%%k.txt
    ren logs\temp_log.txt log_%%k.txt
)
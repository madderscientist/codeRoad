@echo off
setlocal
rem 删除本目录（脚本所在目录）下所有 .exe 文件（带确认提示）

pushd "%~dp0"

rem 统计 .exe 文件数量
for /f %%A in ('dir /b *.exe 2^>nul ^| find /c /v ""') do set COUNT=%%A
if "%COUNT%"=="" set COUNT=0

if %COUNT% EQU 0 (
    echo 本目录没有 .exe 文件。
    popd
    endlocal
    exit /b 0
)

echo 将删除目录 "%CD%" 下的 %COUNT% 个 .exe 文件（操作不可撤销）。
choice /m "确认删除吗"
if errorlevel 2 (
    echo 操作已取消。
    popd
    endlocal
    exit /b 0
)

del /f /q "%CD%\*.exe"
echo 删除完成。
popd
endlocal
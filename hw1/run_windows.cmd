@echo off
setlocal
chcp 65001 >nul
cd /d "%~dp0.."

cmake -S hw1 -B build-debug -DHW1_BUILD_TESTS=ON -DHW1_BUILD_BENCH=OFF -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 exit /b 1
cmake --build build-debug --config Debug --target hw1_demo hw1_tests
if errorlevel 1 exit /b 1
echo Original demonstration:
if exist build-debug\Debug\hw1_demo.exe (
    build-debug\Debug\hw1_demo.exe
) else (
    build-debug\hw1_demo.exe
)
if errorlevel 1 exit /b 1

ctest --test-dir build-debug -C Debug -V --output-on-failure
if errorlevel 1 exit /b 1

cmake -S hw1 -B build-release -DHW1_BUILD_TESTS=OFF -DHW1_BUILD_BENCH=ON -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build build-release --config Release --target hw1_bench
if errorlevel 1 exit /b 1
if exist build-release\Release\hw1_bench.exe (
    build-release\Release\hw1_bench.exe hw1\bench\results.csv
) else (
    build-release\hw1_bench.exe hw1\bench\results.csv
)
if errorlevel 1 exit /b 1
echo Both runs completed successfully.

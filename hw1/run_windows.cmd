@echo off
setlocal
cd /d "%~dp0.."

cmake -S hw1 -B build-debug -DHW1_BUILD_TESTS=ON -DHW1_BUILD_BENCH=OFF -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 exit /b 1
cmake --build build-debug --config Debug --target hw1_tests
if errorlevel 1 exit /b 1
ctest --test-dir build-debug -C Debug -V --output-on-failure
if errorlevel 1 exit /b 1

cmake -S hw1 -B build-release -DHW1_BUILD_TESTS=OFF -DHW1_BUILD_BENCH=ON -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build build-release --config Release --target hw1_bench
if errorlevel 1 exit /b 1
if exist build-release\Release\hw1_bench.exe (
    build-release\Release\hw1_bench.exe hw1\results\bench_windows.csv
) else (
    build-release\hw1_bench.exe hw1\results\bench_windows.csv
)
if errorlevel 1 exit /b 1
echo Both runs completed successfully.

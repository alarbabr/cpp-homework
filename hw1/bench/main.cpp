#include "../main.cpp"

#include <chrono>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#ifndef NDEBUG
#error "Benchmarks must be built in Release."
#endif
#ifdef HW1_ASAN_ENABLED
#error "Benchmarks must be built without sanitizers."
#endif

constexpr int repetitions = 5;
volatile double benchmarkSink = 0.0;

double observedValue(int value) {
    return static_cast<double>(value);
}

double observedValue(const Point& point) {
    return point.x + point.y;
}

template <typename T, typename Compare>
double measure(const std::vector<T>& input, Compare comp, bool useMySort) {
    // Подготовка копии не входит в измеряемое время.
    std::vector<T> data = input;
    auto start = std::chrono::steady_clock::now();
    if (useMySort) {
        mySort(data.begin(), data.end(), comp);
    } else {
        std::sort(data.begin(), data.end(), comp);
    }
    auto finish = std::chrono::steady_clock::now();

    // Проверка и наблюдение результата также вне измеряемого участка.
    if (!std::is_sorted(data.begin(), data.end(), comp)) {
        throw std::runtime_error("Benchmark produced an unsorted result");
    }
    if (!data.empty()) {
        benchmarkSink = benchmarkSink + observedValue(data.front()) + observedValue(data.back());
    }
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

double median(std::vector<double> times) {
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

struct Result {
    std::string type;
    std::string distribution;
    std::size_t n;
    double myMs;
    double stdMs;
};

template <typename T, typename Compare>
Result benchmark(const std::string& type, const std::string& distribution,
                 const std::vector<T>& input, Compare comp) {
    // По одному прогревочному запуску каждого алгоритма.
    measure(input, comp, true);
    measure(input, comp, false);

    std::vector<double> myTimes;
    std::vector<double> stdTimes;
    for (int trial = 0; trial < repetitions; trial++) {
        // Чередуем порядок, чтобы один алгоритм не запускался всегда первым.
        if (trial % 2 == 0) {
            myTimes.push_back(measure(input, comp, true));
            stdTimes.push_back(measure(input, comp, false));
        } else {
            stdTimes.push_back(measure(input, comp, false));
            myTimes.push_back(measure(input, comp, true));
        }
    }
    return {type, distribution, input.size(), median(myTimes), median(stdTimes)};
}

void printRow(const Result& result) {
    std::cout << std::left << std::setw(8) << result.type
              << std::setw(10) << result.distribution
              << std::right << std::setw(10) << result.n
              << std::setw(15) << result.myMs
              << std::setw(15) << result.stdMs
              << std::setw(13) << result.myMs / result.stdMs << '\n';
}

int main(int argc, char* argv[]) {
    try {
        if (argc > 2) {
            throw std::runtime_error("Usage: hw1_bench [output.csv]");
        }
        std::cout << "Release benchmark, median of " << repetitions << " trials. Times in ms.\n";
#ifdef _MSC_VER
        std::cout << "Compiler: MSVC " << _MSC_VER << '\n';
#else
        std::cout << "Compiler: " << __VERSION__ << '\n';
#endif
        std::cout << "Random seed: 20261010. Copies, checks and output are outside timing.\n";
        std::cout << "Type    Input              N      mySort_ms    std_sort_ms       my/std\n";
        std::cout << std::fixed << std::setprecision(6);

        std::mt19937 generator(20261010U);
        std::uniform_int_distribution<int> intDistribution(-1000000, 1000000);
        std::uniform_real_distribution<double> pointDistribution(-1000.0, 1000.0);
        std::vector<Result> results;
        for (std::size_t n : {1000U, 10000U, 100000U, 1000000U}) {
            std::vector<int> ints(n);
            std::vector<Point> points(n);
            for (int& value : ints) {
                value = intDistribution(generator);
            }
            for (Point& point : points) {
                point = {pointDistribution(generator), pointDistribution(generator)};
            }
            results.push_back(benchmark("int", "random", ints, std::less<int>{}));
            printRow(results.back());
            results.push_back(benchmark("Point", "random", points, byDistance));
            printRow(results.back());

            if (n == 100000) {
                std::sort(ints.begin(), ints.end());
                std::sort(points.begin(), points.end(), byDistance);
                results.push_back(benchmark("int", "sorted", ints, std::less<int>{}));
                printRow(results.back());
                results.push_back(benchmark("Point", "sorted", points, byDistance));
                printRow(results.back());
            }
        }

        for (const std::string& type : {std::string("int"), std::string("Point")}) {
            double randomMs = 0.0;
            double sortedMs = 0.0;
            for (const Result& result : results) {
                if (result.type == type && result.n == 100000) {
                    if (result.distribution == "random") {
                        randomMs = result.myMs;
                    } else {
                        sortedMs = result.myMs;
                    }
                }
            }
            std::cout << type << ": mySort sorted/random at n=100000 = " << sortedMs / randomMs << '\n';
        }
        std::cout << "Result observation checksum: " << benchmarkSink << '\n';

        if (argc == 2) {
            std::ofstream csv(argv[1]);
            if (!csv) {
                throw std::runtime_error("Cannot open CSV output file");
            }
            csv << "type,distribution,n,repetitions,mySort_ms,std_sort_ms,my_over_std\n";
            csv << std::fixed << std::setprecision(6);
            for (const Result& result : results) {
                csv << result.type << ',' << result.distribution << ',' << result.n << ','
                    << repetitions << ',' << result.myMs << ',' << result.stdMs << ','
                    << result.myMs / result.stdMs << '\n';
            }
            csv.flush();
            if (!csv) {
                throw std::runtime_error("Cannot write CSV output file");
            }
            std::cout << "Saved CSV: " << argv[1] << '\n';
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

#include "../main.cpp"

#include <climits>
#include <exception>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>

#ifdef NDEBUG
#error "Correctness tests must be built in Debug."
#endif
#ifndef HW1_ASAN_ENABLED
#error "Correctness tests must be built with AddressSanitizer."
#endif

int passed = 0;

void printValue(int value) {
    std::cout << value;
}

void printValue(const Point& point) {
    const auto previousPrecision = std::cout.precision(std::numeric_limits<double>::max_digits10);
    std::cout << '(' << point.x << ", " << point.y << ')';
    std::cout.precision(previousPrecision);
}

template <typename T>
void printInput(const std::string& name, const std::vector<T>& input) {
    std::cout << "[ВХОД] " << name << " (n=" << input.size() << "): [";
    for (std::size_t i = 0; i < input.size(); i++) {
        if (i != 0) {
            std::cout << ", ";
        }
        // Маленькие входы показываем целиком, большие — первые и последние пять элементов.
        if (input.size() > 20 && i == 5) {
            std::cout << "... (пропущено " << input.size() - 10 << " элементов) ..., ";
            i = input.size() - 5;
        }
        printValue(input[i]);
    }
    std::cout << ']' << std::endl;
}

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool sameElements(std::vector<int> a, std::vector<int> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

bool byCoordinates(const Point& a, const Point& b) {
    if (a.x != b.x) {
        return a.x < b.x;
    }
    return a.y < b.y;
}

bool samePoint(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

bool sameElements(std::vector<Point> a, std::vector<Point> b) {
    // Порядок равноудалённых точек не обязан совпадать.
    // Для проверки сохранности упорядочиваем копии по координатам.
    std::sort(a.begin(), a.end(), byCoordinates);
    std::sort(b.begin(), b.end(), byCoordinates);
    return a.size() == b.size()
        && std::equal(a.begin(), a.end(), b.begin(), samePoint);
}

template <typename T, typename Compare>
void runCase(const std::string& name, const std::vector<T>& input, Compare comp) {
    printInput(name, input);
    std::vector<T> actual = input;
    mySort(actual.begin(), actual.end(), comp);
    require(std::is_sorted(actual.begin(), actual.end(), comp), name + ": wrong order");
    require(sameElements(input, actual), name + ": elements changed or lost");
    passed++;
    std::cout << "[PASS] " << name << " (n=" << input.size() << ")\n";
}

void testInts() {
    std::less<int> comp;
    runCase("int/empty", std::vector<int>{}, comp);
    runCase("int/single", std::vector<int>{42}, comp);
    runCase("int/sorted", std::vector<int>{-5, -1, 0, 2, 8}, comp);
    runCase("int/reverse", std::vector<int>{8, 2, 0, -1, -5}, comp);
    runCase("int/equal", std::vector<int>(20, 6), comp);
    runCase("int/negative", std::vector<int>{-7, 0, -2, 5, -10}, comp);
    runCase("int/duplicates", std::vector<int>{3, 1, 3, 2, 1, 3}, comp);
    runCase("int/extremes", std::vector<int>{INT_MAX, 0, INT_MIN}, comp);
    runCase("int/descending", std::vector<int>{7, 2, 5, 1, 9}, std::greater<int>{});

    for (unsigned int seed : {12345U, 20261010U}) {
        std::mt19937 generator(seed);
        std::uniform_int_distribution<int> distribution(-1000, 1000);
        for (std::size_t n : {10U, 1000U, 100000U}) {
            std::vector<int> data(n);
            for (int& value : data) {
                value = distribution(generator);
            }
            runCase("int/random/seed=" + std::to_string(seed), data, comp);
        }
    }

    std::vector<int> data = {7, 2, 5, 1, 9};
    std::vector<int> original = data;
    printInput("int/default-comparator", data);
    mySort(data.begin(), data.end());
    require(std::is_sorted(data.begin(), data.end()), "default comparator: order");
    require(sameElements(original, data), "default comparator: permutation");
    passed++;
    std::cout << "[PASS] int/default-comparator\n";

    int array[] = {7, 2, 5, 1, 9};
    printInput("int/raw-array", std::vector<int>(std::begin(array), std::end(array)));
    mySort(std::begin(array), std::end(array));
    require(std::is_sorted(std::begin(array), std::end(array)), "int[]: order");
    require(sameElements(std::vector<int>{7, 2, 5, 1, 9},
                         std::vector<int>(std::begin(array), std::end(array))),
            "int[]: permutation");
    passed++;
    std::cout << "[PASS] int/raw-array\n";

    std::vector<int> subrange = {100, 7, 2, 5, 1, 9, -100};
    printInput("int/subrange (сортируются индексы 1..5)", subrange);
    mySort(subrange.begin() + 1, subrange.end() - 1);
    require((subrange == std::vector<int>{100, 1, 2, 5, 7, 9, -100}), "subrange");
    passed++;
    std::cout << "[PASS] int/subrange/outside-elements-unchanged\n";
}

void testPoints() {
    runCase("Point/empty", std::vector<Point>{}, byDistance);
    runCase("Point/single", std::vector<Point>{{3, 4}}, byDistance);
    runCase("Point/sorted", std::vector<Point>{{0, 0}, {1, 0}, {1, 1}, {3, 4}}, byDistance);
    runCase("Point/reverse", std::vector<Point>{{3, 4}, {1, 1}, {1, 0}, {0, 0}}, byDistance);
    runCase("Point/equal", std::vector<Point>(20, Point{3, 4}), byDistance);
    runCase("Point/duplicates", std::vector<Point>{{3, 4}, {1, 2}, {3, 4}, {0, 0}, {1, 2}}, byDistance);
    runCase("Point/equal-distance", std::vector<Point>{{3, 4}, {0, 5}, {-3, -4}, {-5, 0}}, byDistance);
    runCase("Point/negative-coordinates", std::vector<Point>{{-7, -1}, {0, 0}, {-1, 2}, {3, -4}}, byDistance);

    for (unsigned int seed : {12345U, 20261010U}) {
        std::mt19937 generator(seed);
        std::uniform_real_distribution<double> distribution(-1000.0, 1000.0);
        for (std::size_t n : {10U, 1000U, 100000U}) {
            std::vector<Point> data(n);
            for (Point& point : data) {
                point = {distribution(generator), distribution(generator)};
            }
            runCase("Point/random/seed=" + std::to_string(seed), data, byDistance);
        }
    }
}

int main() {
    try {
        testInts();
        testPoints();
        std::cout << "All " << passed << " correctness tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] " << error.what() << '\n';
        return 1;
    }
}

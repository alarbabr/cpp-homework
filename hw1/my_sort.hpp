#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <vector>

// Слияние двух частей, уже отсортированных с тем же компаратором.
template <typename T, typename Compare>
std::vector<T> mergeParts(const std::vector<T>& left,
                          const std::vector<T>& right, Compare comp) {
    std::vector<T> result;
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < left.size() && j < right.size()) {
        if (comp(left[i], right[j])) {
            result.push_back(left[i]);
            i++;
        } else {
            result.push_back(right[j]);
            j++;
        }
    }
    while (i < left.size()) {
        result.push_back(left[i]);
        i++;
    }
    while (j < right.size()) {
        result.push_back(right[j]);
        j++;
    }
    return result;
}

template <typename T, typename Compare>
std::vector<T> mergeSort(const std::vector<T>& data, Compare comp) {
    if (data.empty()) {
        return {};
    }
    if (data.size() == 1) {
        return {data[0]};
    }
    auto middle = data.size() / 2;
    std::vector<T> left(data.begin(), data.begin() + middle);
    std::vector<T> right(data.begin() + middle, data.end());
    left = mergeSort(left, comp);
    right = mergeSort(right, comp);
    return mergeParts(left, right, comp);
}

template <typename It,
          typename Compare = std::less<typename std::iterator_traits<It>::value_type>>
void mySort(It first, It last, Compare comp = Compare{}) {
    using T = typename std::iterator_traits<It>::value_type;
    std::vector<T> values(first, last);
    values = mergeSort(values, comp);
    std::copy(values.begin(), values.end(), first);
}

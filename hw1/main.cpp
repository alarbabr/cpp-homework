#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
struct Point {
	double x, y;
};
bool byDistance(const Point& a, const Point& b) {
	double d1 = a.x * a.x + a.y * a.y;
	double d2 = b.x * b.x + b.y * b.y;
	return d1 < d2;
}
bool comesBefore(int a, int b) {
	return a < b;
}
template <typename T,typename Compare>  
std::vector<T> mergeParts(
	const std::vector<T>& left,
	const std::vector<T>& right, Compare comp) { 
	std::vector<T> result;
	int i = 0;
	int j = 0;
	while (i < left.size() and j < right.size()) {
		if (comp(left[i], right[j])) {
			result.push_back(left[i]);
			i++;
		}
		else {
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
std::vector<T> mergeSort(const std::vector<T>& data,Compare comp) {
	if (data.size() <= 1) {
		return data;
	}
	auto middle = data.size() / 2;
	std::vector<T> left(data.begin(), data.begin() + middle);
	std::vector<T> right(data.begin() + middle, data.end());
	left = mergeSort(left, comp);
	right = mergeSort(right,comp);
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
#ifndef HW1_LIBRARY_ONLY
int main() {	
	std::vector<int> data = { 7, 2, 5, 1, 9	 };
	mySort(data.begin(), data.end(), comesBefore);	
	Point p = { 3.0, 4.0 };
	Point q = { 1.0, 2.0 };
	std::vector<Point> points = { p, q };
	mySort(points.begin(),points.end(), byDistance);
	for (int k : data) {
		std::cout << k << ' ';
	}
	std::cout << '\n';
	for (const Point& point : points) {
		std::cout << point.x << " " << point.y << '\n';

	}
}
#endif

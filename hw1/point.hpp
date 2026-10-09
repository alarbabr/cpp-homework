#pragma once

struct Point {
    double x, y;
};

inline bool byDistance(const Point& a, const Point& b) {
    double d1 = a.x * a.x + a.y * a.y;
    double d2 = b.x * b.x + b.y * b.y;
    return d1 < d2;
}

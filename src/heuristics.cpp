#include <heuristics.h>

float h1(point a, point b) {
    return 10.0 * sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

float h2(point a, point b) {
    return 10.0 * (abs(a.x - b.x) + abs(a.y - b.y));
}